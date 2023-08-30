/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::ctnr::Array;
using ostk::core::types::Index;
using ostk::core::types::Shared;
using ostk::core::types::Size;

using ostk::math::obj::VectorXd;

using ostk::physics::env::obj::Celestial;

using ostk::astro::flight::system::dynamics::PositionDerivative;
using ostk::astro::flight::system::dynamics::CentralBodyGravity;
using ostk::astro::flight::system::dynamics::ThirdBodyGravity;
using ostk::astro::flight::system::dynamics::AtmosphericDrag;
using ostk::astro::trajectory::state::CoordinatesSubset;

static const Shared<const Frame> integrationFrameSPtr = Frame::GCRF();

Propagator::Propagator(const NumericalSolver& aNumericalSolver, const Array<Shared<Dynamics>>& aDynamicsArray)
    : dynamicsContexts_(),
      numericalSolver_(aNumericalSolver)
{
    for (const Shared<Dynamics>& aDynamicsSPtr : aDynamicsArray)
    {
        this->registerDynamicsContext(aDynamicsSPtr);
    }
}

bool Propagator::operator==(const Propagator& aPropagator) const
{
    if ((!this->isDefined()) || (!aPropagator.isDefined()))
    {
        return false;
    }

    return (
        numericalSolver_ == aPropagator.numericalSolver_ &&
        *this->coordinatesBrokerSPtr_ == *(aPropagator.coordinatesBrokerSPtr_)
    );
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
    return this->numericalSolver_.isDefined() && this->coordinatesBrokerSPtr_ != nullptr &&
           !this->dynamicsContexts_.isEmpty();
}

const Shared<CoordinatesBroker>& Propagator::accessCoordinatesBroker() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    return this->coordinatesBrokerSPtr_;
}

const NumericalSolver& Propagator::accessNumericalSolver() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    return this->numericalSolver_;
}

Size Propagator::getNumberOfCoordinates() const
{
    return this->accessCoordinatesBroker()->getNumberOfCoordinates();
}

Array<Shared<Dynamics>> Propagator::getDynamics() const
{
    Array<Shared<Dynamics>> dynamicsArray = Array<Shared<Dynamics>>::Empty();

    for (const Dynamics::Context& dynamicsContext : this->dynamicsContexts_)
    {
        dynamicsArray.add(dynamicsContext.dynamics);
    }

    return dynamicsArray;
}

void Propagator::setDynamics(const Array<Shared<Dynamics>>& aDynamicsArray)
{
    this->clearDynamics();

    for (const Shared<Dynamics>& aDynamicsSPtr : aDynamicsArray)
    {
        this->registerDynamicsContext(aDynamicsSPtr);
    }
}

void Propagator::addDynamics(const Shared<Dynamics>& aDynamicsSPtr)
{
    if (aDynamicsSPtr == nullptr || !aDynamicsSPtr->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Dynamics");
    }

    this->registerDynamicsContext(aDynamicsSPtr);
}

void Propagator::clearDynamics()
{
    this->dynamicsContexts_.clear();
    this->coordinatesBrokerSPtr_ = std::make_shared<CoordinatesBroker>();
}

State Propagator::calculateStateAt(const State& aState, const Instant& anInstant) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    const Instant startInstant = aState.getInstant();

    const NumericalSolver::Solution solution = numericalSolver_.integrateDuration(
        extractCoordinatesFromState(aState),
        (anInstant - startInstant).inSeconds(),
        Dynamics::GetSystemOfEquations(this->dynamicsContexts_, startInstant, integrationFrameSPtr)
    );

    return State(anInstant, solution.first, integrationFrameSPtr, this->coordinatesBrokerSPtr_);
}

State Propagator::calculateStateAt(
    const State& aState, const Instant& anInstant, const AstroCondition& anAstroCondition
) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    const Instant startInstant = aState.getInstant();

    anAstroCondition.setReferenceParameters(
        startInstant,
        integrationFrameSPtr,
        this->coordinatesBrokerSPtr_
    );

    const NumericalSolver::ConditionSolution conditionSolution = numericalSolver_.integrateDuration(
        extractCoordinatesFromState(aState),
        (anInstant - startInstant).inSeconds(),
        Dynamics::GetSystemOfEquations(this->dynamicsContexts_, startInstant, integrationFrameSPtr),
        anEventCondition
    );

    if (!conditionSolution.conditionIsSatisfied)
    {
        throw ostk::core::error::RuntimeError("Condition not satisfied.");
    }

    const NumericalSolver::Solution solution = conditionSolution.solution;

    const NumericalSolver::StateVector endStateVector = solution.first;
    const Instant endInstant = startInstant + Duration::Seconds(solution.second);

    return {
        endInstant,
        endStateVector,
        integrationFrameSPtr,
        this->coordinatesBrokerSPtr_,
    };
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

    const Instant startInstant = aState.accessInstant();

    Array<Real> forwardDurations;
    forwardDurations.reserve(anInstantArray.getSize());
    Array<Real> backwardDurations;
    backwardDurations.reserve(anInstantArray.getSize());

    for (const Instant& anInstant : anInstantArray)
    {
        const Real durationInSeconds = (anInstant - startInstant).inSeconds();

        if (anInstant <= startInstant)
        {
            backwardDurations.add(durationInSeconds);
        }
        else
        {
            forwardDurations.add(durationInSeconds);
        }
    }

    const NumericalSolver::StateVector extractedCoordinates = extractCoordinatesFromState(aState);

    // forward propagation only
    Array<NumericalSolver::Solution> forwardPropagatedSolutions;
    if (!forwardDurations.isEmpty())
    {
        forwardPropagatedSolutions = numericalSolver_.integrateDuration(
            extractedCoordinates,
            forwardDurations,
            Dynamics::GetSystemOfEquations(this->dynamicsContexts_, startInstant, integrationFrameSPtr)
        );
    }

    // backward propagation only
    Array<NumericalSolver::Solution> backwardPropagatedSolutions;
    if (!backwardDurations.isEmpty())
    {
        std::reverse(backwardDurations.begin(), backwardDurations.end());

        backwardPropagatedSolutions = numericalSolver_.integrateDuration(
            extractedCoordinates,
            backwardDurations,
            Dynamics::GetSystemOfEquations(this->dynamicsContexts_, startInstant, integrationFrameSPtr)
        );

        std::reverse(backwardPropagatedSolutions.begin(), backwardPropagatedSolutions.end());
    }

    Array<State> propagatedStates;
    propagatedStates.reserve(anInstantArray.getSize());

    Size k = 0;
    for (const NumericalSolver::Solution& solution : (backwardPropagatedSolutions + forwardPropagatedSolutions))
    {
        propagatedStates.add({
            anInstantArray[k],
            solution.first,
            integrationFrameSPtr,
            this->coordinatesBrokerSPtr_,
        });
        ++k;
    }

    return propagatedStates;
}

