/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Ephemerides/Analytical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Magnetic/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/Exponential.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Table;
using ostk::core::fs::File;
using ostk::core::fs::Path;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::geom::d3::objects::Composite;
using ostk::math::geom::d3::objects::Cuboid;
using ostk::math::geom::d3::objects::Point;
using ostk::math::obj::Matrix3d;
using ostk::math::obj::Vector3d;
using ostk::math::obj::VectorXd;

using ostk::physics::Environment;
using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::env::obj::Celestial;
using ostk::physics::env::obj::celest::Earth;
using ostk::physics::env::obj::celest::Moon;
using ostk::physics::env::obj::celest::Sun;
using ostk::physics::env::ephem::Analytical;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Mass;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

using ostk::astro::NumericalSolver;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::Propagator;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::dynamics::CentralBodyGravity;
using ostk::astro::flight::system::dynamics::ThirdBodyGravity;
using ostk::astro::flight::system::dynamics::AtmosphericDrag;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, ostk::physics::units::Time::Unit::Second);

/* UNIT TESTS */
class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        satelliteSystem_ =
            SatelliteSystem(Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 1.0, 2.1);

        earthSpherical_ = std::make_shared<Celestial>(Earth::Spherical());
        defaultDynamics_.add(std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthSpherical_)));

        defaultPropagator_ = {defaultNumericalSolver_, defaultDynamics_};
    }

    const NumericalSolver defaultNumericalSolver_ = {
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};

    const NumericalSolver defaultRK4_ = {
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKutta4, 30.0, 1.0e-15, 1.0e-15};

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    Array<Shared<Dynamics>> defaultDynamics_ = Array<Shared<Dynamics>>::Empty();
    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();
    Shared<Celestial> earthSpherical_ = nullptr;
    Propagator defaultPropagator_ = Propagator::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, Constructor)
{
    {
        EXPECT_NO_THROW(Propagator(defaultNumericalSolver_, defaultDynamics_));
    }

    {
        Environment environment = Environment::Default();
        EXPECT_NO_THROW(Propagator(defaultNumericalSolver_, environment));

        const Propagator propagator = {defaultNumericalSolver_, environment};

        EXPECT_TRUE(propagator.getDynamics().getSize() == 3);

        EXPECT_TRUE(dynamic_cast<const CentralBodyGravity&>(*propagator.getDynamics()[0]).isDefined());
        EXPECT_TRUE(dynamic_cast<const ThirdBodyGravity&>(*propagator.getDynamics()[1]).isDefined());
        EXPECT_TRUE(dynamic_cast<const ThirdBodyGravity&>(*propagator.getDynamics()[2]).isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, EqualToOperator)
{
    {
        const Propagator propagator_x = {defaultPropagator_};
        EXPECT_TRUE(defaultPropagator_ == propagator_x);

        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15};
        const Propagator propagator_1 = {numericalSolver_1, defaultDynamics_};
        EXPECT_FALSE(defaultPropagator_ == propagator_1);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, NotEqualToOperator)
{
    {
        const Propagator propagator_x = {defaultPropagator_};
        EXPECT_FALSE(defaultPropagator_ != propagator_x);

        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15};
        const Propagator propagator_1 = {numericalSolver_1, defaultDynamics_};
        EXPECT_TRUE(defaultPropagator_ != propagator_1);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, IsDefined)
{
    {
        EXPECT_TRUE(defaultPropagator_.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, Undefined)
{
    {
        EXPECT_NO_THROW(Propagator::Undefined());

        EXPECT_FALSE(Propagator::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultPropagator_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultPropagator_.print(std::cout, true));
        EXPECT_NO_THROW(defaultPropagator_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, getDynamics)
{
    {
        EXPECT_TRUE(defaultPropagator_.getDynamics() == defaultDynamics_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, setDynamics)
{
    {
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 1);

        const Shared<Dynamics> centralBodyGravity =
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthSpherical_));

        defaultPropagator_.setDynamics({centralBodyGravity, centralBodyGravity});

        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 2);

        defaultPropagator_.setDynamics({});

        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, addDynamics)
{
    {
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 1);

        const Shared<Dynamics> centralBodyGravity =
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthSpherical_));

        defaultPropagator_.addDynamics(centralBodyGravity);

        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 2);

        defaultPropagator_.addDynamics(centralBodyGravity);

        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 3);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, clearDynamics)
{
    {
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 1);

        defaultPropagator_.clearDynamics();

        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, CalculateStateAt)
{
    // Current state and instant setup
    const State state = {
        Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};

    // Setup instants
    const Array<Instant> instantArray = {
        Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC)};

    for (const Instant& instant : instantArray)
    {
        EXPECT_NO_THROW(defaultPropagator_.calculateStateAt(state, instant));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, CalculateStatesAt)
{
    // Test exception for unsorted instant array
    {
        // Current state and instant setup
        const State state = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};

        // Setup instants
        Array<Instant> instantArray = Array<Instant>::Empty();
        instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC));
        instantArray.add(Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC));
        instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC));
        instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC));

        EXPECT_ANY_THROW(defaultPropagator_.calculateStatesAt(state, instantArray));
    }

    /// Test full state results against reference trajectory
    // Reference data setup
    const Table referenceData = Table::Load(
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                               "Propagated/CalculateStatesAt_StateValidation.csv")),
        Table::Format::CSV,
        true
    );

    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArray = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArray = Array<Vector3d>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC));

        referencePositionArray.add(
            {referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()}
        );
        referenceVelocityArray.add(
            {referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()}
        );
    }

    // Test forward propagation
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 0;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_)};

        const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].accessPosition();
            const Velocity velocity = propagatedStateArray[i].accessVelocity();

            const double positionError = (position.accessCoordinates() - referencePositionArray[i]).norm();
            const double velocityError = (velocity.accessCoordinates() - referenceVelocityArray[i]).norm();

            ASSERT_GT(2e-7, positionError);
            ASSERT_GT(2e-10, velocityError);
            ASSERT_EQ(propagatedStateArray[i].getInstant(), instantArray[i]);

            // // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // Test forward + backward propagation
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 30;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_)};

        const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].accessPosition();
            const Velocity velocity = propagatedStateArray[i].accessVelocity();

            const double positionError = (position.accessCoordinates() - referencePositionArray[i]).norm();
            const double velocityError = (velocity.accessCoordinates() - referenceVelocityArray[i]).norm();

            ASSERT_GT(2e-7, positionError);
            ASSERT_GT(2e-10, velocityError);
            ASSERT_EQ(propagatedStateArray[i].getInstant(), instantArray[i]);

            // // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // Test backward propagation
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 60;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_)};

        const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].accessPosition();
            const Velocity velocity = propagatedStateArray[i].accessVelocity();

            const double positionError = (position.accessCoordinates() - referencePositionArray[i]).norm();
            const double velocityError = (velocity.accessCoordinates() - referenceVelocityArray[i]).norm();

            ASSERT_GT(2e-7, positionError);
            ASSERT_GT(2e-10, velocityError);
            ASSERT_EQ(propagatedStateArray[i].getInstant(), instantArray[i]);

            // // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, OrekitSpherical)
{
    // Current state and instant setup
    const State state = {
        Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 7546.053290, 0.0}, gcrfSPtr_)};

    const State finalState = defaultPropagator_.calculateStateAt(state, state.getInstant() + Duration::Minutes(60.0));

    VectorXd expectedCoordinates(6);
    expectedCoordinates << -5172890.413848375000, -4716058.194055955000, 0000000.000000000000, 05083.946625786208,
        -05576.415230079790, 00000.000000000000;

    VectorXd residuals = finalState.getCoordinates() - expectedCoordinates;
    ASSERT_TRUE((residuals.array() < 1e-7).all()) << String::Format("Residual: {}", residuals.maxCoeff());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, OrekitExponential)
{
    // Current state and instant setup
    const State state = {
        Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 7546.053290, 0.0}, gcrfSPtr_)};

    // Setup Propagator
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential));

    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);

    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<AtmosphericDrag>(AtmosphericDrag(earthSPtr, satelliteSystem_)),
        std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthSPtr)),
        };
    const Propagator propagator = {defaultNumericalSolver_, dynamics};

    const State postBurnState = propagator.calculateStateAt(state, state.getInstant() + Duration::Seconds(60.0 * 60.0));

    VectorXd expectedCoordinates(6);
    expectedCoordinates << -5172889.585695211000, -4716058.453899897000, 0000000.000008572841, 05083.947538266920,
        -05576.414764013522, -00000.000000010687;

    const VectorXd residuals = postBurnState.getCoordinates() - expectedCoordinates;
    ASSERT_TRUE((residuals.array() < 1e-6).all()) << String::Format("Residual: {}", residuals.maxCoeff());
}

