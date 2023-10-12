/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/BrouwerLyddaneMean/BrouwerLyddaneMeanShort.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::String;
using ostk::core::ctnr::Array;
using ostk::core::ctnr::Tuple;

using ostk::math::obj::Vector3d;
using ostk::math::obj::Vector6d;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::coord::Frame;
using ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::Duration;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;

using ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::trajectory::orbit::models::blm::BrouwerLyddaneMeanLong;
using ostk::astro::trajectory::orbit::models::blm::BrouwerLyddaneMeanShort;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, Constructor)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        EXPECT_NO_THROW(COE(semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly););
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, EqualToOperator)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        EXPECT_TRUE(coe == coe);
    }

    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE firstCoe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const COE secondCoe = {semiMajorAxis, eccentricity, inclination, Angle::Degrees(21.0), aop, trueAnomaly};

        EXPECT_FALSE(firstCoe == secondCoe);
    }

    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        EXPECT_FALSE(COE::Undefined() == COE::Undefined());
        EXPECT_FALSE(coe == COE::Undefined());
        EXPECT_FALSE(COE::Undefined() == coe);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, NotEqualToOperator)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        EXPECT_FALSE(coe != coe);
    }

    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE firstCoe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const COE secondCoe = {semiMajorAxis, eccentricity, inclination, Angle::Degrees(21.0), aop, trueAnomaly};

        EXPECT_TRUE(firstCoe != secondCoe);
    }

    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        EXPECT_TRUE(COE::Undefined() != COE::Undefined());
        EXPECT_TRUE(coe != COE::Undefined());
        EXPECT_TRUE(COE::Undefined() != coe);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, StreamOperator)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << coe << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, IsDefined)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        EXPECT_TRUE(coe.isDefined());
    }

    {
        EXPECT_FALSE(COE::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetSemiMajorAxis)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        EXPECT_EQ(semiMajorAxis, coe.getSemiMajorAxis());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getSemiMajorAxis());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetEccentricity)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        EXPECT_EQ(eccentricity, coe.getEccentricity());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getEccentricity());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetInclination)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        EXPECT_EQ(inclination, coe.getInclination());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getInclination());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetRaan)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        EXPECT_EQ(raan, coe.getRaan());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getRaan());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetAop)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        EXPECT_EQ(aop, coe.getAop());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getAop());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetTrueAnomaly)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        EXPECT_EQ(trueAnomaly, coe.getTrueAnomaly());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getTrueAnomaly());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetPeriapsisRadius)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        EXPECT_DOUBLE_EQ(semiMajorAxis.inMeters() * (1.0 - eccentricity), coe.getPeriapsisRadius().inMeters());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getPeriapsisRadius());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetApoapsisRadius)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle trueAnomaly = Angle::Degrees(40.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        EXPECT_DOUBLE_EQ(semiMajorAxis.inMeters() * (1.0 + eccentricity), coe.getApoapsisRadius().inMeters());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getApoapsisRadius());
    }
}

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetMeanAnomaly)
// {

//     using ostk::core::types::Real ;

//     using ostk::math::obj::Vector3d ;

//     using ostk::physics::units::Length ;
//     using ostk::physics::units::Angle ;
//     using ostk::physics::units::Derived ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::coord::Position ;
//     using ostk::physics::coord::Velocity ;
//     using ostk::physics::env::obj::celest::Earth ;

//     using ostk::astro::trajectory::orbit::models::kepler::COE ;

//     {

//         FAIL() ;

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetEccentricAnomaly)
// {

//     using ostk::core::types::Real ;

//     using ostk::math::obj::Vector3d ;

//     using ostk::physics::units::Length ;
//     using ostk::physics::units::Angle ;
//     using ostk::physics::units::Derived ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::coord::Position ;
//     using ostk::physics::coord::Velocity ;
//     using ostk::physics::env::obj::celest::Earth ;

//     using ostk::astro::trajectory::orbit::models::kepler::COE ;

//     {

//         FAIL() ;

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetMeanMotion)
// {

//     using ostk::core::types::Real ;

//     using ostk::math::obj::Vector3d ;

//     using ostk::physics::units::Length ;
//     using ostk::physics::units::Angle ;
//     using ostk::physics::units::Derived ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::coord::Position ;
//     using ostk::physics::coord::Velocity ;
//     using ostk::physics::env::obj::celest::Earth ;

//     using ostk::astro::trajectory::orbit::models::kepler::COE ;

//     {

//         FAIL() ;

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetOrbitalPeriod)
// {

//     using ostk::core::types::Real ;

//     using ostk::math::obj::Vector3d ;

//     using ostk::physics::units::Length ;
//     using ostk::physics::units::Angle ;
//     using ostk::physics::units::Derived ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::coord::Position ;
//     using ostk::physics::coord::Velocity ;
//     using ostk::physics::env::obj::celest::Earth ;

