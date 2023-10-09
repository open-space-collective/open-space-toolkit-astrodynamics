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
using ostk::astro::trajectory::orbit::models::kepler::COE;

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

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BMLOE, ToCOE)
{
    {
        const Length semiMajorAxis = Length::Meters(6972365.96849981);
        const Real eccentricity = 0.001677656315248244;
        const Angle inclination = Angle::Radians(1.705584427014695);
        const Angle raan = Angle::Radians(1.149997444146928);
        const Angle aop = Angle::Radians(1.570796346108476);
        const Angle meanAnomaly = Angle::Radians(6.283185292358735);

        const BMLOE bmloe = {
            semiMajorAxis,
            eccentricity,
            inclination,
            raan,
            aop,
            meanAnomaly,
        };

        const Vector6d coeVectorExpected = {
            6963000.047336794,
            0.001060006293925325,
            1.705675370985273,
            1.149997444147743,
            1.570796346211503,
            6.283185292246888,
        };

        EXPECT_TRUE((coeVectorExpected - bmloe.toCOE().asVector(COE::AnomalyType::Mean)).all() < 1e-15);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BMLOE, Cartesian)
{
    {
        const Real meanSMA = 6972365.96849981;
        const Real meanEcc = 0.001677656315248244;
        const Real meanInc = 1.705584427014695;
        const Real meanRaan = 1.149997444146928;
        const Real meanAop = 1.570796346108476;
        const Real meanMeanAnomaly = 6.283185292358735;

        const Position position = Position::Meters({853730.214, -382070.813, 6892445.528}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({-3093.942450, -6913.357574, 0.000000}, Frame::GCRF());
        const BMLOE bmloe = BMLOE::Cartesian({position, velocity}, Earth::EGM2008.gravitationalParameter_);

        EXPECT_LT(std::abs(bmloe.asVector()[0] - meanSMA), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[1] - meanEcc), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[2] - meanInc), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[3] - meanRaan), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[4] - meanAop), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[5] - meanMeanAnomaly), 1e-6);
    }

    {
        const Position position = Position::Meters({4340789.050, 0.000000, 0.000000}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({0.000000, 9784.755, 5312.688}, Frame::GCRF());

        const BMLOE bmloe = BMLOE::Cartesian({position, velocity}, Earth::EGM2008.gravitationalParameter_);

        const Vector6d expectedMeanElements = {
            6.659637260077382e+006,
            3.473326030715827e-001,
            2.846453224361113e+001,
            3.599450721824177e+002,
            3.599700015119411e+002,
            6.963626331704763e-002,
        };

        EXPECT_LT(std::abs(bmloe.asVector()[0] - expectedMeanElements[0]), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[1] - expectedMeanElements[1]), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[2] - expectedMeanElements[2]), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[3] - expectedMeanElements[3]), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[4] - expectedMeanElements[4]), 1e-6);
        EXPECT_LT(std::abs(bmloe.asVector()[5] - expectedMeanElements[5]), 1e-6);
    }
}
