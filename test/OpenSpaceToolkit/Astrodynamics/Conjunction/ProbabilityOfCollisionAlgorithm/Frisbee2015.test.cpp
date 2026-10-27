/// Apache License 2.0

#include <algorithm>
#include <cmath>
#include <optional>
#include <utility>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/IAU/Theory.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody/Spherical.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Alfano2005.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Frisbee2015.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Uncertainty/Covariance.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::frame::provider::iau::Theory;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::hardbody::Spherical;
using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
using ostk::astrodynamics::conjunction::probabilityofcollisionalgorithm::Alfano2005;
using ostk::astrodynamics::conjunction::probabilityofcollisionalgorithm::Frisbee2015;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::StateBuilder;
using ostk::astrodynamics::uncertainty::Covariance;

namespace
{

State buildState(
    const Instant& anInstant,
    const Shared<const Frame>& aFrameSPtr,
    const Vector3d& aPositionInMeters,
    const Vector3d& aVelocityInMetersPerSecond
)
{
    const StateBuilder stateBuilder = {
        aFrameSPtr,
        {CartesianPosition::Default(), CartesianVelocity::Default()},
    };

    VectorXd coordinates(6);
    coordinates << aPositionInMeters, aVelocityInMetersPerSecond;

    return stateBuilder.build(anInstant, coordinates);
}

MatrixXd buildRTNPositionCovarianceCoordinates(const std::initializer_list<std::initializer_list<double>>& aRowList)
{
    MatrixXd coordinates(3, 3);

    Eigen::Index rowIndex = 0;
    for (const std::initializer_list<double>& row : aRowList)
    {
        Eigen::Index columnIndex = 0;
        for (const double value : row)
        {
            coordinates(rowIndex, columnIndex++) = value;
        }
        ++rowIndex;
    }

    return coordinates;
}

CloseApproach buildCloseApproach(
    const Instant& anInstant,
    const Vector3d& anObject1PositionInKilometers,
    const Vector3d& anObject1VelocityInKilometersPerSecond,
    const MatrixXd& anObject1RTNPositionCovariance,
    const Vector3d& anObject2PositionInKilometers,
    const Vector3d& anObject2VelocityInKilometersPerSecond,
    const MatrixXd& anObject2RTNPositionCovariance
)
{
    const Shared<const Frame> j2000FrameSPtr = Frame::J2000(Theory::IAU_2006);
    const LocalOrbitalFrameFactory rtnFrameFactory = LocalOrbitalFrameFactory::QSW(j2000FrameSPtr);

    State object1State = buildState(
        anInstant, j2000FrameSPtr, anObject1PositionInKilometers * 1e3, anObject1VelocityInKilometersPerSecond * 1e3
    );
    State object2State = buildState(
        anInstant, j2000FrameSPtr, anObject2PositionInKilometers * 1e3, anObject2VelocityInKilometersPerSecond * 1e3
    );

    // RTN frames (equivalent to QSW frames)
    object1State = object1State.withCovariance(Covariance(
        anInstant,
        anObject1RTNPositionCovariance,
        rtnFrameFactory.generateFrame(object1State),
        {CartesianPosition::Default()}
    ));
    object2State = object2State.withCovariance(Covariance(
        anInstant,
        anObject2RTNPositionCovariance,
        rtnFrameFactory.generateFrame(object2State),
        {CartesianPosition::Default()}
    ));

    return {object1State, object2State};
}

State removeCovariance(const State& aState)
{
    return buildState(
        aState.accessInstant(),
        aState.accessFrame(),
        aState.getPosition().getCoordinates(),
        aState.getVelocity().getCoordinates()
    );
}

// Equivalent to NASA CARA passing a zero covariance for Object 1
CloseApproach removeObject1Covariance(const CloseApproach& aCloseApproach)
{
    return {removeCovariance(aCloseApproach.getObject1State()), aCloseApproach.getObject2State()};
}

// Equivalent to NASA CARA passing a zero covariance for Object 2
CloseApproach removeObject2Covariance(const CloseApproach& aCloseApproach)
{
    return {aCloseApproach.getObject1State(), removeCovariance(aCloseApproach.getObject2State())};
}

bool containsMessage(const Array<String>& aMessageArray, const String& aMessageCore)
{
    return std::any_of(
        aMessageArray.begin(),
        aMessageArray.end(),
        [&aMessageCore](const String& aMessage) -> bool
        {
            return aMessage.find(aMessageCore) != std::string::npos;
        }
    );
}

// Test cases from NASA CARA Analysis Tools (FrisbeeMaxPc_UnitTest.m and the sample CDMs it reads):
// https://github.com/nasa/CARA_Analysis_Tools
//
// The data (EME2000 position [km] and velocity [km/s], RTN position covariance [m^2] at TCA) are taken from the CDMs.
// As in NASA CARA, only the position covariances are used.
//
// - Frisbee example (tests 12-13): reproduces the example of Frisbee (2015), with only the Object 1 covariance known.
// - Single covariance Cases 1-1 to 1-15 (tests 14-27): previously validated against Frisbee's data, with either
// covariance unknown in turn. The expected values are computed by NASA CARA over an equal-area square hard body
// (HBRType = 'squareEquArea'), whereas Frisbee2015 integrates over a circle: both agree while the combined hard-body
// radius is small with respect to the smaller standard deviation of the critical combined covariance (sigma_min),
// and the circle yields up to ~12% more beyond. Each case is checked with the closed form (Equation 5), which itself
// degrades as HBR / sigma_min grows, and with Alfano 2005 numerical integration, with per-case tolerances.

// Manufactured case corresponding to Frisbee's example (FrisbeeMaxPcTestCase_Test01.cdm), at TCA
CloseApproach buildFrisbeeExample()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2008-06-27 15:34:55.320"), Scale::UTC),
        Vector3d(-1818.269382, 1040.563930, -6772.707308),
        Vector3d(-3.609802798, 6.269245755, 1.933211527),
        buildRTNPositionCovarianceCoordinates(
            {{9.406873876577049e+03, -1.859415720232862e+04, -6.767206137928272e+04},
             {-1.859415720232862e+04, 5.124346486116677e+04, 1.825065889128269e+05},
             {-6.767206137928272e+04, 1.825065889128269e+05, 6.654558817959293e+05}}
        ),
        Vector3d(-1819.008023, 1039.894591, -6772.922705),
        Vector3d(6.316590032, -3.383797284, -2.177018239),
        buildRTNPositionCovarianceCoordinates(
            {{8.319951133221251e+04, -7.211164999897055e+04, 2.634915390598247e+05},
             {-7.211164999897055e+04, 7.078754316780070e+04, -2.260293616776890e+05},
             {2.634915390598247e+05, -2.260293616776890e+05, 8.351383766006979e+05}}
        )
    );
}

