/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Directory.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Path.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Magnetic/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Table;
using ostk::core::container::Tuple;
using ostk::core::filesystem::Directory;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
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
using ostk::physics::Environment;
using ostk::physics::environment::Object;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::environment::object::celestial::Moon;
using ostk::physics::environment::object::celestial::Sun;
using ostk::physics::environment::object::celestial::Earth;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::ThirdBodyGravity;
using ostk::astrodynamics::dynamics::AtmosphericDrag;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::eventcondition::InstantCondition;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::state::NumericalSolver;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

/* UNIT TESTS */
class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator : public ::testing::Test
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

    const NumericalSolver defaultRKD5_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        5.0,
        1.0e-15,
        1.0e-15,
    };

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    Array<Shared<Dynamics>> defaultDynamics_ = Array<Shared<Dynamics>>::Empty();
    const Mass satelliteDryMass_ = Mass(100.0, Mass::Unit::Kilogram);
    const Mass propellantMass_ = Mass(15.0, Mass::Unit::Kilogram);
    const Mass satelliteWetMass_ =
        Mass(satelliteDryMass_.inKilograms() + propellantMass_.inKilograms(), Mass::Unit::Kilogram);
    PropulsionSystem propulsionSystem_ = PropulsionSystem::Undefined();
    Composite satelliteGeometry_ = Composite::Undefined();
    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();
    Shared<Celestial> earthSpherical_ = nullptr;
    Propagator defaultPropagator_ = Propagator::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, Constructor)
{
    {
        EXPECT_NO_THROW(Propagator(defaultNumericalSolver_, defaultDynamics_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, EqualToOperator)
{
    {
        const Propagator propagator_x = {defaultPropagator_};
        EXPECT_TRUE(defaultPropagator_ == propagator_x);

        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15,
        };
        const Propagator propagator_1 = {numericalSolver_1, defaultDynamics_};
        EXPECT_FALSE(defaultPropagator_ == propagator_1);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, NotEqualToOperator)
{
    {
        const Propagator propagatorX = {defaultPropagator_};
        EXPECT_FALSE(defaultPropagator_ != propagatorX);

        const NumericalSolver numericalSolver1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15,
        };
        const Propagator propagator1 = {numericalSolver1, defaultDynamics_};
        EXPECT_TRUE(defaultPropagator_ != propagator1);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, IsDefined)
{
    {
        EXPECT_TRUE(defaultPropagator_.isDefined());
    }

    {
        const Propagator propagatorWithEmptyDynamicsArray = {defaultNumericalSolver_};
        EXPECT_FALSE(propagatorWithEmptyDynamicsArray.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, Undefined)
{
    {
        EXPECT_NO_THROW(Propagator::Undefined());

        EXPECT_FALSE(Propagator::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultPropagator_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultPropagator_.print(std::cout, true));
        EXPECT_NO_THROW(defaultPropagator_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, AccessCoordinateBroker)
{
    {
        EXPECT_TRUE(defaultPropagator_.accessCoordinateBroker() != nullptr);
    }

    {
        EXPECT_ANY_THROW(Propagator::Undefined().accessCoordinateBroker());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, AccessNumericalSolver)
{
    {
        EXPECT_NO_THROW(defaultPropagator_.accessNumericalSolver());
    }

    {
        EXPECT_ANY_THROW(Propagator::Undefined().accessNumericalSolver());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, Getters)
{
    {
        EXPECT_TRUE(defaultPropagator_.getNumberOfCoordinates() == 6);
    }

    {
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 2);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, SetDynamics)
{
    {
        EXPECT_TRUE(defaultPropagator_.getNumberOfCoordinates() == 6);
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 2);

        const Shared<Dynamics> centralBodyGravity = std::make_shared<CentralBodyGravity>(earthSpherical_);

        defaultPropagator_.setDynamics({centralBodyGravity});

        EXPECT_TRUE(defaultPropagator_.getNumberOfCoordinates() == 6);
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 1);

        defaultPropagator_.setDynamics({});

        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 0);
        EXPECT_FALSE(defaultPropagator_.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, AddDynamics)
{
    {
        EXPECT_TRUE(defaultPropagator_.getNumberOfCoordinates() == 6);
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 2);

        const Shared<Dynamics> centralBodyGravity = std::make_shared<CentralBodyGravity>(earthSpherical_);

        defaultPropagator_.addDynamics(centralBodyGravity);

        EXPECT_TRUE(defaultPropagator_.getNumberOfCoordinates() == 6);
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 3);

        defaultPropagator_.addDynamics(centralBodyGravity);

        EXPECT_TRUE(defaultPropagator_.getNumberOfCoordinates() == 6);
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 4);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, ClearDynamics)
{
    {
        EXPECT_TRUE(defaultPropagator_.getNumberOfCoordinates() == 6);
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() >= 1);

        defaultPropagator_.clearDynamics();

        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, CalculateStateAt)
{
    // Current state and instant setup
    const State state = {
        Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
    };

    // Setup instants
    const Array<Instant> instantArray = {
        Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC),
    };

    for (const Instant& instant : instantArray)
    {
        EXPECT_NO_THROW(defaultPropagator_.calculateStateAt(state, instant));
    }

    {
        // Test that the propagator respects the input Frame
        const State outputStateGCRF = defaultPropagator_.calculateStateAt(state, instantArray[0]);
        const State outputStateITRF =
            defaultPropagator_.calculateStateAt(state.inFrame(Frame::ITRF()), instantArray[0]);

        EXPECT_EQ(outputStateGCRF.getFrame(), Frame::GCRF());
        EXPECT_EQ(outputStateITRF.getFrame(), Frame::ITRF());

        const State outputStateGCRF2 = outputStateITRF.inFrame(Frame::GCRF());

        EXPECT_EQ(outputStateGCRF.getInstant(), outputStateGCRF2.getInstant());
        EXPECT_EQ(outputStateGCRF.getFrame(), outputStateGCRF2.getFrame());
        EXPECT_EQ(outputStateGCRF.accessCoordinateBroker(), outputStateGCRF2.accessCoordinateBroker());

        // Get the coordinates from the two states
        const VectorXd coordinates1 = outputStateGCRF.getCoordinates();
        const VectorXd coordinates2 = outputStateGCRF2.getCoordinates();

        // Check that the two vectors have the same size
        EXPECT_EQ(coordinates1.size(), coordinates2.size());

        // Check that each element of the two vectors is close
        for (int i = 0; i < coordinates1.size(); ++i)
        {
            EXPECT_NEAR(coordinates1(i), coordinates2(i), 1e-5);
        }
    }

    {
        // Test that the propagator respects the input State dimensions

        VectorXd coords(8);
        coords << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, 1, 2;

        const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            std::make_shared<CoordinateSubset>(CoordinateSubset("extra1", 1)),
            std::make_shared<CoordinateSubset>(CoordinateSubset("extra2", 1)),
        };

        const State bigState = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            coords,
            gcrfSPtr_,
            coordinateSubsets,
        };

        // Confirm the propagator only needs 6 dimensions
        EXPECT_EQ(defaultPropagator_.getNumberOfCoordinates(), 6);
        EXPECT_EQ(bigState.getCoordinates().size(), 8);

        const State outputState = defaultPropagator_.calculateStateAt(bigState, instantArray[0]);

        EXPECT_NE(bigState, outputState);
        EXPECT_EQ(bigState.getCoordinateSubsets(), outputState.getCoordinateSubsets());
        EXPECT_EQ(outputState.getCoordinates().size(), 8);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, CalculateStateAt_Condition)
{
    {
        // Current state and instant setup
        const State state = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

        // Setup instants
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC);

        const InstantCondition condition = {
            InstantCondition::Criterion::StrictlyPositive,
            state.accessInstant() + Duration::Seconds(60.0),
        };

        const Propagator propagator = {defaultRKD5_, defaultDynamics_};

        const NumericalSolver::ConditionSolution conditionSolution =
            propagator.calculateStateToCondition(state, endInstant, condition);

        const State endState = conditionSolution.state;

        EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
        EXPECT_TRUE(endState.accessInstant() < endInstant);
        EXPECT_LT((endState.accessInstant() - condition.getInstant()).inSeconds(), 1e-7);

        const InstantCondition failureCondition = {
            InstantCondition::Criterion::StrictlyPositive,
            state.accessInstant() + Duration::Seconds(7000.0),
        };

        EXPECT_FALSE(propagator.calculateStateToCondition(state, endInstant, failureCondition).conditionIsSatisfied);
    }

    {
        // Current state and instant setup
        const State state = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

        // Setup instants
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC);

        const InstantCondition condition = {
            InstantCondition::Criterion::StrictlyPositive,
            state.accessInstant() + Duration::Seconds(60.0),
        };

        const Propagator propagator = {defaultRKD5_, defaultDynamics_};

        const NumericalSolver::ConditionSolution conditionSolutionGCRF =
            propagator.calculateStateToCondition(state, endInstant, condition);

        const NumericalSolver::ConditionSolution conditionSolutionITRF =
            propagator.calculateStateToCondition(state.inFrame(Frame::ITRF()), endInstant, condition);

        // Test that the propagator respects the input Frame
        const State outputStateGCRF = conditionSolutionGCRF.state;
        const State outputStateITRF = conditionSolutionITRF.state;

        EXPECT_EQ(outputStateGCRF.getFrame(), Frame::GCRF());
        EXPECT_EQ(outputStateITRF.getFrame(), Frame::ITRF());

        const State outputStateGCRF2 = outputStateITRF.inFrame(Frame::GCRF());

        EXPECT_EQ(outputStateGCRF.getFrame(), outputStateGCRF2.getFrame());
        EXPECT_EQ(outputStateGCRF.accessCoordinateBroker(), outputStateGCRF2.accessCoordinateBroker());

        // Get the coordinates from the two states
        const VectorXd coordinates1 = outputStateGCRF.getCoordinates();
        const VectorXd coordinates2 = outputStateGCRF2.getCoordinates();

        // Check that the two vectors have the same size
        EXPECT_EQ(coordinates1.size(), coordinates2.size());

        // Check that each element of the two vectors is close
        for (int i = 0; i < coordinates1.size(); ++i)
        {
            EXPECT_NEAR(coordinates1(i), coordinates2(i), 1e-5);
        }
    }

    {
        // Test that the propagator respects the input State dimensions

        VectorXd coords(8);
        coords << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, 1, 2;

        const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            std::make_shared<CoordinateSubset>(CoordinateSubset("extra1", 1)),
            std::make_shared<CoordinateSubset>(CoordinateSubset("extra2", 1)),
        };

        const State bigState = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            coords,
            gcrfSPtr_,
            coordinateSubsets,
        };

        // Setup instants
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC);

        const InstantCondition condition = {
            InstantCondition::Criterion::StrictlyPositive,
            bigState.accessInstant() + Duration::Seconds(60.0),
        };

        const Propagator propagator = {defaultRKD5_, defaultDynamics_};

        // Confirm the propagator only needs 6 dimensions
        EXPECT_EQ(propagator.getNumberOfCoordinates(), 6);
        EXPECT_EQ(bigState.getCoordinates().size(), 8);

        const NumericalSolver::ConditionSolution conditionSolution =
            propagator.calculateStateToCondition(bigState, endInstant, condition);

        const State outputState = conditionSolution.state;

        EXPECT_NE(bigState, outputState);
        EXPECT_EQ(bigState.getCoordinateSubsets(), outputState.getCoordinateSubsets());
        EXPECT_EQ(outputState.getCoordinates().size(), 8);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, CalculateStatesAt)
{
    // Test exception for unsorted instant array
    {
        // Current state and instant setup
        const State state = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

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
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/"
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

        referencePositionArray.add({
            referenceRow[1].accessReal(),
            referenceRow[2].accessReal(),
            referenceRow[3].accessReal(),
        });
        referenceVelocityArray.add({
            referenceRow[4].accessReal(),
            referenceRow[5].accessReal(),
            referenceRow[6].accessReal(),
        });
    }

    // Test forward propagation
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 0;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_),
        };

        const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].getPosition();
            const Velocity velocity = propagatedStateArray[i].getVelocity();

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
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_),
        };

        const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].getPosition();
            const Velocity velocity = propagatedStateArray[i].getVelocity();

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
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_),
        };

        const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].getPosition();
            const Velocity velocity = propagatedStateArray[i].getVelocity();

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

    {
        // Current state and instant setup
        const State state = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

        // Test that the propagator respects the input Frame
        const Array<State> outputStatesGCRF = defaultPropagator_.calculateStatesAt(state, instantArray);
        const Array<State> outputStatesITRF =
            defaultPropagator_.calculateStatesAt(state.inFrame(Frame::ITRF()), instantArray);

        // Validation loop
        for (size_t i = 0; i < outputStatesGCRF.getSize(); i++)
        {
            EXPECT_EQ(outputStatesGCRF[i].getFrame(), Frame::GCRF());
            EXPECT_EQ(outputStatesITRF[i].getFrame(), Frame::ITRF());

            const State outputStateGCRF2 = outputStatesITRF[i].inFrame(Frame::GCRF());

            EXPECT_EQ(outputStatesGCRF[i].getInstant(), outputStateGCRF2.getInstant());
            EXPECT_EQ(outputStatesGCRF[i].getFrame(), outputStateGCRF2.getFrame());
            EXPECT_EQ(outputStatesGCRF[i].accessCoordinateBroker(), outputStateGCRF2.accessCoordinateBroker());

            // Get the coordinates from the two states
            const VectorXd coordinates1 = outputStatesGCRF[i].getCoordinates();
            const VectorXd coordinates2 = outputStateGCRF2.getCoordinates();

            // Check that the two vectors have the same size
            EXPECT_EQ(coordinates1.size(), coordinates2.size());

            // Check that each element of the two vectors is close
            for (int j = 0; j < coordinates1.size(); ++j)
            {
                EXPECT_NEAR(coordinates1(j), coordinates2(j), 1e-3);
            }
        }
    }

    {
        // Test that the propagator respects the input State dimensions

        VectorXd coords(8);
        coords << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, 1, 2;

        const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            std::make_shared<CoordinateSubset>(CoordinateSubset("extra1", 1)),
            std::make_shared<CoordinateSubset>(CoordinateSubset("extra2", 1)),
        };

        const State bigState = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            coords,
            gcrfSPtr_,
            coordinateSubsets,
        };

        // Confirm the propagator only needs 6 dimensions
        EXPECT_EQ(defaultPropagator_.getNumberOfCoordinates(), 6);
        EXPECT_EQ(bigState.getCoordinates().size(), 8);

        const Array<State> outputStates = defaultPropagator_.calculateStatesAt(bigState, instantArray);

        for (size_t i = 0; i < outputStates.getSize(); i++)
        {
            EXPECT_NE(bigState, outputStates[i]);
            EXPECT_EQ(bigState.getCoordinateSubsets(), outputStates[i].getCoordinateSubsets());
            EXPECT_EQ(outputStates[i].getCoordinates().size(), 8);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, Default)
{
    {
        EXPECT_NO_THROW(Propagator::Default());
        EXPECT_TRUE(Propagator::Default().isDefined());
    }

    {
        Environment environment = Environment::Default();
        EXPECT_NO_THROW(Propagator::Default(environment));
        EXPECT_NO_THROW(Propagator::Default(environment).isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, FromEnvironment)
{
    {
        Environment environment = Environment::Default();
        EXPECT_NO_THROW(Propagator::FromEnvironment(defaultNumericalSolver_, environment));

        const Propagator propagator = Propagator::FromEnvironment(defaultNumericalSolver_, environment);

        // default environment has 3 bodies (Earth Sun and Moon) with gravitational fields
        // and 1 default dynamics of integrating the Position Derivative
        EXPECT_TRUE(propagator.getDynamics().getSize() == 4);

        EXPECT_TRUE(static_cast<const CentralBodyGravity&>(*propagator.getDynamics()[0]).isDefined());
        EXPECT_TRUE(static_cast<const ThirdBodyGravity&>(*propagator.getDynamics()[1]).isDefined());
        EXPECT_TRUE(static_cast<const ThirdBodyGravity&>(*propagator.getDynamics()[2]).isDefined());
    }

    {
        const Array<Shared<Object>> celestials = {
            std::make_shared<Earth>(Earth::FromModels(
                std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
                std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
                std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
            )),
            std::make_shared<Moon>(Moon::Default()),
            std::make_shared<Sun>(Sun::Default()),
        };
        Environment environment = Environment(Instant::J2000(), celestials);
        const Propagator propagator = Propagator::FromEnvironment(defaultNumericalSolver_, environment);

        EXPECT_TRUE(propagator.getDynamics().getSize() == 5);

        EXPECT_TRUE(static_cast<const CentralBodyGravity&>(*propagator.getDynamics()[0]).isDefined());
        EXPECT_TRUE(static_cast<const AtmosphericDrag&>(*propagator.getDynamics()[1]).isDefined());
        EXPECT_TRUE(static_cast<const ThirdBodyGravity&>(*propagator.getDynamics()[2]).isDefined());
        EXPECT_TRUE(static_cast<const ThirdBodyGravity&>(*propagator.getDynamics()[3]).isDefined());
        EXPECT_TRUE(static_cast<const PositionDerivative&>(*propagator.getDynamics()[4]).isDefined());
    }
}
