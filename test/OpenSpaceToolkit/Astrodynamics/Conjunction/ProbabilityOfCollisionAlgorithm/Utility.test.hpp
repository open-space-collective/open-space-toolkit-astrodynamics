/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Utility_test__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Utility_test__

#include <array>
#include <cctype>
#include <ostream>
#include <string>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Path.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/IAU/Theory.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody/Spherical.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Uncertainty/Covariance.hpp>

/// @brief Build the state (J2000 position [m], velocity [m/s] and RTN position covariance [m^2]) of an object from a
/// table row
///
/// @details The state has no covariance if the covariance cells of the object are empty.
///
/// @param aRow A table row
/// @param anObjectPrefix The column prefix of the object (e.g. "OBJ_1")
/// @param anInstant The instant of the state (TCA)
/// @return The state of the object
inline ostk::astrodynamics::trajectory::State BuildJ2000StateWithRTNPositionCovariance(
    const ostk::core::container::table::Row& aRow,
    const ostk::core::type::String& anObjectPrefix,
    const ostk::physics::time::Instant& anInstant
)
{
    using ostk::core::type::Shared;
    using ostk::core::type::String;

    using ostk::mathematics::object::MatrixXd;
    using ostk::mathematics::object::VectorXd;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::coordinate::frame::provider::iau::Theory;

    using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
    using ostk::astrodynamics::trajectory::State;
    using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
    using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
    using ostk::astrodynamics::trajectory::StateBuilder;
    using ostk::astrodynamics::uncertainty::Covariance;

    const auto get = [&aRow, &anObjectPrefix](const String& aColumnSuffix) -> double
    {
        return aRow[String::Format("{}_{}", anObjectPrefix, aColumnSuffix)].accessReal();
    };

    // EME2000 (J2000)
    const Shared<const Frame> j2000FrameSPtr = Frame::J2000(Theory::IAU_2006);

    VectorXd coordinates(6);
    coordinates << get("POS_X"), get("POS_Y"), get("POS_Z"), get("VEL_X"), get("VEL_Y"), get("VEL_Z");

    const StateBuilder stateBuilder = {
        j2000FrameSPtr,
        {CartesianPosition::Default(), CartesianVelocity::Default()},
    };

    const State state = stateBuilder.build(anInstant, coordinates);

    // No position uncertainty if the covariance cells are empty (loaded as undefined objects)
    if (!aRow[String::Format("{}_COV_RR", anObjectPrefix)].isDefined())
    {
        return state;
    }

    // Symmetric covariance, filled from its upper triangle (e.g. COV_RT is the R-T element)
    const std::array<String, 3> axes = {"R", "T", "N"};

    MatrixXd covariance(3, 3);
    for (Eigen::Index i = 0; i < 3; ++i)
    {
        for (Eigen::Index j = i; j < 3; ++j)
        {
            covariance(i, j) = get(String::Format("COV_{}{}", axes[i], axes[j]));
            covariance(j, i) = covariance(i, j);
        }
    }

    // RTN frame (equivalent to the QSW frame) of the object
    const LocalOrbitalFrameFactory rtnFrameFactory = LocalOrbitalFrameFactory::QSW(j2000FrameSPtr);
    const Shared<const Frame> rtnFrameSPtr = rtnFrameFactory.generateFrame(state);

    return state.withCovariance(Covariance(anInstant, covariance, rtnFrameSPtr, {CartesianPosition::Default()}));
}

/// @brief Convert a test case name to a GoogleTest name, which can only contain alphanumeric characters and underscores
/// (e.g. "NASA CARA Frisbee Max Pc Test Case 1-12 | 70.0m HBR | No Object 1 Covariance" ->
/// "NASACARAFrisbeeMaxPcTestCase1_12_70_0mHBR_NoObject1Covariance")
///
/// @param aName A test case name
/// @return The GoogleTest name: spaces are removed, other non-alphanumeric characters are replaced by underscores
inline std::string ToTestName(const ostk::core::type::String& aName)
{
    std::string testName;

    for (const char character : aName)
    {
        if (character == ' ')
        {
            continue;
        }

        testName += std::isalnum(static_cast<unsigned char>(character)) ? character : '_';
    }

    return testName;
}

