/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Size;
using ostk::core::ctnr::Pair;

static const Shared<const Frame> gcrfSPtr = Frame::GCRF();

Propagator::Propagator(const SatelliteDynamics& aSatelliteDynamics, const NumericalSolver& aNumericalSolver)
    : satelliteDynamics_(aSatelliteDynamics),
      numericalSolver_(aNumericalSolver),
      maneuverArray_(Array<Maneuver>::Empty())
{
}

Propagator::Propagator(const SatelliteDynamics& aSatelliteDynamics, const NumericalSolver& aNumericalSolver, const Array<Maneuver>& aManeuverArray)
    : satelliteDynamics_(aSatelliteDynamics),
      numericalSolver_(aNumericalSolver),
      maneuverArray_(aManeuverArray)
{
}

Propagator* Propagator::clone() const
{
    return new Propagator(*this);
}

bool Propagator::operator==(const Propagator& aPropagator) const
{
    if ((!this->isDefined()) || (!aPropagator.isDefined()))
    {
        return false;
    }

    return (satelliteDynamics_ == aPropagator.satelliteDynamics_) && (numericalSolver_ == aPropagator.numericalSolver_);
}

bool Propagator::operator!=(const Propagator& aPropagator) const
{
    return !((*this) == aPropagator);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Propagator& aPropagator)
{
    aPropagator.print(anOutputStream);

    return anOutputStream;
}

bool Propagator::isDefined() const
{
    return satelliteDynamics_.isDefined() && numericalSolver_.isDefined();
}

State Propagator::calculateStateAt(const State& aState, const Instant& anInstant) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    const VectorXd stateCoordinates = aState.getCoordinates();

    SatelliteDynamics::StateVector startStateVector(
        stateCoordinates.data(), stateCoordinates.data() + stateCoordinates.size()
    );

    satelliteDynamics_.setInstant(aState.getInstant());

    SatelliteDynamics::StateVector endStateVector = numericalSolver_.integrateStateFromInstantToInstant(
        startStateVector, aState.getInstant(), anInstant, satelliteDynamics_.getDynamicalEquations()
    );

    return {
        anInstant,
        Position::Meters({endStateVector[0], endStateVector[1], endStateVector[2]}, gcrfSPtr),
        Velocity::MetersPerSecond({endStateVector[3], endStateVector[4], endStateVector[5]}, gcrfSPtr)};
}

Array<State> Propagator::calculateStatesAt(const State& aState, const Array<Instant>& anInstantArray) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    if (!aState.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    if (anInstantArray.isEmpty())
    {
        return Array<State>::Empty();
    }

    for (Size k = 0; k < anInstantArray.getSize() - 1; ++k)
    {
        if (anInstantArray[k] > anInstantArray[k + 1])
        {
            throw ostk::core::error::runtime::Wrong("Unsorted Instant Array");
        }
    }

    const VectorXd stateCoordinates = aState.getCoordinates();
    SatelliteDynamics::StateVector startStateVector(
        stateCoordinates.data(), stateCoordinates.data() + stateCoordinates.size()
    );

    satelliteDynamics_.setInstant(aState.getInstant());

    Array<Instant> forwardInstants;
    Array<Instant> backwardInstants;

    for (const Instant& anInstant : anInstantArray)
    {
        if (anInstant <= aState.getInstant())
        {
            backwardInstants.add(anInstant);
        }
        else
        {
            forwardInstants.add(anInstant);
        }
    }

    // TBI: Dummy implementation at Propagator level, it should probably not live at this level
    // Check size of this->maneuverArray_ before continuing

    Array<Maneuver> forwardInstantManeuvers;
    Array<Maneuver> backwardInstantManeuvers;

    for (const Maneuver& aManeuver : this->maneuverArray_)
    {
        if (aManeuver.getInstant() <= aState.getInstant())
        {
            backwardInstantManeuvers.add(aManeuver);
        }
        else
        {
            forwardInstantManeuvers.add(aManeuver);
        }
    }

    Array<Pair<Instant, Maneuver>> forwardInstantsNonManeuverPairs = Array<Instant>(forwardInstants.begin(), forwardInstants.end())
        .map<Pair<Instant, Maneuver>>([] (const Instant& anInstant) -> Pair<Instant, Maneuver> { return { anInstant, Maneuver::Undefined() } ; }) ;
    Array<Pair<Instant, Maneuver>> forwardInstantManeuverPairs = Array<Maneuver>(forwardInstantManeuvers.begin(), forwardInstantManeuvers.end())
        .map<Pair<Instant, Maneuver>>([] (const Maneuver& aManeuver) -> Pair<Instant, Maneuver> { return { aManeuver.getInstant(), aManeuver } ; }) ;

    Array<Pair<Instant, Maneuver>> forwardInstantPairs = forwardInstantManeuverPairs + forwardInstantManeuverPairs;

    std::sort(
        forwardInstantPairs.begin(),
        forwardInstantPairs.end(),
        [](const auto& lhs, const auto& rhs)
        {
            return lhs.first < rhs.first;
        }
    );

    // forward propagation only
    Array<SatelliteDynamics::StateVector> propagatedForwardStateVectorArray;
    Instant startInstant = aState.getInstant();
    if (!forwardInstantPairs.isEmpty())
    {

        for (const auto& instantManeuverPair: forwardInstantPairs)
        {

            Instant propagationInstant = instantManeuverPair.first;
            Maneuver maneuver = instantManeuverPair.second;

            SatelliteDynamics::StateVector propagationVector;

            if (!maneuver.isDefined())
            {
                propagationVector = numericalSolver_.integrateStateFromInstantToInstant(
                    startStateVector, startInstant, propagationInstant, satelliteDynamics_.getDynamicalEquations()
                );

                startStateVector = propagationVector;
                startInstant = propagationInstant;

                propagatedForwardStateVectorArray.add(propagationVector);
            }
            else
            {
                Vector3d deltaV = maneuver.getVelocityDelta().getCoordinates();  // Check Frame, link with attitude provider

                propagationVector[3] += deltaV[0];
                propagationVector[4] += deltaV[1];
                propagationVector[5] += deltaV[2];
            }
        }
    }

    // backward propagation only
    Array<SatelliteDynamics::StateVector> propagatedBackwardStateVectorArray;
    if (!backwardInstants.isEmpty())
    {
        std::reverse(backwardInstants.begin(), backwardInstants.end());

        propagatedBackwardStateVectorArray = numericalSolver_.integrateStatesAtSortedInstants(
            startStateVector, aState.getInstant(), backwardInstants, satelliteDynamics_.getDynamicalEquations()
        );

        std::reverse(propagatedBackwardStateVectorArray.begin(), propagatedBackwardStateVectorArray.end());
    }

    Array<State> propagatedStates;
    propagatedStates.reserve(anInstantArray.getSize());

    Size k = 0;
    for (const SatelliteDynamics::StateVector& stateVector :
         (propagatedBackwardStateVectorArray + propagatedForwardStateVectorArray))
    {
        State propagatedState = {
            anInstantArray[k],
            Position::Meters({stateVector[0], stateVector[1], stateVector[2]}, gcrfSPtr),
            Velocity::MetersPerSecond({stateVector[3], stateVector[4], stateVector[5]}, gcrfSPtr)};

        propagatedStates.add(propagatedState);

        ++k;
    }

    return propagatedStates;
}

