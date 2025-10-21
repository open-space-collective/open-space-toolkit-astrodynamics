/// Apache License 2.0

#include <boost/numeric/odeint.hpp>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/Exponential.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Ephemeris/Analytical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Table;
using ostk::core::container::Tuple;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::ephemeris::Analytical;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::environment::object::celestial::Moon;
using ostk::physics::environment::object::celestial::Sun;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Mass;
using ostk::physics::unit::Time;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::guidancelaw::ConstantThrust;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameDirection;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameTransformProvider;
using ostk::astrodynamics::trajectory::state::NumericalSolver;
using ostk::astrodynamics::trajectory::StateBuilder;

class OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ConstantThrust : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        this->defaultSatelliteSystem_ = {
            satelliteDryMass_, satelliteGeometry, Matrix3d::Identity(), 1.2, 2.1, defaultPropulsionSystem_
        };
    }

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    const Shared<const LocalOrbitalFrameFactory> localOrbitalFrameFactorySPtr_ =
        LocalOrbitalFrameFactory::VNC(gcrfSPtr_);
    LocalOrbitalFrameDirection localOrbitalFrameDirection_ = {
        {1.0, 0.0, 0.0},
        localOrbitalFrameFactorySPtr_,
    };

    const ConstantThrust defaultConstantThrust_ = {localOrbitalFrameDirection_};

    const PropulsionSystem defaultPropulsionSystem_ = {0.1, 1500.0};
    const Mass satelliteDryMass_ = Mass::Kilograms(100.0);

    SatelliteSystem defaultSatelliteSystem_ = SatelliteSystem::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ConstantThrust, Constructor)