// TBI: Add once we have thrust
// TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, ConstantThrustManeuverWithDrag)
// {
//     using ostk::astro::flight::system::dynamics::ThrusterDynamics;
//     using ostk::astro::flight::system::dynamics::AtmosphericDrag;
//     using ostk::math::obj::VectorXd;
//     using ostk::physics::data::Direction;
//     using ostk::physics::environment::atmospheric::earth::Exponential;
//     using ostk::astro::flight::system::SatelliteSystem;
//     using ostk::astro::flight::system::PropulsionModel;

//     // Current state and instant setup
//     const State state = {
//         Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC),
//         Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
//         Velocity::MetersPerSecond({0.0, 7546.053290, 0.0}, gcrfSPtr_),
//         Mass::Kilograms(10.0)};

//     // Satellite shape does not matter for drag, since constant Cd & area are defined
//     const Composite satelliteGeometry(Cuboid(
//         {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
//         {1.0, 2.0, 3.0}
//     ));

//     // Propulsion Model
//     const PropulsionModel propulsionModel = PropulsionModel(1.0, 1500.0);

//     // Satellite System
//     // Cd = 2.1
//     // Area = 1.0
//     const SatelliteSystem satelliteSystem = {
//         Mass::Kilograms(90.0),
//         satelliteGeometry,
//         Matrix3d::Identity(),
//         1.0,
//         2.1,
//         propulsionModel};  // TBI: use dry mass from satellite system in thruster dynamics