// Single covariance Case 1-1 (SingleCovTestCase1-1.cdm), at TCA
CloseApproach buildSingleCovCase1_01()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2014-01-24 15:59:51.345"), Scale::UTC),
        Vector3d(3691.5759, 3199.626, -4731.7639),
        Vector3d(-2.2891, 6.749, 2.7828),
        buildRTNPositionCovarianceCoordinates(
            {{5.957000E+02, -1.212000E+05, -3.795000E+01},
             {-1.212000E+05, 2.665000E+07, 1.201000E+04},
             {-3.795000E+01, 1.201000E+04, 7.760000E+01}}
        ),
        Vector3d(3693.3994, 3219.8685, -4714.9617),
        Vector3d(3.5385, 3.9764, 5.4903),
        buildRTNPositionCovarianceCoordinates(
            {{6.595000E+06, -1.391000E+09, -2.360000E+05},
             {-1.391000E+09, 2.934000E+11, 4.971000E+07},
             {-2.360000E+05, 4.971000E+07, 9.600000E+03}}
        )
    );
}

// Single covariance Case 1-3 (SingleCovTestCase1-3.cdm), at TCA
CloseApproach buildSingleCovCase1_03()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2009-01-22 05:19:40.203"), Scale::UTC),
        Vector3d(3545.9661, 2716.7773, 5037.8009),
        Vector3d(-5.9855, 4.4875, 1.7942),
        buildRTNPositionCovarianceCoordinates(
            {{6.121000E+01, -5.475000E+01, 1.644000E+01},
             {-5.475000E+01, 1.596000E+03, -2.293000E+01},
             {1.644000E+01, -2.293000E+01, 2.736000E+01}}
        ),
        Vector3d(3562.8819, 2717.6225, 5027.3141),
        Vector3d(-3.4545, -4.7555, 5.1328),
        buildRTNPositionCovarianceCoordinates(
            {{8.802000E+03, -1.717000E+03, -4.439000E+03},
             {-1.717000E+03, 1.529000E+05, -7.264000E+01},
             {-4.439000E+03, -7.264000E+01, 2.958000E+03}}
        )
    );
}

// Single covariance Case 1-4 (SingleCovTestCase1-4.cdm), at TCA
CloseApproach buildSingleCovCase1_04()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2010-07-23 15:40:01.250"), Scale::UTC),
        Vector3d(-2744.4595, -5759.1668, -2191.835),
        Vector3d(5.3618, -0.46653, -5.4933),
        buildRTNPositionCovarianceCoordinates(
            {{6.600000E+02, 0.000000E+00, 0.000000E+00},
             {0.000000E+00, 7.940000E+06, 0.000000E+00},
             {0.000000E+00, 0.000000E+00, 2.200000E+02}}
        ),
        Vector3d(-2739.8759, -5758.1307, -2204.4499),
        Vector3d(-0.01159, 2.7251, -7.1836),
        buildRTNPositionCovarianceCoordinates(
            {{3.300000E+04, 0.000000E+00, 0.000000E+00},
             {0.000000E+00, 8.700000E+06, 0.000000E+00},
             {0.000000E+00, 0.000000E+00, 5.100000E+02}}
        )
    );
}

// Single covariance Case 1-5 (SingleCovTestCase1-5.cdm), at TCA
CloseApproach buildSingleCovCase1_05()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2010-07-23 14:08:05.000"), Scale::UTC),
        Vector3d(-2733.6954, -5772.9162, -2169.3621),
        Vector3d(5.3499, -0.46672, -5.5048),
        buildRTNPositionCovarianceCoordinates(
            {{7.900000E+02, 0.000000E+00, 0.000000E+00},
             {0.000000E+00, 3.204000E+07, 0.000000E+00},
             {0.000000E+00, 0.000000E+00, 2.400000E+02}}
        ),
        Vector3d(-2746.8991, -5778.9559, -2138.6798),
        Vector3d(-0.040675, 2.6569, -7.2096),
        buildRTNPositionCovarianceCoordinates(
            {{2.304000E+07, 0.000000E+00, 0.000000E+00},
             {0.000000E+00, 3.200000E+11, 0.000000E+00},
             {0.000000E+00, 0.000000E+00, 1.500000E+03}}
        )
    );
}

