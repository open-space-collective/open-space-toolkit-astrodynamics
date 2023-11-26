/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/BrouwerLyddaneMean/BrouwerLyddaneMean.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::Index;
using ostk::core::ctnr::Tuple;

using ostk::math::object::Vector3d;
using ostk::math::object::Vector6d;

using ostk::physics::units::Angle;
using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::units::Length;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astro::trajectory::orbit::models::blm::BrouwerLyddaneMean;
using ostk::astro::trajectory::orbit::models::kepler::COE;

class BrouwerLyddaneMeanMock : public BrouwerLyddaneMean
{
   public:
    BrouwerLyddaneMeanMock(
        const Length &aSemiMajorAxis,
        const Real &anEccentricity,
        const Angle &anInclination,
        const Angle &aRaan,
        const Angle &anAop,
        const Angle &aMeanAnomaly
    )
        : BrouwerLyddaneMean(aSemiMajorAxis, anEccentricity, anInclination, aRaan, anAop, aMeanAnomaly)
    {
    }

    MOCK_METHOD(COE, toCOE, (), (const, override));
};

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMean : public ::testing::Test
{
   protected:
    const Length semiMajorAxis_ = Length::Kilometers(6973.743736075629);
    const Real eccentricity_ = 0.001202049190851806;
    const Angle inclination_ = Angle::Degrees(97.8128984109556);
    const Angle raan_ = Angle::Degrees(19.06531022901672);
    const Angle aop_ = Angle::Degrees(89.87616473223646);
    const Angle meanAnomaly_ = Angle::Degrees(270.112053453241);

    const BrouwerLyddaneMeanMock brouwerLyddaneMean_ = BrouwerLyddaneMeanMock {
        semiMajorAxis_,
        eccentricity_,
        inclination_,
        raan_,
        aop_,
        meanAnomaly_,
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMean, Constructor)
{
    {
        EXPECT_NO_THROW(BrouwerLyddaneMeanMock(semiMajorAxis_, eccentricity_, inclination_, raan_, aop_, meanAnomaly_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMean, Getters)
{
    {
        EXPECT_TRUE(brouwerLyddaneMean_.getMeanAnomaly() == meanAnomaly_);
        EXPECT_TRUE(
            brouwerLyddaneMean_.getTrueAnomaly() == COE::TrueAnomalyFromMeanAnomaly(meanAnomaly_, eccentricity_, 1e-15)
        );
        EXPECT_TRUE(
            brouwerLyddaneMean_.getEccentricAnomaly() ==
            COE::EccentricAnomalyFromMeanAnomaly(meanAnomaly_, eccentricity_, 1e-15)
        );
    }

    {
        const BrouwerLyddaneMeanMock brouwerLyddaneMeanMock(
            Length::Undefined(), eccentricity_, inclination_, raan_, aop_, meanAnomaly_
        );
        EXPECT_ANY_THROW(brouwerLyddaneMeanMock.getMeanAnomaly());
        EXPECT_ANY_THROW(brouwerLyddaneMeanMock.getTrueAnomaly());
        EXPECT_ANY_THROW(brouwerLyddaneMeanMock.getEccentricAnomaly());
    }
}