//     // Setup Propagator
//     earthSpherical_->accessAtmosphericModel() = std::make_shared<Exponential>(Exponential());
//     defaultDynamics_.add(std::make_shared<AtmosphericDrag>(AtmosphericDrag(earth, satelliteSystem)));
//     defaultDynamics_.add(
//         std::make_shared<ThrusterDynamics>(ThrusterDynamics(Direction({1.0, 0.0, 0.0}, gcrfSPtr_), satelliteSystem))
//     );  // TBI: Set VNC frame here
//     const Propagator propagator = {defaultDynamics_, defaultNumericalSolver_};

//     const State postBurnState = propagator.calculateStateAt(state, state.getInstant() + Duration::Seconds(60.0
//     * 60.0));

//     VectorXd expectedCoordinates(7);
//     expectedCoordinates << -5320761.901959980000, -4668633.969927717000, 0000000.000006977032, 04920.638509410778,
//         -05657.638329841361, -00000.000000008794, 99.755268109;

//     VectorXd residuals = postBurnState.getCoordinates() - expectedCoordinates;
//     ASSERT_TRUE((residuals.array() < 1e-6).all()) << String::Format("Residual: {}", residuals.maxCoeff());
// }

// /* VALIDATION TESTS */
// /* Force model validation tests */
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody)
{
    // Two body vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/GMAT_TwoBody_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Propagate all states
        const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(2e-3, positionError_GCRF);
            ASSERT_GT(2e-6, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // Two body vs STK
    {
        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/STK_TwoBody_2hr_run.csv"
            )),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referencePositionArray_ITRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_ITRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC));

            referencePositionArray_GCRF.add(
                {referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()}
            );
            referenceVelocityArray_GCRF.add(
                {referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()}
            );

            referencePositionArray_ITRF.add(
                {referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal()}
            );
            referenceVelocityArray_ITRF.add(
                {referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal()}
            );
        }

        const Shared<const Frame> itrfSPtr = Frame::ITRF();

        // Current state and instant setup
        const Instant startInstant = instantArray[0];

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Propagate all states
        const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(2e-7, positionError_GCRF);
            ASSERT_GT(2e-10, velocityError_GCRF);

            // ITRF Compare
            const Position position_ITRF = (propagatedStateArray[i].inFrame(itrfSPtr)).accessPosition();
            const Velocity velocity_ITRF = (propagatedStateArray[i].inFrame(itrfSPtr)).accessVelocity();

            const double positionError_ITRF =
                (position_ITRF.accessCoordinates() - referencePositionArray_ITRF[i]).norm();
            const double velocityError_ITRF =
                (velocity_ITRF.accessCoordinates() - referenceVelocityArray_ITRF[i]).norm();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(2e-2, positionError_ITRF);
            ASSERT_GT(2e-5, velocityError_ITRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "ITRF Position error is: " << positionError_ITRF << "m" << std::endl;
            // std::cout << "ITRF Velocity error is: " << velocityError_ITRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_EGM2008)
{
    // EGM2008 100x100 perturbation only vs STK EGM2008
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_EGM2008_100x100_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Create dynamics
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM2008(100, 100));

        // Setup Propagator
        const Propagator propagator = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth))}};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(1.5e-1, positionError_GCRF);
            ASSERT_GT(1.5e-4, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // EGM2008 360x360, 100x100, 70x70, 45x45 perturbation only self comparison
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_EGM2008_100x100_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Create dynamics
        const Shared<Celestial> earth_360 = std::make_shared<Celestial>(Earth::EGM2008(360, 360));
        const Shared<Celestial> earth_100 = std::make_shared<Celestial>(Earth::EGM2008(100, 100));
        const Shared<Celestial> earth_70 = std::make_shared<Celestial>(Earth::EGM2008(70, 70));
        const Shared<Celestial> earth_45 = std::make_shared<Celestial>(Earth::EGM2008(45, 45));

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagator
        const Propagator propagator_360 = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth_360))}};
        const Propagator propagator_100 = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth_100))}};
        const Propagator propagator_70 = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth_70))}};
        const Propagator propagator_45 = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth_45))}};

        // Propagate all states
        const Array<State> propagatedStateArray_360 = propagator_360.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_100 = propagator_100.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_70 = propagator_70.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_45 = propagator_45.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF_360 = propagatedStateArray_360[i].accessPosition();
            const Velocity velocity_GCRF_360 = propagatedStateArray_360[i].accessVelocity();
            const Position position_GCRF_100 = propagatedStateArray_100[i].accessPosition();
            const Velocity velocity_GCRF_100 = propagatedStateArray_100[i].accessVelocity();
            const Position position_GCRF_70 = propagatedStateArray_70[i].accessPosition();
            const Velocity velocity_GCRF_70 = propagatedStateArray_70[i].accessVelocity();
            const Position position_GCRF_45 = propagatedStateArray_45[i].accessPosition();
            const Velocity velocity_GCRF_45 = propagatedStateArray_45[i].accessVelocity();

            // 360 vs 100
            const double positionError_GCRF_360_100 =
                (position_GCRF_360.accessCoordinates() - position_GCRF_100.accessCoordinates()).norm();
            const double velocityError_GCRF_360_100 =
                (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_100.accessCoordinates()).norm();
            ASSERT_GT(2e-3, positionError_GCRF_360_100);
            ASSERT_GT(2e-6, velocityError_GCRF_360_100);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 100 Position error is: " << positionError_GCRF_360_100 << "m" << std::endl;
            // std::cout << "360 vs 100 Velocity error is: " << velocityError_GCRF_360_100 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 70
            const double positionError_GCRF_360_70 =
                (position_GCRF_360.accessCoordinates() - position_GCRF_70.accessCoordinates()).norm();
            const double velocityError_GCRF_360_70 =
                (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_70.accessCoordinates()).norm();
            ASSERT_GT(5e-2, positionError_GCRF_360_70);
            ASSERT_GT(5e-5, velocityError_GCRF_360_70);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 70 Position error is: " << positionError_GCRF_360_70 << "m" << std::endl;
            // std::cout << "360 vs 70 Velocity error is: " << velocityError_GCRF_360_70 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 45
            const double positionError_GCRF_360_45 =
                (position_GCRF_360.accessCoordinates() - position_GCRF_45.accessCoordinates()).norm();
            const double velocityError_GCRF_360_45 =
                (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_45.accessCoordinates()).norm();
            ASSERT_GT(5e-1, positionError_GCRF_360_45);
            ASSERT_GT(5e-4, velocityError_GCRF_360_45);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 45 Position error is: " << positionError_GCRF_360_45 << "m" << std::endl;
            // std::cout << "360 vs 45 Velocity error is: " << velocityError_GCRF_360_45 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_EGM96)
{
    // EGM96 360x360 perturbation only vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/GMAT_EGM96_360x360_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Create dynamics
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM96(360, 360));

        // Setup Propagator
        const Propagator propagator = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth))}};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(9e-2, positionError_GCRF);
            ASSERT_GT(9e-5, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // EGM96 70x70 perturbation only vs STK WGS84EGM96
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_WGS84EGM96_70x70_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Create dynamics
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM96(70, 70));

        // Setup Propagator
        const Propagator propagator = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth))}};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(1.5e-4, positionError_GCRF);
            ASSERT_GT(1.5e-7, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // EGM96 180x180, 90x90, 45x45 perturbation only self comparison
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/GMAT_EGM96_360x360_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Create dynamics
        const Shared<Celestial> earth_360 = std::make_shared<Celestial>(Earth::EGM96(360, 360));
        const Shared<Celestial> earth_180 = std::make_shared<Celestial>(Earth::EGM96(180, 180));
        const Shared<Celestial> earth_90 = std::make_shared<Celestial>(Earth::EGM96(90, 90));
        const Shared<Celestial> earth_45 = std::make_shared<Celestial>(Earth::EGM96(45, 45));

        // Setup Propagator
        const Propagator propagator_360 = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth_360))}};
        const Propagator propagator_180 = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth_180))}};
        const Propagator propagator_90 = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth_90))}};
        const Propagator propagator_45 = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth_45))}};

        // Propagate all states
        const Array<State> propagatedStateArray_360 = propagator_360.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_180 = propagator_180.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_90 = propagator_90.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_45 = propagator_45.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF_360 = propagatedStateArray_360[i].accessPosition();
            const Velocity velocity_GCRF_360 = propagatedStateArray_360[i].accessVelocity();
            const Position position_GCRF_180 = propagatedStateArray_180[i].accessPosition();
            const Velocity velocity_GCRF_180 = propagatedStateArray_180[i].accessVelocity();
            const Position position_GCRF_90 = propagatedStateArray_90[i].accessPosition();
            const Velocity velocity_GCRF_90 = propagatedStateArray_90[i].accessVelocity();
            const Position position_GCRF_45 = propagatedStateArray_45[i].accessPosition();
            const Velocity velocity_GCRF_45 = propagatedStateArray_45[i].accessVelocity();

            // 360 vs 180
            const double positionError_GCRF_360_180 =
                (position_GCRF_360.accessCoordinates() - position_GCRF_180.accessCoordinates()).norm();
            const double velocityError_GCRF_360_180 =
                (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_180.accessCoordinates()).norm();
            ASSERT_GT(9e-7, positionError_GCRF_360_180);
            ASSERT_GT(9e-10, velocityError_GCRF_360_180);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 180 Position error is: " << positionError_GCRF_360_180 << "m" << std::endl;
            // std::cout << "360 vs 180 Velocity error is: " << velocityError_GCRF_360_180 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 90
            const double positionError_GCRF_360_90 =
                (position_GCRF_360.accessCoordinates() - position_GCRF_90.accessCoordinates()).norm();
            const double velocityError_GCRF_360_90 =
                (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_90.accessCoordinates()).norm();
            ASSERT_GT(7e-3, positionError_GCRF_360_90);
            ASSERT_GT(7e-6, velocityError_GCRF_360_90);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 90 Position error is: " << positionError_GCRF_360_90 << "m" << std::endl;
            // std::cout << "360 vs 90 Velocity error is: " << velocityError_GCRF_360_90 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 45
            const double positionError_GCRF_360_45 =
                (position_GCRF_360.accessCoordinates() - position_GCRF_45.accessCoordinates()).norm();
            const double velocityError_GCRF_360_45 =
                (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_45.accessCoordinates()).norm();
            ASSERT_GT(5e-1, positionError_GCRF_360_45);
            ASSERT_GT(5e-4, velocityError_GCRF_360_45);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 45 Position error is: " << positionError_GCRF_360_45 << "m" << std::endl;
            // std::cout << "360 vs 45 Velocity error is: " << velocityError_GCRF_360_45 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_EGM84)
{
    // EGM84 70x70 perturbation only vs STK EGM84
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_WGS84_70x70_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagator
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM84(70, 70));
        const Propagator propagator = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth))}};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(7e-5, positionError_GCRF);
            ASSERT_GT(7e-8, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // EGM84 180x180, 70x70, 45x45 perturbation only self comparison
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_WGS84_70x70_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Create dynamics
        const Shared<Celestial> earth_180 = std::make_shared<Celestial>(Earth::EGM84(180, 180));
        const Shared<Celestial> earth_70 = std::make_shared<Celestial>(Earth::EGM84(70, 70));
        const Shared<Celestial> earth_45 = std::make_shared<Celestial>(Earth::EGM84(45, 45));

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagator
        const Propagator propagator_180 = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth_180))}};
        const Propagator propagator_70 = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth_70))}};
        const Propagator propagator_45 = {
            defaultNumericalSolver_, {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth_45))}};

        // Propagate all states
        const Array<State> propagatedStateArray_180 = propagator_180.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_70 = propagator_70.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_45 = propagator_45.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF_180 = propagatedStateArray_180[i].accessPosition();
            const Velocity velocity_GCRF_180 = propagatedStateArray_180[i].accessVelocity();
            const Position position_GCRF_70 = propagatedStateArray_70[i].accessPosition();
            const Velocity velocity_GCRF_70 = propagatedStateArray_70[i].accessVelocity();
            const Position position_GCRF_45 = propagatedStateArray_45[i].accessPosition();
            const Velocity velocity_GCRF_45 = propagatedStateArray_45[i].accessVelocity();

            // 180 vs 70
            const double positionError_GCRF_180_70 =
                (position_GCRF_180.accessCoordinates() - position_GCRF_70.accessCoordinates()).norm();
            const double velocityError_GCRF_180_70 =
                (velocity_GCRF_180.accessCoordinates() - velocity_GCRF_70.accessCoordinates()).norm();
            ASSERT_GT(6e-2, positionError_GCRF_180_70);
            ASSERT_GT(6e-5, velocityError_GCRF_180_70);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "180 vs 70 Position error is: " << positionError_GCRF_180_70 << "m" << std::endl;
            // std::cout << "180 vs 70 Velocity error is: " << velocityError_GCRF_180_70 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 180 vs 45
            const double positionError_GCRF_180_45 =
                (position_GCRF_180.accessCoordinates() - position_GCRF_45.accessCoordinates()).norm();
            const double velocityError_GCRF_180_45 =
                (velocity_GCRF_180.accessCoordinates() - velocity_GCRF_45.accessCoordinates()).norm();
            ASSERT_GT(3e-1, positionError_GCRF_180_45);
            ASSERT_GT(3e-4, velocityError_GCRF_180_45);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "180 vs 45 Position error is: " << positionError_GCRF_180_45 << "m" << std::endl;
            // std::cout << "180 vs 45 Velocity error is: " << velocityError_GCRF_180_45 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_Sun_Moon)
{
    // Sun+Moon perturbation only vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/GMAT_TwoBody_Sun_Moon_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Create dynamics
        const Shared<Celestial> moonSpherical_ = std::make_shared<Celestial>(Moon::Spherical());
        const Shared<Celestial> sunSpherical_ = std::make_shared<Celestial>(Sun::Spherical());
        defaultDynamics_ = {
            std::make_shared<CentralBodyGravity>(earthSpherical_),
            std::make_shared<ThirdBodyGravity>(moonSpherical_),
            std::make_shared<ThirdBodyGravity>(sunSpherical_)};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        const Propagator propagator = {defaultNumericalSolver_, defaultDynamics_};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(2e-3, positionError_GCRF);
            ASSERT_GT(2e-6, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_Sun)
{
    // Sun perturbation only vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/GMAT_TwoBody_Sun_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // dynamics setup
        const Shared<Celestial> sunSpherical = std::make_shared<Celestial>(Sun::Spherical());
        defaultDynamics_ = {
            std::make_shared<CentralBodyGravity>(earthSpherical_),
            std::make_shared<ThirdBodyGravity>(sunSpherical)};

        // Setup Propagator
        const Propagator propagator = {defaultNumericalSolver_, defaultDynamics_};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(2e-3, positionError_GCRF);
            ASSERT_GT(2e-6, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_Moon)
{
    // Moon perturbation only vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/GMAT_TwoBody_Moon_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // dynamics setup
        const Shared<Celestial> moonSpherical = std::make_shared<Celestial>(Moon::Spherical());
        defaultDynamics_ = {
            std::make_shared<CentralBodyGravity>(earthSpherical_),
            std::make_shared<ThirdBodyGravity>(moonSpherical),
        };

        // Setup Propagator
        const Propagator propagator = {defaultNumericalSolver_, defaultDynamics_};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(2e-3, positionError_GCRF);
            ASSERT_GT(2e-6, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_Drag_Constant_Exponential_500km)
{
    // Earth with Exponential atmospheric drag compared against OREKit
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2023-01-01 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
                                   "Orekit_Drag_Exponential_500km_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(Instant::DateTime(
                DateTime::Parse(referenceRow[0].accessString(), DateTime::Format::ISO8601), Scale::UTC
            ));

            referencePositionArray_GCRF.add(
                Vector3d(referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                Vector3d(referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal())
            );
        }

        // Setup dynamics
        const Earth earth = Earth::FromModels(
            std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
            std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
            std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
        );
        const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthSPtr)),
            std::make_shared<AtmosphericDrag>(AtmosphericDrag(earthSPtr, satelliteSystem_)),
        };

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagator model and orbit
        const Propagator propagator = {defaultNumericalSolver_, dynamics};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(2e-4, positionError_GCRF);
            ASSERT_GT(2e-6, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_Drag_Constant_Exponential_320km)
{
    // Earth with Exponential atmospheric drag compared against OREKit
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2023-01-01 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
                                   "Orekit_Drag_Exponential_320km_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(Instant::DateTime(
                DateTime::Parse(referenceRow[0].accessString(), DateTime::Format::ISO8601), Scale::UTC
            ));

            referencePositionArray_GCRF.add(
                Vector3d(referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                Vector3d(referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal())
            );
        }

        // Setup dynamics
        const Earth earth = Earth::FromModels(
            std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
            std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
            std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
        );
        const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthSPtr)),
            std::make_shared<AtmosphericDrag>(AtmosphericDrag(earthSPtr, satelliteSystem_)),
        };

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagator model and orbit
        const Propagator propagator = {defaultNumericalSolver_, dynamics};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(2e-4, positionError_GCRF);
            ASSERT_GT(2e-6, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

/* Propagation Interval validation test */
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_IntervalSelfComparison)
{
    // Test that calculateStatesAt returns the same answer if states are requested at short or long intervals
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        const size_t propagationSpanHours = 24;
        const size_t propagationIntervalSeconds = 30;

        Array<Instant> instantArray = Array<Instant>::Empty();
        for (size_t i = 0; i < (propagationSpanHours * 3600) / propagationIntervalSeconds;
             i += propagationIntervalSeconds)
        {
            instantArray.add(startInstant + Duration::Seconds(i));
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};

        // Propagate all states at short interval
        const Array<State> propagatedStateArray_short = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Propagate all states at long interval
        Array<Instant> instantArray_longInterval = Array<Instant>::Empty();
        const size_t indexIncrement = 10;
        for (size_t i = 0; i < instantArray.getSize(); i += indexIncrement)
        {
            instantArray_longInterval.add(instantArray[i]);
        }
        const Array<State> propagatedStateArray_long =
            defaultPropagator_.calculateStatesAt(state, instantArray_longInterval);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // Run once every X times
            if (i % indexIncrement == 0)
            {
                // GCRF Compare
                const Position position_short = propagatedStateArray_short[i].accessPosition();
                const Velocity velocity_short = propagatedStateArray_short[i].accessVelocity();
                const Position position_long = propagatedStateArray_long[i / indexIncrement].accessPosition();
                const Velocity velocity_long = propagatedStateArray_long[i / indexIncrement].accessVelocity();

                const double positionError_GCRF =
                    (position_short.accessCoordinates() - position_long.accessCoordinates()).norm();
                const double velocityError_GCRF =
                    (velocity_short.accessCoordinates() - velocity_long.accessCoordinates()).norm();

                ASSERT_EQ(*Frame::GCRF(), *position_short.accessFrame());
                ASSERT_EQ(*Frame::GCRF(), *velocity_short.accessFrame());
                ASSERT_EQ(*Frame::GCRF(), *position_long.accessFrame());
                ASSERT_EQ(*Frame::GCRF(), *velocity_long.accessFrame());

                ASSERT_GT(1.0e-7, positionError_GCRF);
                ASSERT_GT(1.0e-7, velocityError_GCRF);

                // Results console output

                // std::cout << "**************************************" << std::endl;
                // std::cout.setf(std::ios::scientific,std::ios::floatfield);
                // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
                // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
                // std::cout.setf(std::ios::fixed,std::ios::floatfield);
                // std::cout << "**************************************" << std::endl;
            }
        }
    }

    // Test that calculateStateAt returns the almost the same answer if states are requested at short or long intervals
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        const size_t propagationSpanHours = 24;
        const size_t propagationIntervalSeconds = 30;

        Array<Instant> instantArray = Array<Instant>::Empty();
        for (size_t i = 0; i < (propagationSpanHours * 3600) / propagationIntervalSeconds;
             i += propagationIntervalSeconds)
        {
            instantArray.add(startInstant + Duration::Seconds(i));
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};

        testing::internal::CaptureStdout();

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // Propagator output generated
            const State state_short = defaultPropagator_.calculateStateAt(state, instantArray[i]);

            // Run once every X times
            if (i % 10 == 0)
            {
                const State state_long = defaultPropagator_.calculateStateAt(state, instantArray[i]);

                // GCRF Compare
                const Position position_short = state_short.accessPosition();
                const Velocity velocity_short = state_short.accessVelocity();
                const Position position_long = state_long.accessPosition();
                const Velocity velocity_long = state_long.accessVelocity();

                const double positionError_GCRF =
                    (position_short.accessCoordinates() - position_long.accessCoordinates()).norm();
                const double velocityError_GCRF =
                    (velocity_short.accessCoordinates() - velocity_long.accessCoordinates()).norm();

                ASSERT_GT(3e-8, positionError_GCRF);
                ASSERT_GT(3e-11, velocityError_GCRF);

                // Results console output

                // std::cout << "**************************************" << std::endl;
                // std::cout.setf(std::ios::scientific,std::ios::floatfield);
                // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
                // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
                // std::cout.setf(std::ios::fixed,std::ios::floatfield);
                // std::cout << "**************************************" << std::endl;
            }
        }

        EXPECT_TRUE(testing::internal::GetCapturedStdout().empty());
    }
}