// Single covariance Case 1-6 (SingleCovTestCase1-6.cdm), at TCA
CloseApproach buildSingleCovCase1_06()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2010-07-23 14:08:05.000"), Scale::UTC),
        Vector3d(-2733.6954, -5772.9162, -2169.3621),
        Vector3d(5.3499, -0.46672, -5.5048),
        buildRTNPositionCovarianceCoordinates(
            {{6.600000E+02, 0.000000E+00, 0.000000E+00},
             {0.000000E+00, 7.940000E+06, 0.000000E+00},
             {0.000000E+00, 0.000000E+00, 2.200000E+02}}
        ),
        Vector3d(-2746.8576, -5778.936, -2138.7749),
        Vector3d(-0.040646, 2.657, -7.2096),
        buildRTNPositionCovarianceCoordinates(
            {{2.000000E+05, 0.000000E+00, 0.000000E+00},
             {0.000000E+00, 8.700000E+06, 0.000000E+00},
             {0.000000E+00, 0.000000E+00, 5.100000E+02}}
        )
    );
}

// Single covariance Case 1-7 (SingleCovTestCase1-7.cdm), at TCA
CloseApproach buildSingleCovCase1_07()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2009-10-20 17:30:00.000"), Scale::UTC),
        Vector3d(-5672.7616, 285.7693, 3619.9203),
        Vector3d(-3.0962, -5.5109, -4.3868),
        buildRTNPositionCovarianceCoordinates(
            {{7.700000E+02, 0.000000E+00, 0.000000E+00},
             {0.000000E+00, 2.000000E+07, 0.000000E+00},
             {0.000000E+00, 0.000000E+00, 5.200000E+01}}
        ),
        Vector3d(-5673.6189, 285.5693, 3619.5539),
        Vector3d(-3.7933, 1.4647, -6.5642),
        buildRTNPositionCovarianceCoordinates(
            {{7.100000E+03, 0.000000E+00, 0.000000E+00},
             {0.000000E+00, 5.400000E+08, 0.000000E+00},
             {0.000000E+00, 0.000000E+00, 8.000000E+02}}
        )
    );
}

// Single covariance Case 1-8 (SingleCovTestCase1-8.cdm), at TCA
CloseApproach buildSingleCovCase1_08()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2009-10-20 17:30:00.000"), Scale::UTC),
        Vector3d(-5672.7616, 285.7693, 3619.9203),
        Vector3d(-3.0962, -5.5109, -4.3868),
        buildRTNPositionCovarianceCoordinates(
            {{7.100000E+02, 0.000000E+00, 0.000000E+00},
             {0.000000E+00, 1.400000E+07, 0.000000E+00},
             {0.000000E+00, 0.000000E+00, 4.600000E+01}}
        ),
        Vector3d(-5673.64, 285.5714, 3619.5676),
        Vector3d(-3.7933, 1.4647, -6.5642),
        buildRTNPositionCovarianceCoordinates(
            {{6.600000E+03, 0.000000E+00, 0.000000E+00},
             {0.000000E+00, 4.900000E+08, 0.000000E+00},
             {0.000000E+00, 0.000000E+00, 7.500000E+02}}
        )
    );
}

// Single covariance Case 1-9 (SingleCovTestCase1-9.cdm), at TCA
CloseApproach buildSingleCovCase1_09()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2009-10-20 17:30:00.000"), Scale::UTC),
        Vector3d(-5672.7616, 285.7693, 3619.9203),
        Vector3d(-3.0962, -5.5109, -4.3868),
        buildRTNPositionCovarianceCoordinates(
            {{7.700000E+02, 0.000000E+00, 0.000000E+00},
             {0.000000E+00, 2.000000E+07, 0.000000E+00},
             {0.000000E+00, 0.000000E+00, 5.200000E+01}}
        ),
        Vector3d(-5673.6379, 285.5727, 3619.5712),
        Vector3d(-3.7933, 1.4647, -6.5642),
        buildRTNPositionCovarianceCoordinates(
            {{7.100000E+03, 0.000000E+00, 0.000000E+00},
             {0.000000E+00, 5.400000E+08, 0.000000E+00},
             {0.000000E+00, 0.000000E+00, 8.000000E+02}}
        )
    );
}

// Single covariance Case 1-10 (SingleCovTestCase1-10.cdm), at TCA
CloseApproach buildSingleCovCase1_10()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2014-01-23 00:30:34.907"), Scale::UTC),
        Vector3d(-3482.732, -3056.736, 4959.4809),
        Vector3d(2.8181, -6.7829, -2.1931),
        buildRTNPositionCovarianceCoordinates(
            {{3.464000E+02, -5.306000E+04, 2.847000E+01},
             {-5.306000E+04, 9.351000E+06, -3.012000E+03},
             {2.847000E+01, -3.012000E+03, 5.727000E+01}}
        ),
        Vector3d(-3484.68, -3091.2534, 4934.5391),
        Vector3d(-3.6879, -4.217, -5.2353),
        buildRTNPositionCovarianceCoordinates(
            {{2.456000E+06, -4.140000E+08, 7.265000E+04},
             {-4.140000E+08, 6.990000E+10, -1.231000E+07},
             {7.265000E+04, -1.231000E+07, 3.429000E+03}}
        )
    );
}

