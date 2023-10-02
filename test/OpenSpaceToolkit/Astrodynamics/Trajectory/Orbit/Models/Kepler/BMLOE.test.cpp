/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/BMLOE.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::String;
using ostk::core::ctnr::Array;
using ostk::core::ctnr::Tuple;

using Vector6d = Eigen::Matrix<double, 1, 6>;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::coord::Frame;
using ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::Duration;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;

using ostk::astro::trajectory::orbit::models::kepler::BMLOE;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BMLOE, Constructor)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle meanAnomaly = Angle::Degrees(40.0);

        EXPECT_NO_THROW(BMLOE(semiMajorAxis, eccentricity, inclination, raan, aop, meanAnomaly););
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BMLOE, Cartesian)
{
    {
        const Real meanSMA = 6972365.90155841;
        const Real meanEcc = 0.00167736;
        const Real meanInc = Angle::Degrees(97.72278930).inRadians();
        const Real meanRaan = Angle::Degrees(65.89000000).inRadians();
        const Real meanAop = Angle::Degrees(90.0).inRadians();
        const Real meanMeanAnomaly = Angle::Degrees(0.0).inRadians();

        const Position position = Position::Meters({853730.214, -382070.813, 6892445.528}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({-3093.942450, -6913.357574, 0.000000}, Frame::GCRF());
        BMLOE bmloe = BMLOE::Cartesian({position, velocity}, Earth::EGM96.gravitationalParameter_);

        EXPECT_LT(std::abs(bmloe.asVector()[0] - meanSMA), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[1] - meanEcc), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[2] - meanInc), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[3] - meanRaan), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[4] - meanAop), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[5] - meanMeanAnomaly), 1e-6);
    }

    {
        const Position position = Position::Meters({4340.789050, -0.000000, -0.000000}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({-0.000000, 9.784755, 5.312688}, Frame::GCRF());

        const Vector6d expectedMeanElements = {
            6.659637260077382e+003,
            3.473326030715827e-001,
            2.846453224361113e+001,
            3.599450721824177e+002,
            3.599700015119411e+002,
            6.963626331704763e-002,
        };
    }
}