//     using ostk::astro::trajectory::orbit::models::kepler::COE ;

//     {

//         FAIL() ;

//     }

// }

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetCartesianState)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(0.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const COE::CartesianState cartesianState =
            coe.getCartesianState(Earth::EGM2008.gravitationalParameter_, Frame::GCRF());

        const Position& position = cartesianState.first;
        const Velocity& velocity = cartesianState.second;

        EXPECT_TRUE(position.getCoordinates().isNear(Vector3d(7000000.0, 0.0, 0.0), Real::Epsilon()))
            << position.getCoordinates();
        EXPECT_EQ(Position::Unit::Meter, position.getUnit());
        EXPECT_EQ(Frame::GCRF(), position.accessFrame());

        EXPECT_TRUE(velocity.getCoordinates().isNear(Vector3d(0.0, 7546.0532872678359, 0.0), Real::Epsilon()))
            << velocity.getCoordinates();
        EXPECT_EQ(Velocity::Unit::MeterPerSecond, velocity.getUnit());
        EXPECT_EQ(Frame::GCRF(), velocity.accessFrame());
    }

    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.05;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(10.0);
        const Angle aop = Angle::Degrees(20.0);
        const Angle trueAnomaly = Angle::Degrees(30.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const COE::CartesianState cartesianState =
            coe.getCartesianState(Earth::EGM2008.gravitationalParameter_, Frame::GCRF());

        const Position& position = cartesianState.first;
        const Velocity& velocity = cartesianState.second;

        const Vector3d referencePosition = {3607105.091490086168, 4317224.055878742598, 3625268.495929836296};
        const Vector3d referenceVelocity = {-6467.082890311416, 2601.669219824156, 3685.141177179613};

        const Real positionTolerance = 1e-8;
        const Real velocityTolerance = 1e-12;

        EXPECT_TRUE(position.getCoordinates().isNear(referencePosition, positionTolerance)) << String::Format(
            "{} - {} = {} [m]",
            position.getCoordinates().toString(),
            referencePosition.toString(),
            (position.getCoordinates() - referencePosition).norm()
        );
        EXPECT_EQ(Position::Unit::Meter, position.getUnit());
        EXPECT_EQ(Frame::GCRF(), position.accessFrame());

        EXPECT_TRUE(velocity.getCoordinates().isNear(referenceVelocity, velocityTolerance)) << String::Format(
            "{} - {} = {} [m/s]",
            velocity.getCoordinates().toString(),
            referenceVelocity.toString(),
            (velocity.getCoordinates() - referenceVelocity).norm()
        );
        EXPECT_EQ(Velocity::Unit::MeterPerSecond, velocity.getUnit());
        EXPECT_EQ(Frame::GCRF(), velocity.accessFrame());
    }

    {
        // [TBI] Add more tests
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getCartesianState(Earth::EGM2008.gravitationalParameter_, Frame::GCRF()));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetVector)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(180.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(45.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Vector6d coeVector = {
            semiMajorAxis.inMeters(),
            eccentricity,
            inclination.inRadians(),
            raan.inRadians(),
            aop.inRadians(),
            trueAnomaly.inRadians(),
        };

        EXPECT_TRUE(((coeVector - coe.getSIVector(COE::AnomalyType::True)).sum() < 1e-15));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, Undefined)
{
    {
        EXPECT_NO_THROW(COE::Undefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, Cartesian)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(0.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE referenceCoe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const COE::CartesianState cartesianState =
            referenceCoe.getCartesianState(Earth::EGM2008.gravitationalParameter_, Frame::GCRF());

        const COE coe = COE::Cartesian(cartesianState, Earth::EGM2008.gravitationalParameter_);

        EXPECT_TRUE(coe.getSemiMajorAxis().inMeters().isNear(referenceCoe.getSemiMajorAxis().inMeters(), 1e-6));
        EXPECT_TRUE(coe.getEccentricity().isNear(referenceCoe.getEccentricity(), Real::Epsilon()));
        EXPECT_TRUE(coe.getInclination().inDegrees().isNear(referenceCoe.getInclination().inDegrees(), Real::Epsilon())
        );
        EXPECT_TRUE(coe.getRaan().inDegrees().isNear(referenceCoe.getRaan().inDegrees(), Real::Epsilon()));
        EXPECT_TRUE(coe.getAop().inDegrees().isNear(referenceCoe.getAop().inDegrees(), Real::Epsilon()));
        EXPECT_TRUE(coe.getTrueAnomaly().inDegrees().isNear(referenceCoe.getTrueAnomaly().inDegrees(), Real::Epsilon())
        );
    }

    // test edge case where true anomly is nan without std::clamp
    {
        // Non-circular, inclined
        {
            const COE::CartesianState cartesianState = {
                Position::Meters({6994047.553724454, 477490.53731528745, 416.6892069390969}, Frame::GCRF()),
                Velocity::MetersPerSecond({-516.3880965905739, 7563.794200060748, 6.600657312278296}, Frame::GCRF()),
            };

            const COE coe = COE::Cartesian(cartesianState, Earth::EGM2008.gravitationalParameter_);

            EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), 7087442.844884155, 1e-6);
            EXPECT_NEAR(coe.getEccentricity(), 0.010880490113017855, Real::Epsilon());
            EXPECT_NEAR(coe.getInclination().inRadians(), 0.0008726646260034904, Real::Epsilon());
            EXPECT_NEAR(coe.getRaan().inRadians(), 6.283185307179586, Real::Epsilon());
            EXPECT_NEAR(coe.getAop().inRadians(), 0.06816523492423941, Real::Epsilon());
            EXPECT_NEAR(coe.getTrueAnomaly().inRadians(), 0.0, Real::Epsilon());
        }

        // Circular, inclined
        {
            const Length semiMajorAxis = Length::Kilometers(7000.0);
            const Real eccentricity = 0.0;
            const Angle inclination = Angle::Degrees(97.0);
            const Angle raan = Angle::Degrees(0.0);
            const Angle aop = Angle::Degrees(0.0);
            const Angle trueAnomaly = Angle::Degrees(0.0);

            const COE referenceCoe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

            const COE::CartesianState cartesianState =
                referenceCoe.getCartesianState(Earth::EGM2008.gravitationalParameter_, Frame::GCRF());

            const COE coe = COE::Cartesian(cartesianState, Earth::EGM2008.gravitationalParameter_);

            EXPECT_TRUE(coe.getSemiMajorAxis().inMeters().isNear(referenceCoe.getSemiMajorAxis().inMeters(), 1e-6));
            EXPECT_TRUE(coe.getEccentricity().isNear(referenceCoe.getEccentricity(), Real::Epsilon()));
            EXPECT_TRUE(
                coe.getInclination().inDegrees().isNear(referenceCoe.getInclination().inDegrees(), Real::Epsilon())
            );
            EXPECT_TRUE(coe.getRaan().inDegrees().isNear(referenceCoe.getRaan().inDegrees(), Real::Epsilon()));
            EXPECT_TRUE(coe.getAop().inDegrees().isNear(referenceCoe.getAop().inDegrees(), Real::Epsilon()));
            EXPECT_TRUE(
                coe.getTrueAnomaly().inDegrees().isNear(referenceCoe.getTrueAnomaly().inDegrees(), Real::Epsilon())
            );
        }
    }

    {
        EXPECT_ANY_THROW(COE::Cartesian(
            COE::CartesianState({Position::Undefined(), Velocity::Undefined()}), Earth::EGM2008.gravitationalParameter_
        ));
    }

    {
        const Position position =
            Position::Meters({6596065.624114551, 2282234.953292401, -18030.93992064121}, Frame::GCRF());
        const Velocity velocity =
            Velocity::MetersPerSecond({345.4716519563907, -967.0404288726759, 7488.686029827369}, Frame::GCRF());
        const COE coe = COE::Cartesian({position, velocity}, Earth::EGM2008.gravitationalParameter_);

        EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), 6983041.667517853, 1e-10);
        EXPECT_NEAR(coe.getEccentricity(), 0.001288522395593299, 1e-10);
        EXPECT_NEAR(coe.getInclination().inDegrees(), 97.80765762597238, 1e-10);
        EXPECT_NEAR(coe.getRaan().inDegrees(), 19.06529544678578, 1e-10);
        EXPECT_NEAR(coe.getAop().inDegrees(), 68.50632506660459, 1e-10);
        EXPECT_NEAR(coe.getMeanAnomaly().inDegrees(), 291.4817543658902, 1e-10);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, FromSIVector)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(180.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(45.0);

        const Vector6d coeVector = {
            semiMajorAxis.inMeters(),
            eccentricity,
            inclination.inRadians(),
            raan.inRadians(),
            aop.inRadians(),
            trueAnomaly.inRadians(),
        };

        const COE coeFromVector = COE::FromSIVector(coeVector, COE::AnomalyType::True);
        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};
        EXPECT_TRUE(coeFromVector == coe);
    }
}

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, EccentricAnomalyFromTrueAnomaly)
// {