// Single covariance Case 1-11 (SingleCovTestCase1-11.cdm), at TCA
CloseApproach buildSingleCovCase1_11()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2009-01-22 05:19:36.773"), Scale::UTC),
        Vector3d(3561.0353, 2705.418, 5033.2019),
        Vector3d(-5.9739, 4.4964, 1.8108),
        buildRTNPositionCovarianceCoordinates(
            {{7.391000E+02, -1.759000E+05, -3.484000E+01},
             {-1.759000E+05, 4.475000E+07, 1.009000E+04},
             {-3.484000E+01, 1.009000E+04, 1.607000E+01}}
        ),
        Vector3d(3554.4057, 2706.0064, 5039.8775),
        Vector3d(-3.4657, -4.7641, 5.1167),
        buildRTNPositionCovarianceCoordinates(
            {{4.880000E+05, 4.584000E+07, -1.072000E+04},
             {4.584000E+07, 4.381000E+09, -6.307000E+05},
             {-1.072000E+04, -6.307000E+05, 2.223000E+03}}
        )
    );
}

// Single covariance Case 1-12 (SingleCovTestCase1-12.cdm), at TCA
CloseApproach buildSingleCovCase1_12()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2014-01-30 23:59:59.000"), Scale::UTC),
        Vector3d(1937.6172, 6227.9323, 1894.1883),
        Vector3d(-5.2086, -0.10065, 5.6228),
        buildRTNPositionCovarianceCoordinates(
            {{1.000000E+01, -1.001000E+00, -1.000000E+00},
             {-1.001000E+00, 1.000000E+03, -1.000000E+00},
             {-1.000000E+00, -1.000000E+00, 1.000000E+02}}
        ),
        Vector3d(1937.5354, 6227.9263, 1894.1123),
        Vector3d(-5.2766, -0.10196, 5.6961),
        buildRTNPositionCovarianceCoordinates(
            {{1.000000E+02, -1.011000E+00, -1.000000E+00},
             {-1.011000E+00, 1.000000E+04, -1.000000E+00},
             {-1.000000E+00, -1.000000E+00, 1.000000E+03}}
        )
    );
}

// Single covariance Case 1-13 (SingleCovTestCase1-13.cdm), at TCA
CloseApproach buildSingleCovCase1_13()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2010-04-30 04:02:09.119"), Scale::UTC),
        Vector3d(-2490.9106, 6207.8187, -738.0217),
        Vector3d(-4.6999, -1.1726, 5.9836),
        buildRTNPositionCovarianceCoordinates(
            {{2.102000E+02, 2.916000E+02, 4.348000E+01},
             {2.916000E+02, 3.838000E+03, -4.287000E+01},
             {4.348000E+01, -4.287000E+01, 8.617000E+01}}
        ),
        Vector3d(-2496.5155, 6205.5717, -740.6917),
        Vector3d(1.4085, -0.34689, -7.5467),
        buildRTNPositionCovarianceCoordinates(
            {{4.836000E+04, -8.740000E+05, -7.111000E+02},
             {-8.740000E+05, 1.839000E+07, 6.902000E+03},
             {-7.111000E+02, 6.902000E+03, 1.089000E+03}}
        )
    );
}

// Single covariance Case 1-14 (SingleCovTestCase1-14.cdm), at TCA
CloseApproach buildSingleCovCase1_14()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2010-09-10 09:02:19.817"), Scale::UTC),
        Vector3d(-4827.9255, 4171.4314, -2139.4098),
        Vector3d(-4.689, -2.6033, 5.5257),
        buildRTNPositionCovarianceCoordinates(
            {{8.557000E+02, -1.060000E+05, -5.748000E+01},
             {-1.060000E+05, 1.701000E+07, 1.600000E+04},
             {-5.748000E+01, 1.600000E+04, 8.381000E+01}}
        ),
        Vector3d(-4823.8703, 4174.9849, -2139.8064),
        Vector3d(-2.9871, -5.6461, -4.297),
        buildRTNPositionCovarianceCoordinates(
            {{3.196000E+03, -5.028000E+05, 5.553000E+02},
             {-5.028000E+05, 9.381000E+07, -7.508000E+04},
             {5.553000E+02, -7.508000E+04, 2.202000E+02}}
        )
    );
}

// Single covariance Case 1-15 (SingleCovTestCase1-15.cdm), at TCA
CloseApproach buildSingleCovCase1_15()
{
    return buildCloseApproach(
        Instant::DateTime(DateTime::Parse("2011-02-07 10:00:08.878"), Scale::UTC),
        Vector3d(3353.9209, -5421.3879, -2164.3206),
        Vector3d(3.2981, 4.239, -5.5117),
        buildRTNPositionCovarianceCoordinates(
            {{4.669000E+02, -1.807000E+04, 8.473000E+01},
             {-1.807000E+04, 1.967000E+06, -1.634000E+03},
             {8.473000E+01, -1.634000E+03, 4.869000E+01}}
        ),
        Vector3d(3331.1905, -5433.7345, -2169.5498),
        Vector3d(3.156, -0.81259, 7.0307),
        buildRTNPositionCovarianceCoordinates(
            {{9.356000E+03, -1.066000E+06, -1.060000E+03},
             {-1.066000E+06, 1.273000E+08, 1.087000E+05},
             {-1.060000E+03, 1.087000E+05, 7.633000E+02}}
        )
    );
}

