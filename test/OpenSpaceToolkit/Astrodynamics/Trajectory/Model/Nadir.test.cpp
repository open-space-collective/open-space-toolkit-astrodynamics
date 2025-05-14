/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Nadir.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

#include <Global.test.hpp>

using ostk::core::type::Shared;

using ostk::mathematics::object::Vector6d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astrodynamics::trajectory::model::Nadir;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::Kepler;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Nadir : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Earth earth = Earth::WGS84();
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const COE coe = {
            Length::Kilometers(7000.0),  // Semi-major axis
            0.0,                         // Eccentricity
            Angle::Degrees(98.0),        // Inclination
            Angle::Degrees(0.0),         // RAAN
            Angle::Degrees(0.0),         // AOP
            Angle::Degrees(0.0)          // True anomaly
        };

        const Kepler keplerianModel = {
            coe,
            epoch_,
            EarthGravitationalModel::EGM2008.gravitationalParameter_,
            EarthGravitationalModel::EGM2008.equatorialRadius_,
            EarthGravitationalModel::EGM2008.J2_,
            EarthGravitationalModel::EGM2008.J4_,
            Kepler::PerturbationType::None
        };

        orbit_ = {keplerianModel, std::make_shared<Earth>(earth)};
    }

    const Instant epoch_ = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
    Orbit orbit_ = Orbit::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Nadir, Constructor)
{
    EXPECT_NO_THROW(Nadir nadirModel(orbit_));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Nadir, IsDefined)
{
    const Nadir nadirModel(orbit_);

    EXPECT_TRUE(nadirModel.isDefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Nadir, CalculateStateAt)
{
    const Nadir nadirModel(orbit_);

    const State state = nadirModel.calculateStateAt(epoch_);

    const VectorXd coordinates = state.getCoordinates();

    // Value obtained from Orekit

    const Vector6d expectedCoordinates = {
        6378136.936099239,
        2.6122573763132095E-4,
        -6.584354159011127,
        -0.07204923298218091,
        -956.9095709619337,
        6804.692417661864,
    };

    EXPECT_VECTORS_ALMOST_EQUAL(coordinates, expectedCoordinates, 1e-5);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Nadir, Clone)
{
    const Nadir nadirModel(orbit_);

    Nadir* clonedModel = nadirModel.clone();

    EXPECT_TRUE(clonedModel != nullptr);
    EXPECT_TRUE(*clonedModel == nadirModel);

    delete clonedModel;
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Nadir, Print)
{
    const Nadir nadirModel(orbit_);

    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(nadirModel.print(std::cout, true));
    EXPECT_NO_THROW(nadirModel.print(std::cout, false));

    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_FALSE(output.empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Nadir, StreamOperator)
{
    const Nadir nadirModel(orbit_);

    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(std::cout << nadirModel);

    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_FALSE(output.empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Nadir, EqualityOperator)
{
    const Nadir nadirModel1(orbit_);
    const Nadir nadirModel2(orbit_);

    EXPECT_TRUE(nadirModel1 == nadirModel2);

    const Orbit differentOrbit = Orbit::Undefined();
    const Nadir nadirModel3(differentOrbit);

    EXPECT_FALSE(nadirModel1 == nadirModel3);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Nadir, InequalityOperator)
{
    const Nadir nadirModel1(orbit_);
    const Nadir nadirModel2(orbit_);

    EXPECT_FALSE(nadirModel1 != nadirModel2);

    const Orbit differentOrbit = Orbit::Undefined();
    const Nadir nadirModel3(differentOrbit);

    EXPECT_TRUE(nadirModel1 != nadirModel3);
}