//     using ostk::core::types::Real ;

//     using ostk::math::obj::Vector3d ;

//     using ostk::physics::units::Length ;
//     using ostk::physics::units::Angle ;
//     using ostk::physics::units::Derived ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::coord::Position ;
//     using ostk::physics::coord::Velocity ;
//     using ostk::physics::env::obj::celest::Earth ;

//     using ostk::astro::trajectory::orbit::models::kepler::COE ;

//     {

//         FAIL() ;

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, TrueAnomalyFromEccentricAnomaly)
// {

//     using ostk::core::types::Real ;

//     using ostk::math::obj::Vector3d ;

//     using ostk::physics::units::Length ;
//     using ostk::physics::units::Angle ;
//     using ostk::physics::units::Derived ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::coord::Position ;
//     using ostk::physics::coord::Velocity ;
//     using ostk::physics::env::obj::celest::Earth ;

//     using ostk::astro::trajectory::orbit::models::kepler::COE ;

//     {

//         FAIL() ;

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, MeanAnomalyFromEccentricAnomaly)
// {

//     using ostk::core::types::Real ;

//     using ostk::math::obj::Vector3d ;

//     using ostk::physics::units::Length ;
//     using ostk::physics::units::Angle ;
//     using ostk::physics::units::Derived ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::coord::Position ;
//     using ostk::physics::coord::Velocity ;
//     using ostk::physics::env::obj::celest::Earth ;