/// @brief Probability of collision test case: a close approach, the hard bodies of both objects and the expected
/// probability of collision.
///
/// @details This file is not compiled on its own: it is included by the probability of collision algorithm tests. All
/// member functions are defined in the struct body (hence inline), so that it can be included by several tests.
struct PoCTestCase
{
    ostk::core::type::String name;
    ostk::core::type::String comment;  ///< Contextual information (e.g. orbit regime, relative motion), possibly empty
    ostk::astrodynamics::conjunction::CloseApproach closeApproach;
    ostk::core::type::Shared<const ostk::astrodynamics::conjunction::HardBody> hardBody1SPtr;
    ostk::core::type::Shared<const ostk::astrodynamics::conjunction::HardBody> hardBody2SPtr;
    double expectedProbabilityOfCollision;

    /// @brief Get the description of the test case (e.g. for test failure messages)
    ///
    /// @return The name, followed by the comment in parentheses (if any)
    ostk::core::type::String getDescription() const
    {
        if (comment.isEmpty())
        {
            return name;
        }

        return ostk::core::type::String::Format("{} ({})", name, comment);
    }

    /// @brief Build the test cases from:
    ///
    /// Alfano, Salvatore. (2009). Satellite Conjunction Monte Carlo Analysis. AAS 09-233.
    ///
    /// @details Although most of these cases are low relative speed encounters (outside the short-term encounter
    /// assumption of 2D probability of collision methods), they were designed by Alfano as a benchmark for probability
    /// of collision methods, mixing linear and nonlinear relative motion. The expected values are those of the 2D
    /// probability of collision integral (not the Monte Carlo truth), hence these tests check the numerical
    /// integration rather than the physical validity of the methods.
    ///
    /// The data is loaded from Data/Alfano2009TestCases.csv (see Data/README.md). The combined hard-body radius is
    /// split evenly between both (spherical) hard bodies.
    ///
    /// @return The test cases (Alfano 2009 Test Case 1 to 11)
    static ostk::core::container::Array<PoCTestCase> BuildAlfano2009PoCTestCases()
    {
        using ostk::core::container::Array;
        using ostk::core::container::Table;
        using ostk::core::filesystem::File;
        using ostk::core::filesystem::Path;
        using ostk::core::type::String;

        using ostk::astrodynamics::conjunction::hardbody::Spherical;

        const Table table = Table::Load(
            File::Path(
                Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/"
                            "Data/Alfano2009TestCases.csv")
            ),
            Table::Format::CSV,
            true
        );

        Array<PoCTestCase> testCases = Array<PoCTestCase>::Empty();
        testCases.reserve(table.getRowCount());

        for (const auto& row : table)
        {
            const double combinedHardBodyRadius = row["COMBINED_HBR"].accessReal();

            // An empty comment cell is loaded as an undefined object
            const String comment = row["COMMENT"].isString() ? row["COMMENT"].accessString() : String::Empty();

            testCases.add({
                row["NAME"].accessString(),
                comment,
                {PoCTestCase::BuildState(row, "OBJ_1"), PoCTestCase::BuildState(row, "OBJ_2")},
                std::make_shared<Spherical>(combinedHardBodyRadius / 2.0),
                std::make_shared<Spherical>(combinedHardBodyRadius / 2.0),
                row["EXPECTED_POC"].accessReal(),
            });
        }

