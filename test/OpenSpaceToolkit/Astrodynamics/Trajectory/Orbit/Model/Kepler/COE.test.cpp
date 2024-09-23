/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanShort.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Tuple;
using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::Vector6d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::time::Time;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanLong;
using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanShort;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        coe_ = {
            defaultSemiMajorAxis_,
            defaultEccentricity_,
            defaultInclination_,
            defaultRaan_,
            defaultAop_,
            defaultTrueAnomaly_,
        };
    }

    const Length defaultSemiMajorAxis_ = Length::Kilometers(7000.0);
    const Real defaultEccentricity_ = 0.1;
    const Angle defaultInclination_ = Angle::Degrees(10.0);
    const Angle defaultRaan_ = Angle::Degrees(20.0);
    const Angle defaultAop_ = Angle::Degrees(30.0);
    const Angle defaultTrueAnomaly_ = Angle::Degrees(40.0);

    COE coe_ = COE::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, Constructor)
{
    {
        EXPECT_NO_THROW(COE(defaultSemiMajorAxis_,
                            defaultEccentricity_,
                            defaultInclination_,
                            defaultRaan_,
                            defaultAop_,
                            defaultTrueAnomaly_););
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, EqualToOperator)
{
    {
        EXPECT_TRUE(coe_ == coe_);
    }

    {
        const COE secondCoe = {
            defaultSemiMajorAxis_,
            defaultEccentricity_,
            defaultInclination_,
            Angle::Degrees(21.0),
            defaultAop_,
            defaultTrueAnomaly_,
        };

        EXPECT_FALSE(coe_ == secondCoe);
    }

    {
        EXPECT_FALSE(COE::Undefined() == COE::Undefined());
        EXPECT_FALSE(coe_ == COE::Undefined());
        EXPECT_FALSE(COE::Undefined() == coe_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, NotEqualToOperator)
{
    {
        EXPECT_FALSE(coe_ != coe_);
    }

    {
        const COE secondCoe = {
            defaultSemiMajorAxis_,
            defaultEccentricity_,
            defaultInclination_,
            Angle::Degrees(21.0),
            defaultAop_,
            defaultTrueAnomaly_
        };

        EXPECT_TRUE(coe_ != secondCoe);
    }

    {
        EXPECT_TRUE(COE::Undefined() != COE::Undefined());
        EXPECT_TRUE(coe_ != COE::Undefined());
        EXPECT_TRUE(COE::Undefined() != coe_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << coe_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, IsDefined)
{
    {
        EXPECT_TRUE(coe_.isDefined());
    }

    {
        EXPECT_FALSE(COE::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetSemiMajorAxis)
{
    {
        EXPECT_EQ(defaultSemiMajorAxis_, coe_.getSemiMajorAxis());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getSemiMajorAxis());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetEccentricity)
{
    {
        EXPECT_EQ(defaultEccentricity_, coe_.getEccentricity());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getEccentricity());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetInclination)
{
    {
        EXPECT_EQ(defaultInclination_, coe_.getInclination());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getInclination());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetRaan)
{
    {
        EXPECT_EQ(defaultRaan_, coe_.getRaan());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getRaan());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetAop)
{
    {
        EXPECT_EQ(defaultAop_, coe_.getAop());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getAop());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetTrueAnomaly)
{
    {
        EXPECT_EQ(defaultTrueAnomaly_, coe_.getTrueAnomaly());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getTrueAnomaly());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetPeriapsisRadius)
{
    {
        EXPECT_DOUBLE_EQ(
            defaultSemiMajorAxis_.inMeters() * (1.0 - defaultEccentricity_), coe_.getPeriapsisRadius().inMeters()
        );
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getPeriapsisRadius());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetApoapsisRadius)
{
    {
        EXPECT_DOUBLE_EQ(
            defaultSemiMajorAxis_.inMeters() * (1.0 + defaultEccentricity_), coe_.getApoapsisRadius().inMeters()
        );
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getApoapsisRadius());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetSemiLatusRectum)
{
    {
        EXPECT_DOUBLE_EQ(
            defaultSemiMajorAxis_.inMeters() * (1.0 - std::pow(defaultEccentricity_, 2)),
            coe_.getSemiLatusRectum().inMeters()
        );
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getSemiLatusRectum());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetRadialDistance)
{
    {
        EXPECT_DOUBLE_EQ(
            defaultSemiMajorAxis_.inMeters() * (1.0 - std::pow(defaultEccentricity_, 2)) /
                (1.0 + defaultEccentricity_ * std::cos(defaultTrueAnomaly_.inRadians())),
            coe_.getRadialDistance().inMeters()
        );
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getRadialDistance());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetAngularMomentum)
{
    {
        const Derived angularMomentum = coe_.getAngularMomentum(Earth::EGM2008.gravitationalParameter_);

        EXPECT_DOUBLE_EQ(
            std::sqrt(
                Earth::EGM2008.gravitationalParameter_.in(Earth::EGM2008.gravitationalParameter_.getUnit()) *
                coe_.getSemiLatusRectum().inMeters()
            ),
            angularMomentum.in(angularMomentum.getUnit())
        );
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getAngularMomentum(Earth::EGM2008.gravitationalParameter_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ComputeMeanLTAN)
{
    {
        const Array<Tuple<String, String, String, Real>> testData = {
            {"2024-09-23T19:34:00.000", "09:05:14.305", "09:05:13.710", 359.782729651204},
        };

        for (const auto& data : testData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(std::get<0>(data)), Scale::UTC);
            const Time expectedLTAN = Time::Parse(std::get<1>(data));
            const Time expectedMLTAN = Time::Parse(std::get<2>(data));
            const Angle raan = Angle::Degrees(std::get<3>(data));

            const Time meanLTAN = COE::ComputeMeanLTAN(raan, instant);

            EXPECT_EQ(meanLTAN.getHour(), expectedMLTAN.getHour());
            EXPECT_EQ(meanLTAN.getMinute(), expectedMLTAN.getMinute());
            EXPECT_NEAR(meanLTAN.getFloatingSeconds(), expectedMLTAN.getFloatingSeconds(), 1e-3);
        }
    }
}

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetMeanAnomaly)
// {

//     using ostk::core::type::Real ;

//     using ostk::mathematics::object::Vector3d ;

//     using ostk::physics::unit::Length ;
//     using ostk::physics::unit::Angle ;
//     using ostk::physics::unit::Derived ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::coordinate::Position ;
//     using ostk::physics::coordinate::Velocity ;
//     using ostk::physics::environment::object::celestial::Earth ;

//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE ;

//     {

//         FAIL() ;

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetEccentricAnomaly)
// {

//     using ostk::core::type::Real ;

//     using ostk::mathematics::object::Vector3d ;

//     using ostk::physics::unit::Length ;
//     using ostk::physics::unit::Angle ;
//     using ostk::physics::unit::Derived ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::coordinate::Position ;
//     using ostk::physics::coordinate::Velocity ;
//     using ostk::physics::environment::object::celestial::Earth ;

//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE ;

//     {

//         FAIL() ;

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetMeanMotion)
// {

//     using ostk::core::type::Real ;

//     using ostk::mathematics::object::Vector3d ;

//     using ostk::physics::unit::Length ;
//     using ostk::physics::unit::Angle ;
//     using ostk::physics::unit::Derived ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::coordinate::Position ;
//     using ostk::physics::coordinate::Velocity ;
//     using ostk::physics::environment::object::celestial::Earth ;

//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE ;

//     {

//         FAIL() ;

//     }

// }

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetNodalPrecessionRate)
{
    {
        const COE coe = {
            Earth::EGM2008.equatorialRadius_ + Length::Kilometers(800.0),
            0.0,
            Angle::Degrees(56.0),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
        };
        const Derived nodalPrecessionRate = coe.getNodalPrecessionRate(
            Earth::EGM2008.gravitationalParameter_, Length::Meters(6.378137e6), 1.08262668e-3
        );

        EXPECT_NEAR(nodalPrecessionRate.in(nodalPrecessionRate.getUnit()), -7.44e-7, 1e-6);
    }
}

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetOrbitalPeriod)
// {

//     using ostk::core::type::Real ;

//     using ostk::mathematics::object::Vector3d ;

//     using ostk::physics::unit::Length ;
//     using ostk::physics::unit::Angle ;
//     using ostk::physics::unit::Derived ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::coordinate::Position ;
//     using ostk::physics::coordinate::Velocity ;
//     using ostk::physics::environment::object::celestial::Earth ;

//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE ;

//     {

//         FAIL() ;

//     }

// }

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetCartesianState)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetVector)
{
    {
        const Vector6d coeVector = {
            defaultSemiMajorAxis_.inMeters(),
            defaultEccentricity_,
            defaultInclination_.inRadians(),
            defaultRaan_.inRadians(),
            defaultAop_.inRadians(),
            defaultTrueAnomaly_.inRadians(),
        };

        EXPECT_TRUE(((coeVector - coe_.getSIVector(COE::AnomalyType::True)).sum() < 1e-15));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, Undefined)
{
    {
        EXPECT_NO_THROW(COE::Undefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, Cartesian)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, FromSIVector)
{
    {
        const Vector6d coeVector = {
            defaultSemiMajorAxis_.inMeters(),
            defaultEccentricity_,
            defaultInclination_.inRadians(),
            defaultRaan_.inRadians(),
            defaultAop_.inRadians(),
            defaultTrueAnomaly_.inRadians(),
        };

        const COE coeFromVector = COE::FromSIVector(coeVector, COE::AnomalyType::True);
        EXPECT_TRUE(coeFromVector == coe_);
    }
}

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, EccentricAnomalyFromTrueAnomaly)
// {

//     using ostk::core::type::Real ;

//     using ostk::mathematics::object::Vector3d ;

//     using ostk::physics::unit::Length ;
//     using ostk::physics::unit::Angle ;
//     using ostk::physics::unit::Derived ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::coordinate::Position ;
//     using ostk::physics::coordinate::Velocity ;
//     using ostk::physics::environment::object::celestial::Earth ;

//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE ;

//     {

//         FAIL() ;

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, TrueAnomalyFromEccentricAnomaly)
// {

//     using ostk::core::type::Real ;

//     using ostk::mathematics::object::Vector3d ;

//     using ostk::physics::unit::Length ;
//     using ostk::physics::unit::Angle ;
//     using ostk::physics::unit::Derived ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::coordinate::Position ;
//     using ostk::physics::coordinate::Velocity ;
//     using ostk::physics::environment::object::celestial::Earth ;

//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE ;

//     {

//         FAIL() ;

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, MeanAnomalyFromEccentricAnomaly)
// {

//     using ostk::core::type::Real ;

//     using ostk::mathematics::object::Vector3d ;

//     using ostk::physics::unit::Length ;
//     using ostk::physics::unit::Angle ;
//     using ostk::physics::unit::Derived ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::coordinate::Position ;
//     using ostk::physics::coordinate::Velocity ;
//     using ostk::physics::environment::object::celestial::Earth ;

//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE ;

//     {

//         FAIL() ;

//     }

// }

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, EccentricAnomalyFromMeanAnomaly)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, TrueAnomalyFromMeanAnomaly)
{
    {
        const Angle meanAnomaly = Angle::Radians(5.08731632317414);
        const Real eccentricity = 0.001288522395593299;
        const Real tolerance = 1e-15;

        const Angle trueAnomaly = COE::TrueAnomalyFromMeanAnomaly(meanAnomaly, eccentricity, tolerance);

        EXPECT_NEAR(trueAnomaly.inRadians(), 5.084916881853494, tolerance);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, StringFromElement)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ConvertAnomaly)
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