{
    {
        EXPECT_NO_THROW(ConstantThrust constantThrust(localOrbitalFrameDirection_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ConstantThrust, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultConstantThrust_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ConstantThrust, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultConstantThrust_.print(std::cout, true));
        EXPECT_NO_THROW(defaultConstantThrust_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ConstantThrust, Getters)
{
    {
        // Not checking equality due to precision loss.
        EXPECT_NO_THROW(defaultConstantThrust_.getLocalThrustDirection());
    }

    {
        ConstantThrust constantThrust(LocalOrbitalFrameDirection::Undefined());
        EXPECT_THROW(constantThrust.getLocalThrustDirection(), ostk::core::error::runtime::Undefined);
    }
}

/* Include Thruster Dynamics computeContribution validation test */
TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ConstantThrust, calculateThrustAccelerationAt)
{
    {
        // Test Case (thrust direction, local orbital frame, reference data)
        const Array<Tuple<Shared<const LocalOrbitalFrameFactory>, Vector3d, String>> testCases = {
            {LocalOrbitalFrameFactory::VNC(gcrfSPtr_),
             Vector3d({1.0, 0.0, 0.0}),
             "Orekit_ConstantThrustManeuver_VNC_inclined_1hr_run.csv"},
            {LocalOrbitalFrameFactory::QSW(gcrfSPtr_),
             Vector3d({0.0, 1.0, 0.0}),
             "Orekit_ConstantThrustManeuver_QSW_inclined_1hr_run.csv"},
            {LocalOrbitalFrameFactory::TNW(gcrfSPtr_),
             Vector3d({1.0, 0.0, 0.0}),
             "Orekit_ConstantThrustManeuver_TNW_inclined_1hr_run.csv"},
            {LocalOrbitalFrameFactory::LVLH(gcrfSPtr_),
             Vector3d({1.0, 0.0, 0.0}),
             "Orekit_ConstantThrustManeuver_LVLH_inclined_1hr_run.csv"},
            {LocalOrbitalFrameFactory::VVLH(gcrfSPtr_),
             Vector3d({1.0, 0.0, 0.0}),
             "Orekit_ConstantThrustManeuver_VVLH_inclined_1hr_run.csv"}
        };

        // Loop through test cases
        for (const auto& testCase : testCases)
        {
            // Extract test case input data
            const Shared<const LocalOrbitalFrameFactory> localOrbitalFrameFactory = std::get<0>(testCase);
            const Vector3d localOrbitalFrameThrustVector = std::get<1>(testCase);
            const String referenceDataFileName = std::get<2>(testCase);

            // Define constant thrust thruster dynamics
            LocalOrbitalFrameDirection localOrbitalFrameDirection =
                LocalOrbitalFrameDirection(localOrbitalFrameThrustVector, localOrbitalFrameFactory);
            const Shared<Thruster> thrusterDynamicsSPtr = std::make_shared<Thruster>(
                defaultSatelliteSystem_, std::make_shared<ConstantThrust>(localOrbitalFrameDirection)
            );

            // Reference data setup
            const Table referenceData = Table::Load(
                File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/GuidanceLaw/" + referenceDataFileName)
                ),
                Table::Format::CSV,
                true
            );

            // Initialize reference data arrays
            Array<Instant> instantArray = Array<Instant>::Empty();
            Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
            Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();
            Array<double> referenceMassArray = Array<double>::Empty();
            Array<Vector3d> referenceManeuverAccelerationArrayGCRF = Array<Vector3d>::Empty();

            for (const auto& referenceRow : referenceData)
            {
                instantArray.add(Instant::DateTime(
                    DateTime::Parse(referenceRow[0].accessString(), DateTime::Format::ISO8601), Scale::UTC
                ));
                referencePositionArrayGCRF.add(
                    Vector3d(referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal())
                );
                referenceVelocityArrayGCRF.add(
                    Vector3d(referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal())
                );
                referenceManeuverAccelerationArrayGCRF.add(Vector3d(
                    referenceRow[16].accessReal(), referenceRow[17].accessReal(), referenceRow[18].accessReal()
                ));
                referenceMassArray.add(referenceRow[22].accessReal());
            }

            // Validation loop
            for (size_t i = 1; i < instantArray.getSize() - 1; i++)
            {
                // Current state and instant setup from Orekit test case
                VectorXd OrekitStateCoordinates(7);
                OrekitStateCoordinates << referencePositionArrayGCRF[i], referenceVelocityArrayGCRF[i],
                    referenceMassArray[i];  // Check size input Dynamics

                const VectorXd maneuverContributionFromOrekitStateGCRF =
                    thrusterDynamicsSPtr->computeContribution(instantArray[i], OrekitStateCoordinates, gcrfSPtr_);

                const double referenceStepSize = 30.0;

                const double maneuverComputationErrorAccelerationContributionXGCRF =
                    std::abs(maneuverContributionFromOrekitStateGCRF[0] - referenceManeuverAccelerationArrayGCRF[i][0]);
                const double maneuverComputationErrorAccelerationContributionYGCRF =
                    std::abs(maneuverContributionFromOrekitStateGCRF[1] - referenceManeuverAccelerationArrayGCRF[i][1]);
                const double maneuverComputationErrorAccelerationContributionZGCRF =
                    std::abs(maneuverContributionFromOrekitStateGCRF[2] - referenceManeuverAccelerationArrayGCRF[i][2]);
                const double maneuverComputationErrorMassContributionGCRF = std::abs(
                    maneuverContributionFromOrekitStateGCRF[3] -
                    ((referenceMassArray[i + 1] - referenceMassArray[i]) / referenceStepSize)
                );
                const double maneuverComputationAccelerationContributionErrorGCRF =
                    (maneuverContributionFromOrekitStateGCRF.segment(0, 3) - referenceManeuverAccelerationArrayGCRF[i])
                        .norm();

                ASSERT_GT(5e-10, maneuverComputationErrorAccelerationContributionXGCRF);
                ASSERT_GT(5e-10, maneuverComputationErrorAccelerationContributionYGCRF);
                ASSERT_GT(5e-10, maneuverComputationErrorAccelerationContributionZGCRF);
                ASSERT_GT(1e-9, maneuverComputationErrorMassContributionGCRF);
                ASSERT_GT(1e-9, maneuverComputationAccelerationContributionErrorGCRF);

                // Results console output
                // std::cout << "**************************************" << std::endl;
                // std::cout.setf(std::ios::scientific,std::ios::floatfield);
                // std::cout << "Instant is: " << instantArray[i] << std::endl;
                // std::cout << "Maneuver contribution computation X GCRF error is: " <<
                // maneuverComputationErrorAccelerationContributionXGCRF << "m/s^2" << std::endl; std::cout <<
                // "Maneuver
                //     // contribution computation Y GCRF error is: " <<
                //     maneuverComputationErrorAccelerationContributionYGCRF
                // << "m/s^2" << std::endl; std::cout << "Maneuver contribution computation Z GCRF error is: " <<
                // maneuverComputationErrorAccelerationContributionZGCRF << "m/s^2" << std::endl; std::cout <<
                // "Maneuver
                //     // contribution computation Mass error is: " << maneuverComputationErrorMassContributionGCRF <<
                //     "kg"
                //     <<
                // std::endl; std::cout << "Maneuver contribution computation Acceleration GCRF Norm error is: " <<
                // maneuverComputationAccelerationContributionErrorGCRF << "m/s^2" << std::endl;
                // std::cout.setf(std::ios::fixed,std::ios::floatfield);
                // std::cout << "**************************************" << std::endl;
            }
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ConstantThrust, Intrack)
{
    {
        ConstantThrust constantThrust = ConstantThrust::Intrack(true);
        Vector3d direction = {1.0, 0.0, 0.0};
        EXPECT_TRUE(constantThrust.getLocalThrustDirection().getValue() == direction);
        EXPECT_TRUE(
            constantThrust.getLocalThrustDirection().accessLocalOrbitalFrameFactory()->getProviderType() ==
            LocalOrbitalFrameTransformProvider::Type::VNC
        );
    }

    {
        Vector3d direction = {-1.0, 0.0, 0.0};
        ConstantThrust constantThrust = ConstantThrust::Intrack(false);
        EXPECT_TRUE(constantThrust.getLocalThrustDirection().getValue() == direction);
        EXPECT_TRUE(
            constantThrust.getLocalThrustDirection().accessLocalOrbitalFrameFactory()->getProviderType() ==
            LocalOrbitalFrameTransformProvider::Type::VNC
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ConstantThrust, FromManeuver)
{
    const StateBuilder stateBuilder = StateBuilder(gcrfSPtr_, Maneuver::RequiredCoordinateSubsets);
    const Maneuver maneuver = Maneuver({
        stateBuilder.build(
            Instant::J2000() + Duration::Seconds(0.0),
            (VectorXd(10) << 7.0e6, 0.0, 0.0, 0.0, 8.0e3, 0.0, 1.0, 2.0, 3.0, -1.0e-5).finished()
        ),
        stateBuilder.build(
            Instant::J2000() + Duration::Seconds(10.0),
            (VectorXd(10) << 0.0, 7.0e6, 0.0, -8.0e3, 0.0, 0.0, 4.0, 5.0, 6.0, -1.0e-5).finished()
        ),
    });
    const Maneuver::MeanDirectionAndMaximumAngularOffset meanDirectionAndMaximumAngularOffset =
        maneuver.calculateMeanThrustDirectionAndMaximumAngularOffset(localOrbitalFrameFactorySPtr_);

    {
        EXPECT_THROW(
            try {
                ConstantThrust::FromManeuver(maneuver, LocalOrbitalFrameFactory::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Local Orbital Frame Factory} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    // Not considering maximum allowed angular offset for now
    {
        ConstantThrust constantThrust = ConstantThrust::FromManeuver(maneuver, localOrbitalFrameFactorySPtr_);
        EXPECT_EQ(
            constantThrust.getLocalThrustDirection().getValue(), meanDirectionAndMaximumAngularOffset.first.getValue()
        );
        EXPECT_EQ(
            constantThrust.getLocalThrustDirection().accessLocalOrbitalFrameFactory(), localOrbitalFrameFactorySPtr_
        );
    }

    // Considering a maximum allowed angular offset, but it's not violated
    {
        ConstantThrust constantThrust = ConstantThrust::FromManeuver(
            maneuver,
            localOrbitalFrameFactorySPtr_,
            Angle::Degrees(1.1 * meanDirectionAndMaximumAngularOffset.second.inDegrees(0.0, 360.0))
        );
        EXPECT_EQ(
            constantThrust.getLocalThrustDirection().getValue(), meanDirectionAndMaximumAngularOffset.first.getValue()
        );
        EXPECT_EQ(
            constantThrust.getLocalThrustDirection().accessLocalOrbitalFrameFactory(), localOrbitalFrameFactorySPtr_
        );
    }

    // Considering a maximum allowed angular offset, and it's violated
    {
        EXPECT_THROW(
            try {
                ConstantThrust::FromManeuver(
                    maneuver,
                    localOrbitalFrameFactorySPtr_,
                    Angle::Degrees(0.9 * meanDirectionAndMaximumAngularOffset.second.inDegrees(0.0, 360.0))
                );
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_NE(e.getMessage().find("Maximum angular offset"), std::string::npos);
                EXPECT_NE(e.getMessage().find(" is greater than the maximum allowed ("), std::string::npos);
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
}