//     using ostk::astro::trajectory::orbit::models::kepler::COE ;

//     {

//         FAIL() ;

//     }

// }

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, EccentricAnomalyFromMeanAnomaly)
{
    {
        const Angle meanAnomaly = Angle::Degrees(0.0);
        const Real eccentricity = 0.0;
        const Real tolerance = 1e-5;

        const Angle eccentricAnomaly = COE::EccentricAnomalyFromMeanAnomaly(meanAnomaly, eccentricity, tolerance);

        EXPECT_NEAR(eccentricAnomaly.inDegrees(), 0.0, 0.0);
    }

    {
        const Angle meanAnomaly = Angle::Radians(0.99262603391585447);
        const Real eccentricity = 0.05;
        const Real tolerance = 1e-8;

        const Angle eccentricAnomaly = COE::EccentricAnomalyFromMeanAnomaly(meanAnomaly, eccentricity, tolerance);

        EXPECT_NEAR(eccentricAnomaly.inRadians(), 1.0356353614863638, 1e-8);
    }

    {
        const Angle meanAnomaly = Angle::Degrees(0.0);
        const Real eccentricity = 0.0;
        const Real tolerance = 1e-5;

        EXPECT_ANY_THROW(COE::EccentricAnomalyFromMeanAnomaly(Angle::Undefined(), eccentricity, tolerance));
        EXPECT_ANY_THROW(COE::EccentricAnomalyFromMeanAnomaly(meanAnomaly, Real::Undefined(), tolerance));
        EXPECT_ANY_THROW(COE::EccentricAnomalyFromMeanAnomaly(meanAnomaly, eccentricity, Real::Undefined()));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, TrueAnomalyFromMeanAnomaly)
{
    {
        const Angle meanAnomaly = Angle::Radians(5.08731632317414);
        const Real eccentricity = 0.001288522395593299;
        const Real tolerance = 1e-15;

        const Angle trueAnomaly = COE::TrueAnomalyFromMeanAnomaly(meanAnomaly, eccentricity, tolerance);

        EXPECT_NEAR(trueAnomaly.inRadians(), 5.084916881853494, tolerance);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, StringFromElement)
{
    const Array<Tuple<COE::Element, String>> testCases = {
        {COE::Element::SemiMajorAxis, "SemiMajorAxis"},
        {COE::Element::Eccentricity, "Eccentricity"},
        {COE::Element::Inclination, "Inclination"},
        {COE::Element::Aop, "Aop"},
        {COE::Element::Raan, "Raan"},
        {COE::Element::TrueAnomaly, "TrueAnomaly"},
        {COE::Element::MeanAnomaly, "MeanAnomaly"},
        {COE::Element::EccentricAnomaly, "EccentricAnomaly"},
    };

    for (const auto& testCase : testCases)
    {
        const COE::Element element = std::get<0>(testCase);
        const String& expectedString = std::get<1>(testCase);

        EXPECT_EQ(COE::StringFromElement(element), expectedString);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, ConvertAnomaly)
{
    {
        EXPECT_DOUBLE_EQ(
            COE::FromSIVector({7.0e6, 0.0, 0.0, 0.0, 0.0, 0.0}, COE::AnomalyType::True).getTrueAnomaly().inDegrees(),
            0.0
        );
    }

    {
        EXPECT_DOUBLE_EQ(
            COE::FromSIVector({7.0e6, 0.0, 0.0, 0.0, 0.0, 0.0}, COE::AnomalyType::Mean).getMeanAnomaly().inDegrees(),
            0.0
        );
    }

    {
        EXPECT_DOUBLE_EQ(
            COE::FromSIVector({7.0e6, 0.0, 0.0, 0.0, 0.0, 0.0}, COE::AnomalyType::Eccentric)
                .getEccentricAnomaly()
                .inDegrees(),
            0.0
        );
    }
}