        return testCases;
    }

    /// @brief Build the maximum probability of collision test cases from NASA CARA Analysis Tools
    /// (FrisbeeMaxPc_UnitTest.m and the sample CDMs it reads): https://github.com/nasa/CARA_Analysis_Tools
    ///
    /// @details One test case per combination of geometry, combined hard-body radius and unknown covariance:
    /// - Frisbee example (NASA CARA tests 12-13): reproduces the example of Frisbee (2015), with only the Object 1
    /// covariance known.
    /// - Single covariance Cases 1-1 to 1-15 (NASA CARA tests 14-27): previously validated against Frisbee's data, with
    /// either covariance unknown in turn. The expected values are computed by NASA CARA over an equal-area square hard
    /// body (HBRType = 'squareEquArea').
    ///
    /// The expected probability of collision is the maximum probability of collision. As in NASA CARA, only the
    /// position covariances are used. The comment contains the ratio of the combined hard-body radius to the smaller
    /// standard deviation of the critical combined covariance (HBR / sigma_min).
    ///
    /// The data is loaded from Data/NASACARAFrisbeeMaxPcTestCases.csv (see Data/README.md). The combined hard-body
    /// radius is split evenly between both (spherical) hard bodies.
    ///
    /// @return The test cases
    static ostk::core::container::Array<PoCTestCase> BuildNASACARAFrisbeeMaxPcTestCases()
    {
        using ostk::core::container::Array;
        using ostk::core::container::Table;
        using ostk::core::filesystem::File;
        using ostk::core::filesystem::Path;
        using ostk::core::type::String;

        using ostk::physics::time::DateTime;
        using ostk::physics::time::Instant;
        using ostk::physics::time::Scale;

        using ostk::astrodynamics::conjunction::hardbody::Spherical;

        const Table table = Table::Load(
            File::Path(
                Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/"
                            "Data/NASACARAFrisbeeMaxPcTestCases.csv")
            ),
            Table::Format::CSV,
            true
        );

        Array<PoCTestCase> testCases = Array<PoCTestCase>::Empty();
        testCases.reserve(table.getRowCount());

        for (const auto& row : table)
        {
            const double combinedHardBodyRadius = row["COMBINED_HBR"].accessReal();

            // An empty comment cell is loaded as an undefined object
            const String comment = row["COMMENT"].isString() ? row["COMMENT"].accessString() : String::Empty();

            const Instant instant = Instant::DateTime(DateTime::Parse(row["TCA_UTC"].accessString()), Scale::UTC);

            testCases.add({
                row["NAME"].accessString(),
                comment,
                {BuildJ2000StateWithRTNPositionCovariance(row, "OBJ_1", instant),
                 BuildJ2000StateWithRTNPositionCovariance(row, "OBJ_2", instant)},
                std::make_shared<Spherical>(combinedHardBodyRadius / 2.0),
                std::make_shared<Spherical>(combinedHardBodyRadius / 2.0),
                row["EXPECTED_MAXIMUM_POC"].accessReal(),
            });
        }

        return testCases;
    }

    /// @brief Build the test cases from Orekit (Alfano2005Test.java, Chan1997Test.java, Laas2015Test.java and
    /// Patera2005Test.java), defined by their miss distance and combined position uncertainty in the rotated encounter
    /// frame: https://gitlab.orekit.org/orekit/orekit
    ///
    /// @details The test cases are the Chan, CSM, CDM and Alfano test cases of Orekit computing the probability of
    /// collision from xm, ym, sigmaX and sigmaY, with the expected probability of collision of each Orekit method in a
    /// dedicated column (e.g. EXPECTED_LAAS2015_POC). Each test case is expanded with its swapped version (xm and ym
    /// swapped, as well as sigmaX and sigmaY). An artificial close approach yielding these rotated encounter frame
    /// parameters is built for each of them (see BuildCloseApproachFromRotatedEncounterFrameParams).
    ///
    /// The data is loaded from Data/OrekitRotatedEncounterFrameTestCases.csv (see Data/README.md). The combined
    /// hard-body radius is split evenly between both (spherical) hard bodies.
    ///
    /// @code{.cpp}
    ///     Array<PoCTestCase> testCases =
    ///     PoCTestCase::BuildOrekitRotatedEncounterFrameTestCases("EXPECTED_LAAS2015_POC") ;
    /// @endcode
    ///
    /// @param anExpectedProbabilityOfCollisionColumnName The name of the column of the expected probability of
    /// collision (EXPECTED_ALFANO2005_POC, EXPECTED_CHAN1997_POC, EXPECTED_LAAS2015_POC or EXPECTED_PATERA2005_POC)
    /// @return The test cases
    static ostk::core::container::Array<PoCTestCase> BuildOrekitRotatedEncounterFrameTestCases(
        const ostk::core::type::String& anExpectedProbabilityOfCollisionColumnName
    )
    {
        using ostk::core::container::Array;
        using ostk::core::container::Table;
        using ostk::core::filesystem::File;
        using ostk::core::filesystem::Path;
        using ostk::core::type::String;

        using ostk::astrodynamics::conjunction::hardbody::Spherical;

        const Table table = Table::Load(
            File::Path(
                Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/"
                            "Data/OrekitRotatedEncounterFrameTestCases.csv")
            ),
            Table::Format::CSV,
            true
        );

        Array<PoCTestCase> testCases = Array<PoCTestCase>::Empty();
        testCases.reserve(table.getRowCount());

        for (const auto& row : table)
        {
            const double combinedHardBodyRadius = row["COMBINED_HBR"].accessReal();

            testCases.add({
                row["NAME"].accessString(),
                String::Empty(),
                PoCTestCase::BuildCloseApproachFromRotatedEncounterFrameParams(
                    row["X_M"].accessReal(),
                    row["Y_M"].accessReal(),
                    row["SIGMA_X"].accessReal(),
                    row["SIGMA_Y"].accessReal()
                ),
                std::make_shared<Spherical>(combinedHardBodyRadius / 2.0),
                std::make_shared<Spherical>(combinedHardBodyRadius / 2.0),
                row[anExpectedProbabilityOfCollisionColumnName].accessReal(),
            });
        }

        return testCases;
    }

   private:
    /// @brief Build a close approach from its miss distance and combined position uncertainty in the rotated encounter
    /// frame (see CloseApproach::computeRotatedEncounterFrameParameters)
    ///
    /// @details Infinitely many close approaches yield the same rotated encounter frame parameters. This one is built
    /// at J2000 in GCRF as follows:
    /// - Object 1 is at (10000 km, 0, 0) with a velocity of (0, 10 km/s, 0): its QSW (RTN) frame is aligned with GCRF
    /// (R = X, T = Y, N = Z).
    /// - Object 2 has a velocity of (0, -10 km/s, 0): the relative velocity is along -T, hence the encounter plane is
    /// the R-N plane.
    /// - Object 1 position covariance (in its QSW frame) is diag(sigmaX^2, 1, sigmaY^2): the T variance is along the
    /// relative velocity, hence it is projected out of the encounter plane.
    /// - Object 2 position covariance is zero (positive semi-definite, which Covariance accepts), so that the combined
    /// covariance is the Object 1 covariance: its principal axes in the encounter plane are R (sigmaX) and N (sigmaY).
    /// - Object 2 is at (xM, 0, yM) in the Object 1 QSW frame: the relative position is perpendicular to the relative
    /// velocity (i.e. at TCA), with components xM and yM along the principal axes.
    ///
    /// @param aMissDistanceX The miss distance along the x principal axis (xM) [m]
    /// @param aMissDistanceY The miss distance along the y principal axis (yM) [m]
    /// @param aSigmaX The combined standard deviation along the x principal axis [m]
    /// @param aSigmaY The combined standard deviation along the y principal axis [m]
    /// @return The close approach
    static ostk::astrodynamics::conjunction::CloseApproach BuildCloseApproachFromRotatedEncounterFrameParams(
        double aMissDistanceX, double aMissDistanceY, double aSigmaX, double aSigmaY
    )
    {
        using ostk::core::type::Shared;

        using ostk::mathematics::object::MatrixXd;
        using ostk::mathematics::object::Vector3d;
        using ostk::mathematics::object::VectorXd;

        using ostk::physics::coordinate::Frame;
        using ostk::physics::time::Instant;

        using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
        using ostk::astrodynamics::trajectory::State;
        using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
        using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
        using ostk::astrodynamics::trajectory::StateBuilder;
        using ostk::astrodynamics::uncertainty::Covariance;

        const Instant instant = Instant::J2000();
        const Shared<const Frame> gcrfFrameSPtr = Frame::GCRF();

        const StateBuilder stateBuilder = {
            gcrfFrameSPtr,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        const auto buildState = [&stateBuilder, &instant](const Vector3d& aPosition, const Vector3d& aVelocity) -> State
        {
            VectorXd coordinates(6);
            coordinates << aPosition, aVelocity;

            return stateBuilder.build(instant, coordinates);
        };

        // Object 1: its QSW (RTN) frame is aligned with GCRF (R = X, T = Y, N = Z)
        const Vector3d object1Position = {10.0e6, 0.0, 0.0};
        const Vector3d object1Velocity = {0.0, 10.0e3, 0.0};

        // Object 2: relative velocity along -T (encounter plane = R-N plane), relative position (xM, 0, yM) in RTN
        const Vector3d object2Position = object1Position + Vector3d(aMissDistanceX, 0.0, aMissDistanceY);
        const Vector3d object2Velocity = {0.0, -10.0e3, 0.0};

        const State object1State = buildState(object1Position, object1Velocity);
        const State object2State = buildState(object2Position, object2Velocity);

        // Object 1 position covariance in RTN: sigmaX along R, sigmaY along N (the T variance plays no role)
        MatrixXd object1PositionCovariance = MatrixXd::Zero(3, 3);
        object1PositionCovariance(0, 0) = aSigmaX * aSigmaX;
        object1PositionCovariance(1, 1) = 1.0;
        object1PositionCovariance(2, 2) = aSigmaY * aSigmaY;

        // Object 2 position covariance: zero, so that the combined covariance is the Object 1 covariance
        const MatrixXd object2PositionCovariance = MatrixXd::Zero(3, 3);

        const LocalOrbitalFrameFactory qswFrameFactory = LocalOrbitalFrameFactory::QSW(gcrfFrameSPtr);

        return {
            object1State.withCovariance(Covariance(
                instant,
                object1PositionCovariance,
                qswFrameFactory.generateFrame(object1State),
                {CartesianPosition::Default()}
            )),
            object2State.withCovariance(Covariance(
                instant,
                object2PositionCovariance,
                qswFrameFactory.generateFrame(object2State),
                {CartesianPosition::Default()}
            )),
        };
    }

    /// @brief Build the state (GCRF position [m], velocity [m/s] and position-velocity covariance [m, s]) of an object
    /// at J2000 from a table row
    ///
    /// @param aRow A table row
    /// @param anObjectPrefix The column prefix of the object (e.g. "OBJ_1")
    /// @return The state of the object
    static ostk::astrodynamics::trajectory::State BuildState(
        const ostk::core::container::table::Row& aRow, const ostk::core::type::String& anObjectPrefix
    )
    {
        using ostk::core::type::Shared;
        using ostk::core::type::String;

        using ostk::mathematics::object::MatrixXd;
        using ostk::mathematics::object::VectorXd;

        using ostk::physics::coordinate::Frame;
        using ostk::physics::time::Instant;

        using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
        using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
        using ostk::astrodynamics::trajectory::StateBuilder;
        using ostk::astrodynamics::uncertainty::Covariance;

        const auto get = [&aRow, &anObjectPrefix](const String& aColumnSuffix) -> double
        {
            return aRow[String::Format("{}_{}", anObjectPrefix, aColumnSuffix)].accessReal();
        };

        const Instant instant = Instant::J2000();
        const Shared<const Frame> gcrfFrameSPtr = Frame::GCRF();

        VectorXd coordinates(6);
        coordinates << get("POS_X"), get("POS_Y"), get("POS_Z"), get("VEL_X"), get("VEL_Y"), get("VEL_Z");

        // Symmetric covariance, filled from its upper triangle (e.g. COV_XVY is the X-VY element)
        const std::array<String, 6> axes = {"X", "Y", "Z", "VX", "VY", "VZ"};

        MatrixXd covariance(6, 6);
        for (Eigen::Index i = 0; i < 6; ++i)
        {
            for (Eigen::Index j = i; j < 6; ++j)
            {
                covariance(i, j) = get(String::Format("COV_{}{}", axes[i], axes[j]));
                covariance(j, i) = covariance(i, j);
            }
        }

        const StateBuilder stateBuilder = {
            gcrfFrameSPtr,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        return stateBuilder.build(instant, coordinates)
            .withCovariance(Covariance(
                instant, covariance, gcrfFrameSPtr, {CartesianPosition::Default(), CartesianVelocity::Default()}
            ));
    }
};

/// @brief Probability of collision analysis test case: a close approach, the hard bodies of both objects, and the
/// expected probability region and maximum probability of collision.
///
/// @details This file is not compiled on its own: it is included by the probability of collision algorithm tests. All
/// member functions are defined in the struct body (hence inline), so that it can be included by several tests.
struct PoCAnalysisTestCase
{
    ostk::core::type::String name;
    ostk::core::type::String comment;  ///< Contextual information (e.g. source, covariance), possibly empty
    ostk::astrodynamics::conjunction::CloseApproach closeApproach;
    ostk::core::type::Shared<const ostk::astrodynamics::conjunction::HardBody> hardBody1SPtr;
    ostk::core::type::Shared<const ostk::astrodynamics::conjunction::HardBody> hardBody2SPtr;
    ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm::ProbabilityRegion expectedRegion;
    double expectedMaximumProbabilityOfCollision;

    /// @brief Get the description of the test case (e.g. for test failure messages)
    ///
    /// @return The name, followed by the comment in parentheses (if any)
    ostk::core::type::String getDescription() const
    {
        if (comment.isEmpty())
        {
            return name;
        }

        return ostk::core::type::String::Format("{} ({})", name, comment);
    }

    /// @brief Build the analysis test cases from NASA CARA Analysis Tools (DilutionMaxPc_UnitTest.m, Omitron CDM test
    /// cases): https://github.com/nasa/CARA_Analysis_Tools
    ///
    /// @details As in NASA CARA, only the position covariances are used.
    ///
    /// NASA CARA scales both covariances by the square of its scale factor (i.e. its scale factor applies to the
    /// standard deviations), whereas the scaling factor of ComputeProbabilityAnalysis applies to the covariances: the
    /// expected maximum probabilities of collision are independent of this convention, the scale factors at which they
    /// occur are not.
    ///
    /// The data is loaded from Data/NASACARADilutionTestCases.csv (see Data/README.md). The combined hard-body radius
    /// is split evenly between both (spherical) hard bodies.
    ///
    /// @return The analysis test cases (NASA CARA Dilution Test Case 1 to 5)
    static ostk::core::container::Array<PoCAnalysisTestCase> BuildNASACARADilutionPoCAnalysisTestCases()
    {
        using ostk::core::container::Array;
        using ostk::core::container::Table;
        using ostk::core::filesystem::File;
        using ostk::core::filesystem::Path;
        using ostk::core::type::String;

        using ostk::physics::time::DateTime;
        using ostk::physics::time::Instant;
        using ostk::physics::time::Scale;

        using ostk::astrodynamics::conjunction::hardbody::Spherical;
        using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;

        const Table table = Table::Load(
            File::Path(
                Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/"
                            "Data/NASACARADilutionTestCases.csv")
            ),
            Table::Format::CSV,
            true
        );

        const auto parseRegion = [](const String& aRegion) -> ProbabilityOfCollisionAlgorithm::ProbabilityRegion
        {
            if (aRegion == "Robust")
            {
                return ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Robust;
            }

            if (aRegion == "Diluted")
            {
                return ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted;
            }

            if (aRegion == "Unknown")
            {
                return ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Unknown;
            }

            throw ostk::core::error::runtime::Wrong("Probability region", aRegion);
        };

        Array<PoCAnalysisTestCase> testCases = Array<PoCAnalysisTestCase>::Empty();
        testCases.reserve(table.getRowCount());

        for (const auto& row : table)
        {
            const double combinedHardBodyRadius = row["COMBINED_HBR"].accessReal();

            // An empty comment cell is loaded as an undefined object
            const String comment = row["COMMENT"].isString() ? row["COMMENT"].accessString() : String::Empty();

            const Instant instant = Instant::DateTime(DateTime::Parse(row["TCA_UTC"].accessString()), Scale::UTC);

            testCases.add({
                row["NAME"].accessString(),
                comment,
                {BuildJ2000StateWithRTNPositionCovariance(row, "OBJ_1", instant),
                 BuildJ2000StateWithRTNPositionCovariance(row, "OBJ_2", instant)},
                std::make_shared<Spherical>(combinedHardBodyRadius / 2.0),
                std::make_shared<Spherical>(combinedHardBodyRadius / 2.0),
                parseRegion(row["EXPECTED_REGION"].accessString()),
                row["EXPECTED_MAXIMUM_POC"].accessReal(),
            });
        }

        return testCases;
    }
};

/// @brief Print a test case (used by GoogleTest, e.g. in parameterized test failure messages)
///
/// @param aTestCase A test case
/// @param anOutputStream An output stream
inline void PrintTo(const PoCTestCase& aTestCase, std::ostream* anOutputStream)
{
    *anOutputStream << aTestCase.getDescription();
}

/// @brief Print an analysis test case (used by GoogleTest, e.g. in parameterized test failure messages)
///
/// @param aTestCase An analysis test case
/// @param anOutputStream An output stream
inline void PrintTo(const PoCAnalysisTestCase& aTestCase, std::ostream* anOutputStream)
{
    *anOutputStream << aTestCase.getDescription();
}

#endif