struct ComputeProbabilityOfCollisionParams
{
    String name;
    CloseApproach closeApproach;
    Real combinedHardBodyRadius;
    Real expectedProbabilityOfCollision;
    std::optional<Real> closedFormRelativeTolerance;
    Real alfano2005RelativeTolerance;
};

}  // namespace

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015 : public ::testing::Test
{
   protected:
    const Derived relativeSpeedThreshold_ = Derived(100.0, Derived::Unit::MeterPerSecond());
    const Frisbee2015 algorithm_ = {true, true, relativeSpeedThreshold_, Frame::GCRF(), std::nullopt};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015, Constructor)
{
    {
        EXPECT_NO_THROW(Frisbee2015());
    }

    {
        EXPECT_NO_THROW(Frisbee2015(true, true, relativeSpeedThreshold_, Frame::GCRF(), std::make_shared<Alfano2005>())
        );
    }

    {
        EXPECT_NO_THROW(Frisbee2015(true, true, relativeSpeedThreshold_, Frame::GCRF(), std::nullopt));
    }

    {
        EXPECT_NO_THROW(Frisbee2015(
            true, true, Derived(10.0, Derived::Unit::MeterPerSecond()), Frame::J2000(Theory::IAU_2006), std::nullopt
        ));
    }

    {
        for (const bool considerObject2Covariance : {true, false})
        {
            for (const bool considerWorstCaseWhenBothCovariancesArePresent : {true, false})
            {
                EXPECT_NO_THROW(Frisbee2015(
                    considerObject2Covariance,
                    considerWorstCaseWhenBothCovariancesArePresent,
                    Derived(10.0, Derived::Unit::MeterPerSecond()),
                    Frame::J2000(Theory::IAU_2006),
                    std::nullopt
                ));
            }
        }
    }

    {
        EXPECT_THROW(
            try {
                Frisbee2015(true, true, relativeSpeedThreshold_, nullptr);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            try {
                Frisbee2015(true, true, relativeSpeedThreshold_, Frame::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            try {
                Frisbee2015(true, true, relativeSpeedThreshold_, Frame::ITRF());
            } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_EQ("Frame = ITRF is wrong.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015, GetName)
{
    {
        EXPECT_EQ("Frisbee 2015 (Closed-form)", algorithm_.getName());
    }

    {
        const Frisbee2015 algorithm = {};

        EXPECT_EQ("Frisbee 2015 (Alfano 2005)", algorithm.getName());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015, IsMaximum)
{
    {
        EXPECT_TRUE(algorithm_.isMaximum());
    }

    {
        const Frisbee2015 algorithm = {};

        EXPECT_TRUE(algorithm.isMaximum());
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015,
    IdentifyUnsatisfiedAssumptions
)
{
    const Frisbee2015 algorithmWithoutObject2Covariance = {false};
    const Frisbee2015 algorithmWithoutWorstCase = {true, false};

    // The Frisbee example is a high relative speed encounter (~14 km/s)
    const CloseApproach closeApproach = buildFrisbeeExample();

    // All assumptions satisfied
    {
        for (const Frisbee2015& algorithm : {algorithm_, algorithmWithoutWorstCase})
        {
            EXPECT_TRUE(algorithm.identifyUnsatisfiedAssumptions(closeApproach).isEmpty());
            EXPECT_TRUE(algorithm.identifyUnsatisfiedAssumptions(removeObject1Covariance(closeApproach)).isEmpty());
            EXPECT_TRUE(algorithm.identifyUnsatisfiedAssumptions(removeObject2Covariance(closeApproach)).isEmpty());
        }

        EXPECT_TRUE(algorithmWithoutObject2Covariance.identifyUnsatisfiedAssumptions(closeApproach).isEmpty());
        EXPECT_TRUE(algorithmWithoutObject2Covariance
                        .identifyUnsatisfiedAssumptions(removeObject2Covariance(closeApproach))
                        .isEmpty());
    }

    // No covariance for Object 1, without considering Object 2 covariance
    {
        const Array<String> unsatisfiedAssumptions =
            algorithmWithoutObject2Covariance.identifyUnsatisfiedAssumptions(removeObject1Covariance(closeApproach));

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 1"));
    }

    // No covariance for either object
    {
        for (const Frisbee2015& algorithm : {algorithm_, algorithmWithoutWorstCase})
        {
            const Array<String> unsatisfiedAssumptions =
                algorithm.identifyUnsatisfiedAssumptions(removeObject1Covariance(removeObject2Covariance(closeApproach))
                );

            EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
            EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No object has position uncertainty"));
        }

        const Array<String> unsatisfiedAssumptions = algorithmWithoutObject2Covariance.identifyUnsatisfiedAssumptions(
            removeObject1Covariance(removeObject2Covariance(closeApproach))
        );

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 1"));
    }

    // Relative speed below the threshold
    {
        const Frisbee2015 algorithm = {true, true, Derived(20000.0, Derived::Unit::MeterPerSecond())};

        const Array<String> unsatisfiedAssumptions = algorithm.identifyUnsatisfiedAssumptions(closeApproach);

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "Relative speed"));
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "is below the threshold"));
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015,
    ComputeProbabilityOfCollision_UndefinedHardBody
)
{
    const CloseApproach closeApproach = buildFrisbeeExample();
    const Shared<const Spherical> hardBodySPtr = std::make_shared<Spherical>(2.0);

    {
        EXPECT_THROW(
            try {
                algorithm_.computeProbabilityOfCollision(closeApproach, nullptr, hardBodySPtr);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Hard body} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            try {
                algorithm_.computeProbabilityOfCollision(closeApproach, hardBodySPtr, nullptr);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Hard body} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015,
    ComputeProbabilityOfCollision_Behaviour
)
{
    // Single covariance Case 1-1 with Object 1 and Object 2 swapped, so that the maximum probability of collision using
    // the Object 2 covariance (Pc2) is the worst case
    const CloseApproach closeApproach = buildSingleCovCase1_01().swap();
    const Shared<const Spherical> hardBodySPtr = std::make_shared<Spherical>(20.0 / 2.0);

    // Maximum probabilities of collision using a single covariance: Pc1 with the Object 1 covariance only, Pc2 with the
    // Object 2 covariance only (computed by swapping the objects back, so that it is the Object 1 covariance)
    const Real probabilityOfCollision1 =
        algorithm_.computeProbabilityOfCollision(removeObject2Covariance(closeApproach), hardBodySPtr, hardBodySPtr);
    const Real probabilityOfCollision2 = algorithm_.computeProbabilityOfCollision(
        removeObject2Covariance(closeApproach.swap()), hardBodySPtr, hardBodySPtr
    );

    ASSERT_GT(probabilityOfCollision2, probabilityOfCollision1);

    // Both covariances, but only the Object 1 covariance is used
    {
        const Frisbee2015 algorithm = {true, false, relativeSpeedThreshold_, Frame::GCRF(), std::nullopt};

        EXPECT_EQ(
            algorithm.computeProbabilityOfCollision(closeApproach, hardBodySPtr, hardBodySPtr), probabilityOfCollision1
        );
    }

    // Both covariances, and the worst case is used
    {
        const Frisbee2015 algorithm = {true, true, relativeSpeedThreshold_, Frame::GCRF(), std::nullopt};

        EXPECT_EQ(
            algorithm.computeProbabilityOfCollision(closeApproach, hardBodySPtr, hardBodySPtr), probabilityOfCollision2
        );
    }

    // Only the Object 2 covariance, which is allowed to be used
    {
        const Frisbee2015 algorithm = {true, false, relativeSpeedThreshold_, Frame::GCRF(), std::nullopt};

        EXPECT_EQ(
            algorithm.computeProbabilityOfCollision(removeObject1Covariance(closeApproach), hardBodySPtr, hardBodySPtr),
            probabilityOfCollision2
        );
    }

    // Only the Object 2 covariance, which is allowed to be used, and the worst case is used
    {
        const Frisbee2015 algorithm = {true, true, relativeSpeedThreshold_, Frame::GCRF(), std::nullopt};

        EXPECT_EQ(
            algorithm.computeProbabilityOfCollision(removeObject1Covariance(closeApproach), hardBodySPtr, hardBodySPtr),
            probabilityOfCollision2
        );
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015,
    ComputeProbabilityOfCollision_FrisbeePaperExample
)
{
    // Example of the basic method from Frisbee (2015), Equations 11 to 15 and Table 1. The NASA CARA Frisbee example
    // CDM reproduces the geometry of the paper, with only the Object 1 covariance known: the collision plane miss
    // position and Object 1 covariance are, up to an in-plane rotation (which leaves the probability of collision
    // unchanged) and the rounding of the CDM [m, m^2]:
    //
    //     r_rel = [1000, 200], C_AC = diag(722500, 2500)
    const CloseApproach closeApproach = removeObject2Covariance(buildFrisbeeExample());

    // Equation 15b: Pc = R_HB^2 * e^(-1/2) / (20000 * sqrt(314))
    const Real probabilityOfCollisionPerSquareMeter = std::exp(-0.5) / (20000.0 * std::sqrt(314.0));

    // Table 1: combined hard-body radius [m] and probability of collision (reported to 1e-6)
    const Array<std::pair<Real, Real>> table = {
        {5.0, 0.000043},
        {10.0, 0.000171},
        {20.0, 0.000685},
        {25.0, 0.001070},
    };

    for (const auto& [combinedHardBodyRadius, tableProbabilityOfCollision] : table)
    {
        const Shared<const Spherical> hardBodySPtr = std::make_shared<Spherical>(combinedHardBodyRadius / 2.0);

        const Real probabilityOfCollision =
            algorithm_.computeProbabilityOfCollision(closeApproach, hardBodySPtr, hardBodySPtr);

        const Real expectedProbabilityOfCollision =
            combinedHardBodyRadius * combinedHardBodyRadius * probabilityOfCollisionPerSquareMeter;

        // The relative tolerance accounts for the rounding of the CDM states (~2e-6 on the probability of collision)
        EXPECT_NEAR(probabilityOfCollision, expectedProbabilityOfCollision, expectedProbabilityOfCollision * 1.0e-5)
            << "Combined hard-body radius: " << combinedHardBodyRadius;
        EXPECT_NEAR(probabilityOfCollision, tableProbabilityOfCollision, 5.0e-7)
            << "Combined hard-body radius: " << combinedHardBodyRadius;
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015, ComputeProbabilityAnalysis
)
{
    // The probability of collision is already an upper bound: the analysis is not applicable
    const CloseApproach closeApproach = buildFrisbeeExample();
    const Shared<const Spherical> hardBodySPtr = std::make_shared<Spherical>(2.0);

    {
        EXPECT_THROW(
            try {
                ProbabilityOfCollisionAlgorithm::ComputeProbabilityAnalysis(
                    std::make_shared<Frisbee2015>(algorithm_), closeApproach, hardBodySPtr, hardBodySPtr
                );
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ(
                    "The probability of collision is already an upper bound: the analysis is not applicable.",
                    e.getMessage()
                );
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        EXPECT_THROW(
            ProbabilityOfCollisionAlgorithm::ComputeProbabilityAnalysis(
                std::make_shared<Frisbee2015>(), closeApproach, hardBodySPtr, hardBodySPtr
            ),
            ostk::core::error::RuntimeError
        );
    }
}

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015_Parameterized
    : public ::testing::TestWithParam<ComputeProbabilityOfCollisionParams>
{
};

INSTANTIATE_TEST_SUITE_P(
    Cases,
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015_Parameterized,
    ::testing::Values(
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_01_HBR70_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_01()),
            70.0,
            3.1394E-6,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_01_HBR70_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_01()),
            70.0,
            3.1860E-4,
            // HBR / sigma_min = 0.41: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_01_HBR20_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_01()),
            20.0,
            2.5628E-7,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_01_HBR20_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_01()),
            20.0,
            2.6520E-5,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_03_HBR70_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_03()),
            70.0,
            7.1823E-04,
            // HBR / sigma_min = 0.72: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_03_HBR70_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_03()),
            70.0,
            1.5071E-03,
            // HBR / sigma_min = 8.94: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            // HBR / sigma_min = 8.94: the NASA CARA expected value uses an equal-area square hard body
            // (squareEquArea), which differs increasingly from the circle as HBR / sigma_min increases
            1.5e-1,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_03_HBR20_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_03()),
            20.0,
            6.2288E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_03_HBR20_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_03()),
            20.0,
            4.2044E-04,
            // HBR / sigma_min = 2.55: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            // HBR / sigma_min = 2.55: the NASA CARA expected value uses an equal-area square hard body
            // (squareEquArea), which differs increasingly from the circle as HBR / sigma_min increases
            1.5e-1,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_04_HBR70_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_04()),
            70.0,
            3.3299E-04,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_04_HBR70_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_04()),
            70.0,
            4.2791E-04,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_04_HBR20_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_04()),
            20.0,
            2.7330E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_04_HBR20_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_04()),
            20.0,
            3.5247E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_05_HBR70_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_05()),
            70.0,
            9.8187E-07,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_05_HBR70_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_05()),
            70.0,
            3.3912E-04,
            // HBR / sigma_min = 0.57: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_05_HBR20_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_05()),
            20.0,
            8.0154E-08,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_05_HBR20_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_05()),
            20.0,
            2.8746E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_06_HBR70_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_06()),
            70.0,
            9.7227E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_06_HBR70_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_06()),
            70.0,
            5.8415E-04,
            // HBR / sigma_min = 1.08: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_06_HBR20_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_06()),
            20.0,
            7.9600E-06,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_06_HBR20_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_06()),
            20.0,
            5.4289E-05,
            // HBR / sigma_min = 0.31: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_07_HBR70_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_07()),
            70.0,
            1.4561E-04,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_07_HBR70_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_07()),
            70.0,
            7.0648E-04,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_07_HBR20_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_07()),
            20.0,
            1.1887E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_07_HBR20_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_07()),
            20.0,
            5.7679E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_08_HBR70_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_08()),
            70.0,
            1.4553E-04,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_08_HBR70_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_08()),
            70.0,
            8.0636E-04,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_08_HBR20_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_08()),
            20.0,
            1.1880E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_08_HBR20_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_08()),
            20.0,
            6.5836E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_09_HBR70_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_09()),
            70.0,
            1.3853E-04,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_09_HBR70_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_09()),
            70.0,
            6.7446E-04,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_09_HBR20_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_09()),
            20.0,
            1.1309E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_09_HBR20_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_09()),
            20.0,
            5.5064E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_10_HBR70_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_10()),
            70.0,
            5.1819E-06,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_10_HBR70_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_10()),
            70.0,
            3.9759E-04,
            // HBR / sigma_min = 0.88: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_10_HBR20_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_10()),
            20.0,
            4.2302E-07,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_10_HBR20_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_10()),
            20.0,
            3.5481E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_11_HBR70_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_11()),
            70.0,
            1.7958E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_11_HBR70_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_11()),
            70.0,
            1.7066E-04,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_11_HBR20_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_11()),
            20.0,
            1.4659E-06,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_11_HBR20_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_11()),
            20.0,
            1.3941E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_12_HBR70_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_12()),
            70.0,
            2.6764E-01,
            // HBR / sigma_min = 4.25: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            // HBR / sigma_min = 4.25: the NASA CARA expected value uses an equal-area square hard body
            // (squareEquArea), which differs increasingly from the circle as HBR / sigma_min increases
            1.5e-1,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_12_HBR70_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_12()),
            70.0,
            2.6808E-01,
            // HBR / sigma_min = 13.06: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            // HBR / sigma_min = 13.06: the NASA CARA expected value uses an equal-area square hard body
            // (squareEquArea), which differs increasingly from the circle as HBR / sigma_min increases
            1.5e-1,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_12_HBR20_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_12()),
            20.0,
            5.4455E-02,
            // HBR / sigma_min = 1.22: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_12_HBR20_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_12()),
            20.0,
            7.6646E-02,
            // HBR / sigma_min = 3.73: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            // HBR / sigma_min = 3.73: the NASA CARA expected value uses an equal-area square hard body
            // (squareEquArea), which differs increasingly from the circle as HBR / sigma_min increases
            1.5e-1,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_13_HBR70_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_13()),
            70.0,
            8.4809E-04,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_13_HBR70_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_13()),
            70.0,
            4.5470E-03,
            // HBR / sigma_min = 4.86: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            // HBR / sigma_min = 4.86: the NASA CARA expected value uses an equal-area square hard body
            // (squareEquArea), which differs increasingly from the circle as HBR / sigma_min increases
            1.5e-1,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_13_HBR20_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_13()),
            20.0,
            6.9822E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_13_HBR20_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_13()),
            20.0,
            1.0159E-03,
            // HBR / sigma_min = 1.39: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            // HBR / sigma_min = 1.39: the NASA CARA expected value uses an equal-area square hard body
            // (squareEquArea), which differs increasingly from the circle as HBR / sigma_min increases
            1.5e-1,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_14_HBR70_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_14()),
            70.0,
            3.3442E-04,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_14_HBR70_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_14()),
            70.0,
            7.8710E-04,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_14_HBR20_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_14()),
            20.0,
            2.7323E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_14_HBR20_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_14()),
            20.0,
            6.4567E-05,
            1.0e-2,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_15_HBR70_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_15()),
            70.0,
            1.1225E-03,
            // HBR / sigma_min = 2.81: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            // HBR / sigma_min = 2.81: the NASA CARA expected value uses an equal-area square hard body
            // (squareEquArea), which differs increasingly from the circle as HBR / sigma_min increases
            1.5e-1,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_15_HBR70_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_15()),
            70.0,
            1.1361E-03,
            // HBR / sigma_min = 3.63: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            // HBR / sigma_min = 3.63: the NASA CARA expected value uses an equal-area square hard body
            // (squareEquArea), which differs increasingly from the circle as HBR / sigma_min increases
            1.5e-1,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_15_HBR20_NoObject1Covariance",
            removeObject1Covariance(buildSingleCovCase1_15()),
            20.0,
            1.6925E-04,
            // HBR / sigma_min = 0.80: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            1.0e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "SingleCovCase1_15_HBR20_NoObject2Covariance",
            removeObject2Covariance(buildSingleCovCase1_15()),
            20.0,
            2.0861E-04,
            // HBR / sigma_min = 1.04: the closed form degrades as the smaller collision plane standard deviation
            // nears or becomes smaller than the combined hard-body radius (Frisbee, 2015)
            std::nullopt,
            1.0e-2,
        }
    ),
    [](const ::testing::TestParamInfo<ComputeProbabilityOfCollisionParams>& aParamInfo) -> std::string
    {
        return aParamInfo.param.name;
    }
);

