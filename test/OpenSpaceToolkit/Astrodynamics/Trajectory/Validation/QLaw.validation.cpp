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

#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Time/Time.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solver/FiniteDifferenceSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
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
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::data::Scalar;
using ostk::physics::Environment;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using ostk::physics::environment::Object;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Time;
using ostk::physics::time::Scale;
using ostk::physics::unit::Mass;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::guidancelaw::QLaw;
using ostk::astrodynamics::solver::FiniteDifferenceSolver;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

class OpenSpaceToolkit_Astrodynamics_Validation_QLawValidation
    : public ::testing::Test,
      public ::testing::WithParamInterface<Tuple<QLaw::GradientStrategy>>
{
   protected:
    void SetUp() override
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        const PropulsionSystem propulsionSystem = {
            Scalar(0.1, PropulsionSystem::thrustSIUnit),
            Scalar(1500.0, PropulsionSystem::specificImpulseSIUnit),
        };

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

INSTANTIATE_TEST_SUITE_P(
    QLaw_GradientStrategy,
    OpenSpaceToolkit_Astrodynamics_Validation_QLawValidation,
    testing::Values(
        std::make_tuple(QLaw::GradientStrategy::FiniteDifference), std::make_tuple(QLaw::GradientStrategy::Analytical)
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Validation_QLawValidation, QLaw_Paper_Case_A)
{
    const auto testParameters = GetParam();
    const QLaw::GradientStrategy gradientStrategy = std::get<0>(testParameters);

    const COE targetCOE = {
        Length::Meters(42000.0e3),
        0.01,
        Angle::Degrees(0.05),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
    };

    const QLaw::Parameters parameters = {
        {
            {COE::Element::SemiMajorAxis, {1.0, 100.0}},
            {COE::Element::Eccentricity, {1.0, 1e-3}},
        },
        3,
        4,
        2,
        0.01,
        100,
        1.0,
        Length::Kilometers(6578.0)
    };

    const Derived gravitationalParameter =
        Derived(398600.49 * 1e9, EarthGravitationalModel::EGM2008.gravitationalParameter_.getUnit());

    const Shared<QLaw> qlaw =
        std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

    const Mass mass = Mass::Kilograms(50.0);
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    const PropulsionSystem propulsionSystem = {
        Scalar(1.0, PropulsionSystem::thrustSIUnit),
        Scalar(3100.0, PropulsionSystem::specificImpulseSIUnit),
    };

    const SatelliteSystem satelliteSystem = {
        mass,
        satelliteGeometry,
        Matrix3d::Identity(),
        1.0,
        2.1,
        propulsionSystem,
    };

    const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
    const Shared<CentralBodyGravity> centralBodyGravity =
        std::make_shared<CentralBodyGravity>(CentralBodyGravity(std::make_shared<Earth>(Earth::Spherical())));
    const Shared<PositionDerivative> positionDerivative = std::make_shared<PositionDerivative>(PositionDerivative());
    const Array<Shared<Dynamics>> dynamics = {
        thruster,
        centralBodyGravity,
        positionDerivative,
    };

    const NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        120.0,
        1e-12,
        1e-12,
    };

    const Propagator propagator = {numericalSolver, dynamics};

    const COE currentCOE = {
        Length::Meters(7000.0e3),
        0.01,
        Angle::Degrees(0.05),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
    };

    const COE::CartesianState cartesianState = currentCOE.getCartesianState(gravitationalParameter, Frame::GCRF());

    VectorXd coordinates(7);
    coordinates << cartesianState.first.getCoordinates(), cartesianState.second.getCoordinates(), 300.0;

    const State initialState = {
        Instant::J2000(),
        coordinates,
        Frame::GCRF(),
        {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()}
    };

    const State state =
        propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Days(14.86522));

    const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

    EXPECT_TRUE(std::abs(endCOE.getSemiMajorAxis().inMeters() - targetCOE.getSemiMajorAxis().inMeters()) < 60000.0);
}