void Propagator::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Propagator") : void();

    ostk::core::utils::Print::Separator(anOutputStream, "Numerical Solver");
    numericalSolver_.print(anOutputStream, false);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Propagator Propagator::Undefined()
{
    return {
        NumericalSolver::Undefined(),
        Array<Shared<Dynamics>>::Empty(),
    };
}

Propagator Propagator::Default()
{
    return {
        NumericalSolver::Default(),
        {std::make_shared<PositionDerivative>()},
    };
}

Propagator Propagator::Default(const Environment& anEnvironment, const SatelliteSystem& aSatelliteSystem)
{
    return Propagator::FromEnvironment(NumericalSolver::Default(), anEnvironment, aSatelliteSystem);
}

Propagator Propagator::FromEnvironment(
    const NumericalSolver& aNumericalSolver, const Environment& anEnvironment, const SatelliteSystem& aSatelliteSystem
)
{
    const auto getDynamics = [aSatelliteSystem](const Shared<const Celestial>& aCelestial) -> Array<Shared<Dynamics>>
    {
        Array<Shared<Dynamics>> dynamics = Array<Shared<Dynamics>>::Empty();

        if (aCelestial->gravitationalModelIsDefined())
        {
            if (aCelestial->getName() == "Earth")
            {
                dynamics.add(std::make_shared<CentralBodyGravity>(aCelestial));
            }
            else
            {
                dynamics.add(std::make_shared<ThirdBodyGravity>(aCelestial));
            }
        }

        if (aCelestial->atmosphericModelIsDefined())
        {
            dynamics.add(std::make_shared<AtmosphericDrag>(aCelestial, aSatelliteSystem));
        }

        return dynamics;
    };

    Array<Shared<Dynamics>> dynamicsArray = Array<Shared<Dynamics>>::Empty();

    for (const String& name : anEnvironment.getObjectNames())
    {
        const Shared<const Celestial> celestialSPtr = anEnvironment.accessCelestialObjectWithName(name);

        dynamicsArray.add(getDynamics(celestialSPtr));
    }

    dynamicsArray.add(std::make_shared<PositionDerivative>());

    return {
        aNumericalSolver,
        dynamicsArray,
    };
}

void Propagator::registerDynamicsContext(const Shared<Dynamics>& aDynamicsSPtr)
{
    // Store read coordinate subsets information
    Array<Pair<Index, Size>> readInfo = Array<Pair<Index, Size>>::Empty();
    readInfo.reserve(aDynamicsSPtr->getReadCoordinatesSubsets().getSize());
    for (const Shared<const CoordinatesSubset>& subset : aDynamicsSPtr->getReadCoordinatesSubsets())
    {
        const Pair<Index, Size> indexAndSize = {this->coordinatesBrokerSPtr_->addSubset(subset), subset->getSize()};
        readInfo.add(indexAndSize);
    }

    // Store write coordinate subsets information
    Array<Pair<Index, Size>> writeInfo = Array<Pair<Index, Size>>::Empty();
    writeInfo.reserve(aDynamicsSPtr->getWriteCoordinatesSubsets().getSize());
    for (const Shared<const CoordinatesSubset>& subset : aDynamicsSPtr->getWriteCoordinatesSubsets())
    {
        const Pair<Index, Size> indexAndSize = {this->coordinatesBrokerSPtr_->addSubset(subset), subset->getSize()};
        writeInfo.add(indexAndSize);
    }

    this->dynamicsContexts_.add({aDynamicsSPtr, readInfo, writeInfo});
}

NumericalSolver::StateVector Propagator::extractCoordinatesFromState(const State& aState) const
{
    const State state = aState.inFrame(integrationFrameSPtr);

    Index offset = 0;
    NumericalSolver::StateVector extractedStateVector = NumericalSolver::StateVector(this->getNumberOfCoordinates());

    for (const Shared<const CoordinatesSubset>& subset : this->coordinatesBrokerSPtr_->getSubsets())
    {
        const Size subsetSize = subset->getSize();
        extractedStateVector.segment(offset, subsetSize) = state.extractCoordinates(subset);
        offset += subsetSize;
    }

    return extractedStateVector;
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