TEST_P(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015_Parameterized,
    ComputeProbabilityOfCollision
)
{
    const ComputeProbabilityOfCollisionParams& params = GetParam();

    // The combined hard-body radius is split evenly between both objects
    const Shared<const Spherical> hardBodySPtr = std::make_shared<Spherical>(params.combinedHardBodyRadius / 2.0);

    // Closed form (Frisbee 2015, Equation 5)
    if (params.closedFormRelativeTolerance.has_value())
    {
        const Frisbee2015 algorithm = {
            true, true, Derived(100.0, Derived::Unit::MeterPerSecond()), Frame::GCRF(), std::nullopt
        };

        EXPECT_NEAR(
            algorithm.computeProbabilityOfCollision(params.closeApproach, hardBodySPtr, hardBodySPtr),
            params.expectedProbabilityOfCollision,
            params.expectedProbabilityOfCollision * params.closedFormRelativeTolerance.value()
        ) << "Closed form";
    }

    // Alfano 2005 numerical integration of the critical degenerate covariance
    {
        const Frisbee2015 algorithm = {
            true, true, Derived(100.0, Derived::Unit::MeterPerSecond()), Frame::GCRF(), std::make_shared<Alfano2005>()
        };

        EXPECT_NEAR(
            algorithm.computeProbabilityOfCollision(params.closeApproach, hardBodySPtr, hardBodySPtr),
            params.expectedProbabilityOfCollision,
            params.expectedProbabilityOfCollision * params.alfano2005RelativeTolerance
        ) << "Alfano 2005";
    }
}