TEST_P(OpenSpaceToolkit_Astrodynamics_Validation_QLawValidation, QLaw_Paper_Case_E)
{
    GTEST_SKIP() << "Skipping test as it does not produce comparable results between Release + Debug.";

    const auto testParameters = GetParam();
    const QLaw::GradientStrategy gradientStrategy = std::get<0>(testParameters);

    const COE targetCOE = {
        Length::Meters(26500.0e3),
        0.7,
        Angle::Degrees(116.0),
        Angle::Degrees(180.0),
        Angle::Degrees(270.0),
        Angle::Degrees(0.0),
    };

    const QLaw::Parameters parameters = {
        {
            {COE::Element::SemiMajorAxis, {1.0, 100.0}},
            {COE::Element::Eccentricity, {1.0, 1e-3}},
            {COE::Element::Inclination, {1.0, 1e-4}},
            {COE::Element::Raan, {1.0, 1e-4}},
            {COE::Element::Aop, {1.0, 1e-4}},
        },
    };

    const Derived gravitationalParameter =
        Derived(398600.49 * 1e9, EarthGravitationalModel::EGM2008.gravitationalParameter_.getUnit());

    const Shared<QLaw> qlaw =
        std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

    const Mass mass = Mass::Kilograms(50.0);
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    const PropulsionSystem propulsionSystem = {
        Scalar(2.0, PropulsionSystem::thrustSIUnit),
        Scalar(2000.0, PropulsionSystem::specificImpulseSIUnit),
    };

    const SatelliteSystem satelliteSystem = {
        mass,
        satelliteGeometry,
        Matrix3d::Identity(),
        1.0,
        2.1,
        propulsionSystem,
    };

    const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
    const Shared<CentralBodyGravity> centralBodyGravity =
        std::make_shared<CentralBodyGravity>(CentralBodyGravity(std::make_shared<Earth>(Earth::Spherical())));
    const Shared<PositionDerivative> positionDerivative = std::make_shared<PositionDerivative>(PositionDerivative());
    const Array<Shared<Dynamics>> dynamics = {
        thruster,
        centralBodyGravity,
        positionDerivative,
    };

    const NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        5.0,
        1e-12,
        1e-12,
    };

    const Propagator propagator = {numericalSolver, dynamics};

    const COE currentCOE = {
        Length::Meters(24505.9e3),
        0.725,
        Angle::Degrees(0.05),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
    };

    const COE::CartesianState cartesianState = currentCOE.getCartesianState(gravitationalParameter, Frame::GCRF());

    VectorXd coordinates(7);
    coordinates << cartesianState.first.getCoordinates(), cartesianState.second.getCoordinates(), 2000.0;

    const State initialState = {
        Instant::J2000(),
        coordinates,
        Frame::GCRF(),
        {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()}
    };

    const State state =
        propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Days(72.49716435185185187));

    const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

    EXPECT_TRUE(std::abs(endCOE.getSemiMajorAxis().inMeters() - targetCOE.getSemiMajorAxis().inMeters()) < 50000.0);

    // TBI: These don't close yes, have to investigate why
    // EXPECT_TRUE(std::abs(endCOE.getEccentricity() - targetCOE.getEccentricity()) < 1e-2);
    // EXPECT_TRUE(std::abs(endCOE.getInclination().inDegrees() - targetCOE.getInclination().inDegrees()) < 1.0);
    // EXPECT_TRUE(std::abs(endCOE.getRaan().inDegrees() - targetCOE.getRaan().inDegrees()) < 1.0);
    // EXPECT_TRUE(std::abs(endCOE.getAop().inDegrees() - targetCOE.getAop().inDegrees()) < 1.0);
}

