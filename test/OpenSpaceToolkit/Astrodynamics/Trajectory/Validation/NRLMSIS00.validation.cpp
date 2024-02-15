/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/CSSISpaceWeather.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/Exponential.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/Manager.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystemBuilder.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Tuple;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;
using ostk::core::type::Size;

using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::Environment;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using SWManager = ostk::physics::environment::atmospheric::earth::Manager;
using ostk::physics::environment::atmospheric::earth::CSSISpaceWeather;
using ostk::physics::environment::Object;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::AtmosphericDrag;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::flight::system::SatelliteSystemBuilder;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

class OpenSpaceToolkit_Astrodynamics_Validation_NRLMSIS00Validation : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        const PropulsionSystem propulsionSystem = {0.1, 1500.0};

        this->satelliteGeometry_ = satelliteGeometry;
        this->propulsionSystem_ = propulsionSystem;

        this->satelliteSystem_ = {
            this->satelliteDryMass_,
            satelliteGeometry_,
            Matrix3d::Identity(),
            1.0,
            2.1,
            propulsionSystem_,
        };

        this->earthSpherical_ = std::make_shared<Celestial>(Earth::Spherical());
        this->defaultDynamics_ = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earthSpherical_),
        };

        this->defaultPropagator_ = {defaultNumericalSolver_, defaultDynamics_};
    }

    const NumericalSolver defaultNumericalSolver_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        5.0,
        1.0e-15,
        1.0e-15,
    };

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    Array<Shared<Dynamics>> defaultDynamics_ = Array<Shared<Dynamics>>::Empty();
    const Mass satelliteDryMass_ = Mass(100.0, Mass::Unit::Kilogram);

    PropulsionSystem propulsionSystem_ = PropulsionSystem::Undefined();
    Composite satelliteGeometry_ = Composite::Undefined();
    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();
    Shared<Celestial> earthSpherical_ = nullptr;
    Propagator defaultPropagator_ = Propagator::Undefined();

    const Shared<CoordinateBroker> dragCoordinateBrokerSPtr_ = std::make_shared<CoordinateBroker>(CoordinateBroker(
        {CartesianPosition::Default(),
         CartesianVelocity::Default(),
         CoordinateSubset::Mass(),
         CoordinateSubset::SurfaceArea(),
         CoordinateSubset::DragCoefficient()}
    ));
};

class OpenSpaceToolkit_Astrodynamics_Validation_NRLMSIS00Validation_Data_Success
    : public OpenSpaceToolkit_Astrodynamics_Validation_NRLMSIS00Validation,
      public ::testing::WithParamInterface<Tuple<Instant, Instant, Duration, std::string>>
{
};

INSTANTIATE_TEST_SUITE_P(
    Input_Data_Spans_Valid,
    OpenSpaceToolkit_Astrodynamics_Validation_NRLMSIS00Validation_Data_Success,
    testing::Values(std::make_tuple(
        Instant::Now() - Duration::Weeks(52 * 5),
        Instant::Now() - Duration::Weeks(52 * 4.9),
        Duration::Days(7),
        "NRLMSISE Past Data limit pass"
    )
                    // [TBI]: This test case fails sporadically because the actual data files are never updated based on
                    // age, meaning they can get stale.

                    // std::make_tuple(
                    //     Instant::Now() + Duration::Weeks(52 * 0.9),
                    //     Instant::Now() + Duration::Weeks(52 * 1.0),
                    //     Duration::Days(7),
                    //     "IERS Finals2000A Future Data limit pass"
                    // )
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Validation_NRLMSIS00Validation_Data_Success, EdgeCases_Input_Data_Horizon)
{
    // long-horizon, low granularity propagation to test input data availability

    // currently limited by:
    // -5 years in the past due to CSSI space weather data when computing drag
    // +1 year in the future due to finals2000a prediction span when computing polar motion for frame conversions

    const auto parameters = GetParam();

    const Instant startInstant = std::get<0>(parameters);
    const Instant endInstant = std::get<1>(parameters);
    const Duration step = std::get<2>(parameters);

    const Real mass = satelliteDryMass_.inKilograms();
    const Real surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea();
    const Real dragCoefficient = satelliteSystem_.getDragCoefficient();

    VectorXd initialStateVector(9);
    initialStateVector << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, mass, surfaceArea,
        dragCoefficient;

    // Current state and instant setup
    const State state = {startInstant, initialStateVector, gcrfSPtr_, dragCoordinateBrokerSPtr_};

    const NumericalSolver RK4 = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        step.inSeconds(),
        1.0e-15,
        1.0e-15,
    };

    // Setup dynamics
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::EGM2008),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::NRLMSISE00)
    );

    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
    };

    // Setup Propagator model and orbit
    const Propagator propagator = {
        RK4,
        dynamics,
    };

    // Propagate all states
    EXPECT_NO_THROW(propagator.calculateStateAt(state, endInstant));
}

class OpenSpaceToolkit_Astrodynamics_Validation_NRLMSIS00Validation_Data_Failure
    : public OpenSpaceToolkit_Astrodynamics_Validation_NRLMSIS00Validation,
      public ::testing::WithParamInterface<Tuple<Instant, Instant, Duration, std::string>>
{
};

INSTANTIATE_TEST_SUITE_P(
    Input_Data_Spans_Invalid,
    OpenSpaceToolkit_Astrodynamics_Validation_NRLMSIS00Validation_Data_Failure,
    testing::Values(
        std::make_tuple(
            Instant::Now() - Duration::Weeks(52 * 6.2),
            Instant::Now() - Duration::Weeks(52 * 6.3),
            Duration::Days(7),
            "NRLMSISE Past Data limit fail"
        ),
        std::make_tuple(
            Instant::Now() + Duration::Weeks(52 * 1.0),
            Instant::Now() + Duration::Weeks(52 * 1.1),
            Duration::Days(7),
            "IERS Finals2000A Future Data limit fail"
        )
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Validation_NRLMSIS00Validation_Data_Failure, EdgeCases_Input_Data_Horizon)
{
    // long-horizon, low granularity propagation to test input data availability

    // currently limited by:
    // -5 years in the past due to CSSI space weather data when computing drag
    // +1 year in the future due to finals2000a prediction span when computing polar motion for frame conversions

    auto parameters = GetParam();

    Instant startInstant = std::get<0>(parameters);
    Instant endInstant = std::get<1>(parameters);
    Duration step = std::get<2>(parameters);

    const Real mass = satelliteDryMass_.inKilograms();
    const Real surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea();
    const Real dragCoefficient = satelliteSystem_.getDragCoefficient();

    VectorXd initialStateVector(9);
    initialStateVector << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, mass, surfaceArea,
        dragCoefficient;

    // Current state and instant setup
    const State state = {startInstant, initialStateVector, gcrfSPtr_, dragCoordinateBrokerSPtr_};

    const NumericalSolver RK4 = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        step.inSeconds(),
        1.0e-15,
        1.0e-15,
    };

    // Setup dynamics
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::EGM2008),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::NRLMSISE00)
    );

    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
    };

    // Setup Propagator model and orbit
    const Propagator propagator = {RK4, dynamics};

    // Propagate all states
    EXPECT_THROW(propagator.calculateStateAt(state, endInstant), ostk::core::error::RuntimeError);
}