void Propagator::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Propagator") : void();

    ostk::core::utils::Print::Separator(anOutputStream, "Satellite Dynamics");
    satelliteDynamics_.print(anOutputStream, false);

    ostk::core::utils::Print::Separator(anOutputStream, "Numerical Solver");
    numericalSolver_.print(anOutputStream, false);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Propagator Propagator::MediumFidelity()
{
    using ostk::math::geom::d3::objects::Composite;
    using ostk::math::geom::d3::objects::Cuboid;
    using ostk::math::geom::d3::objects::Point;
    using ostk::math::obj::Matrix3d;

    using ostk::physics::Environment;
    using ostk::physics::env::Object;
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::env::obj::celest::Moon;
    using ostk::physics::env::obj::celest::Sun;
    using ostk::physics::units::Mass;

    using ostk::astro::flight::system::SatelliteSystem;

    // Create environment
    const Array<Shared<Object>> objects =  // [TBI] Add drag to environment once it is added in ostk-physics
        {std::make_shared<Earth>(Earth::EGM2008(35, 35))};

    const Instant instant = Instant::J2000();

    const Environment customEnvironment = Environment(instant, objects);

    // Satellite system setup
    const Composite satelliteGeometry = Composite {Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    )};
    const SatelliteSystem satelliteSystem = {
        Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

    // Satellite dynamics setup
    const SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

    // Construct default numerical solver
    const NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};

    return {satelliteDynamics, numericalSolver};
}

Propagator Propagator::HighFidelity()
{
    using ostk::math::geom::d3::objects::Composite;
    using ostk::math::geom::d3::objects::Cuboid;
    using ostk::math::geom::d3::objects::Point;
    using ostk::math::obj::Matrix3d;

    using ostk::physics::Environment;
    using ostk::physics::env::Object;
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::env::obj::celest::Moon;
    using ostk::physics::env::obj::celest::Sun;
    using ostk::physics::units::Mass;

    using ostk::astro::flight::system::SatelliteSystem;

    // Create environment
    const Array<Shared<Object>> objects =  // [TBI] Add drag to environment once it is added in ostk-physics
        {std::make_shared<Earth>(Earth::EGM2008(100, 100)),
         std::make_shared<Sun>(Sun::Spherical()),
         std::make_shared<Moon>(Moon::Spherical())};

    const Instant instant = Instant::J2000();

    const Environment customEnvironment = Environment(instant, objects);

    // Satellite system setup
    const Composite satelliteGeometry = Composite {Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    )};
    const SatelliteSystem satelliteSystem = {
        Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

    // Satellite dynamics setup
    const SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

    // Construct default numerical solver
    const NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};

    return {satelliteDynamics, numericalSolver};
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