TEST_P(OpenSpaceToolkit_Astrodynamics_Validation_QLawValidation, SSO_targeting)
{
    const auto testParameters = GetParam();
    const QLaw::GradientStrategy gradientStrategy = std::get<0>(testParameters);

    const Orbit orbit = Orbit::SunSynchronous(
        Instant::J2000(), Length::Meters(585.0e3), Time(10, 0, 0), std::make_shared<Earth>(Earth::Default())
    );

    const Derived gravitationalParameter =
        Derived(398600.49 * 1e9, EarthGravitationalModel::EGM2008.gravitationalParameter_.getUnit());

    const State currentState = orbit.getStateAt(Instant::J2000());
    const COE currentCOE =
        COE::Cartesian({currentState.getPosition(), currentState.getVelocity()}, gravitationalParameter);

    const Mass mass = Mass::Kilograms(200.0);
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    const PropulsionSystem propulsionSystem = {
        Scalar(1.0, PropulsionSystem::thrustSIUnit),
        Scalar(1000.0, PropulsionSystem::specificImpulseSIUnit),
    };

    const SatelliteSystem satelliteSystem = {
        mass,
        satelliteGeometry,
        Matrix3d::Identity(),
        1.0,
        2.1,
        propulsionSystem,
    };

    const Shared<CentralBodyGravity> centralBodyGravity =
        std::make_shared<CentralBodyGravity>(CentralBodyGravity(std::make_shared<Earth>(Earth::Spherical())));
    const Shared<PositionDerivative> positionDerivative = std::make_shared<PositionDerivative>(PositionDerivative());

    const NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        3.0,
        1e-12,
        1e-12,
    };

    VectorXd coordinates(7);
    coordinates << currentState.getCoordinates(), 214.0;

    const State initialState = {
        Instant::DateTime(DateTime(2024, 10, 1, 0, 0, 0), Scale::UTC),
        coordinates,
        Frame::GCRF(),
        {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()}
    };

    // SMA Targeting
    {
        const COE targetCOE = {
            currentCOE.getSemiMajorAxis() + Length::Meters(5000.0),
            currentCOE.getEccentricity(),
            currentCOE.getInclination(),
            currentCOE.getRaan(),
            currentCOE.getAop(),
            currentCOE.getTrueAnomaly(),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::SemiMajorAxis, {1.0, 50.0}},
            },
        };

        const Shared<QLaw> qlaw =
            std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

        const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
        const Array<Shared<Dynamics>> dynamics = {
            thruster,
            centralBodyGravity,
            positionDerivative,
        };

        const Propagator propagator = {numericalSolver, dynamics};

        const State state =
            propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Minutes(15.0));

        const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

        EXPECT_LT(std::abs(endCOE.getSemiMajorAxis().inMeters() - targetCOE.getSemiMajorAxis().inMeters()), 100.0);
    }

    // Eccentricity targeting
    {
        const COE targetCOE = {
            currentCOE.getSemiMajorAxis(),
            currentCOE.getEccentricity() + 1e-3,
            currentCOE.getInclination(),
            currentCOE.getRaan(),
            currentCOE.getAop(),
            currentCOE.getTrueAnomaly(),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::Eccentricity, {1.0, 1e-4}},
            },
        };

        const Shared<QLaw> qlaw =
            std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

        const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
        const Array<Shared<Dynamics>> dynamics = {
            thruster,
            centralBodyGravity,
            positionDerivative,
        };

        const Propagator propagator = {numericalSolver, dynamics};

        const State state =
            propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Minutes(21.0));

        const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

        EXPECT_LT(std::abs(endCOE.getEccentricity() - targetCOE.getEccentricity()), 1e-4);
    }

    // Inclination targeting
    {
        const COE targetCOE = {
            currentCOE.getSemiMajorAxis(),
            currentCOE.getEccentricity(),
            currentCOE.getInclination() + Angle::Degrees(1e-2),
            currentCOE.getRaan(),
            currentCOE.getAop(),
            currentCOE.getTrueAnomaly(),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::Inclination, {1.0, 1e-4}},
            },
        };

        const Shared<QLaw> qlaw =
            std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

        const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
        const Array<Shared<Dynamics>> dynamics = {
            thruster,
            centralBodyGravity,
            positionDerivative,
        };

        const Propagator propagator = {numericalSolver, dynamics};

        const State state =
            propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Hours(1.5));

        const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

        EXPECT_LT(std::abs(endCOE.getInclination().inRadians() - targetCOE.getInclination().inRadians()), 1e-4);
    }

    // // Right Ascension of the Ascending Node targeting
    {
        const COE targetCOE = {
            currentCOE.getSemiMajorAxis(),
            currentCOE.getEccentricity(),
            currentCOE.getInclination(),
            currentCOE.getRaan() - Angle::Degrees(1e-1),
            currentCOE.getAop(),
            currentCOE.getTrueAnomaly(),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::Raan, {1.0, 1e-4}},
            },
        };

        const Shared<QLaw> qlaw =
            std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

        const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
        const Array<Shared<Dynamics>> dynamics = {
            thruster,
            centralBodyGravity,
            positionDerivative,
        };

        const Propagator propagator = {numericalSolver, dynamics};

        const State state =
            propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Hours(1.5));

        const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

        EXPECT_LT(std::abs(endCOE.getRaan().inRadians() - targetCOE.getRaan().inRadians()), 1e-4);
    }

    // Semi-Major Axis + Eccentricity targeting
    {
        const COE targetCOE = {
            currentCOE.getSemiMajorAxis() + Length::Meters(5000.0),
            currentCOE.getEccentricity() + 1e-3,
            currentCOE.getInclination(),
            currentCOE.getRaan(),
            currentCOE.getAop(),
            currentCOE.getTrueAnomaly(),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::SemiMajorAxis, {1.0, 100.0}},
                {COE::Element::Eccentricity, {1.0, 1e-4}},
            },
        };

        const Shared<QLaw> qlaw =
            std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

        const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
        const Array<Shared<Dynamics>> dynamics = {
            thruster,
            centralBodyGravity,
            positionDerivative,
        };

        const Propagator propagator = {numericalSolver, dynamics};

        const State state =
            propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Minutes(40.0));

        const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

        EXPECT_LT(std::abs(endCOE.getSemiMajorAxis().inMeters() - targetCOE.getSemiMajorAxis().inMeters()), 100.0);
        EXPECT_LT(std::abs(endCOE.getEccentricity() - targetCOE.getEccentricity()), 1e-4);
    }

    // Semi-Major Axis + Eccentricity + Inclination targeting
    {
        const COE targetCOE = {
            currentCOE.getSemiMajorAxis() + Length::Meters(5000.0),
            currentCOE.getEccentricity() + 1e-3,
            currentCOE.getInclination() + Angle::Degrees(1e-1),
            currentCOE.getRaan(),
            currentCOE.getAop(),
            currentCOE.getTrueAnomaly(),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::SemiMajorAxis, {1.0, 100.0}},
                {COE::Element::Eccentricity, {1.0, 1e-3}},
                {COE::Element::Inclination, {1.0, 1e-4}},
            },
        };

        const Shared<QLaw> qlaw =
            std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

        const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
        const Array<Shared<Dynamics>> dynamics = {
            thruster,
            centralBodyGravity,
            positionDerivative,
        };

        const Propagator propagator = {numericalSolver, dynamics};

        const State state =
            propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Minutes(130.0));

        const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

        EXPECT_LT(std::abs(endCOE.getSemiMajorAxis().inMeters() - targetCOE.getSemiMajorAxis().inMeters()), 100.0);
        EXPECT_LT(std::abs(endCOE.getEccentricity() - targetCOE.getEccentricity()), 1e-3);
        EXPECT_LT(std::abs(endCOE.getInclination().inRadians() - targetCOE.getInclination().inRadians()), 1e-4);
    }

    // Semi-Major Axis + Eccentricity + Inclination + Right Ascension of Ascending Node targeting
    {
        const COE targetCOE = {
            currentCOE.getSemiMajorAxis() + Length::Meters(5000.0),
            currentCOE.getEccentricity() + 1e-3,
            currentCOE.getInclination() + Angle::Degrees(1e-1),
            currentCOE.getRaan() - Angle::Degrees(1e-1),
            currentCOE.getAop(),
            currentCOE.getTrueAnomaly(),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::SemiMajorAxis, {1.0, 100.0}},
                {COE::Element::Eccentricity, {1.0, 1e-3}},
                {COE::Element::Inclination, {1.0, 1e-4}},
                {COE::Element::Raan, {1.0, 1e-4}},
            },
        };

        const Shared<QLaw> qlaw =
            std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

        const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
        const Array<Shared<Dynamics>> dynamics = {
            thruster,
            centralBodyGravity,
            positionDerivative,
        };

        const Propagator propagator = {numericalSolver, dynamics};

        const State state =
            propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Minutes(145.5));

        const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

        EXPECT_LT(std::abs(endCOE.getSemiMajorAxis().inMeters() - targetCOE.getSemiMajorAxis().inMeters()), 100.0);
        EXPECT_LT(std::abs(endCOE.getEccentricity() - targetCOE.getEccentricity()), 1e-3);
        EXPECT_LT(std::abs(endCOE.getInclination().inRadians() - targetCOE.getInclination().inRadians()), 1e-4);
        EXPECT_LT(std::abs(endCOE.getRaan().inRadians() - targetCOE.getRaan().inRadians()), 1e-4);
    }
}