/* Stepper validation test */
TEST_F(
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_StepperTypeSelfComparison
)
{
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        const size_t propagationSpanHours = 24;
        const size_t propagationIntervalSeconds = 30;

        Array<Instant> instantArray = Array<Instant>::Empty();
        for (size_t i = 0; i < (propagationSpanHours * 3600) / propagationIntervalSeconds;
             i += propagationIntervalSeconds)
        {
            instantArray.add(startInstant + Duration::Seconds(i));
        }

        // Construct default numerical solver
        const NumericalSolver numericalSolver_54 = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};

        // Setup Propagator
        const Propagator propagator_54 = {numericalSolver_54, defaultDynamics_};

        // Propagate all states
        const Array<State> propagatedStateArray_54 = propagator_54.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_78 = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_54 = propagatedStateArray_54[i].accessPosition();
            const Velocity velocity_54 = propagatedStateArray_54[i].accessVelocity();
            const Position position_78 = propagatedStateArray_78[i].accessPosition();
            const Velocity velocity_78 = propagatedStateArray_78[i].accessVelocity();

            const double positionError_GCRF =
                (position_54.accessCoordinates() - position_78.accessCoordinates()).norm();
            const double velocityError_GCRF =
                (velocity_54.accessCoordinates() - velocity_78.accessCoordinates()).norm();

            ASSERT_GT(5e-7, positionError_GCRF);
            ASSERT_GT(5e-10, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}
