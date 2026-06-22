/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AngularVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace model
{

using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::AngularVelocity;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianAcceleration;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

Tabulated::Tabulated(const Array<State>& aStateArray, const Interpolator::Type& anInterpolationType)
    : Model()
{
    VectorXd timestamps;
    MatrixXd coordinates;

    if (!this->computeInterpolationData(aStateArray, timestamps, coordinates))
    {
        return;
    }

    interpolators_.reserve(coordinates.cols());

    for (Index i = 0; i < Size(coordinates.cols()); ++i)
    {
        interpolators_.add(Interpolator::GenerateInterpolator(anInterpolationType, timestamps, coordinates.col(i)));
    }
}

Tabulated::Tabulated(
    const Array<State>& aStateArray,
    const Interpolator::Type& anInterpolationType,
    const Shared<const Frame>& aFrameSPtr
)
    : Tabulated(aStateArray, anInterpolationType)
{
    frameSPtr_ = aFrameSPtr;
}

Tabulated::Tabulated(
    const Array<State>& aStateArray,
    const Map<Shared<const CoordinateSubset>, Interpolator::Type>& anInterpolationTypeMap
)
    : Model()
{
    using ostk::core::type::String;

    using ostk::astrodynamics::trajectory::state::CoordinateBroker;

    VectorXd timestamps;
    MatrixXd coordinates;

    if (!this->computeInterpolationData(aStateArray, timestamps, coordinates))
    {
        return;
    }

    const Shared<const CoordinateBroker>& coordinatesBroker = firstState_.accessCoordinateBroker();

    // Index the requested interpolation types by coordinate subset id. Entries for coordinate subsets that are not
    // present in the states are ignored.
    Map<String, Interpolator::Type> interpolationTypeBySubsetId;

    for (const auto& [coordinateSubsetSPtr, interpolationType] : anInterpolationTypeMap)
    {
        interpolationTypeBySubsetId[coordinateSubsetSPtr->getId()] = interpolationType;
    }

    // Resolve the interpolation type for every coordinate, requiring each coordinate subset present in the states to
    // be specified in the provided map.
    Array<Interpolator::Type> interpolationTypePerCoordinate = Array<Interpolator::Type>::Empty();
    interpolationTypePerCoordinate.reserve(coordinates.cols());

    for (const auto& coordinateSubsetSPtr : coordinatesBroker->accessSubsets())
    {
        const auto interpolationTypeIt = interpolationTypeBySubsetId.find(coordinateSubsetSPtr->getId());

        if (interpolationTypeIt == interpolationTypeBySubsetId.end())
        {
            throw ostk::core::error::RuntimeError(String::Format(
                "No interpolation type was provided for the coordinate subset [{}].", coordinateSubsetSPtr->getName()
            ));
        }

        for (Size i = 0; i < coordinateSubsetSPtr->getSize(); ++i)
        {
            interpolationTypePerCoordinate.add(interpolationTypeIt->second);
        }
    }

    interpolators_.reserve(coordinates.cols());

    for (Index i = 0; i < Size(coordinates.cols()); ++i)
    {
        interpolators_.add(
            Interpolator::GenerateInterpolator(interpolationTypePerCoordinate[i], timestamps, coordinates.col(i))
        );
    }
}

Tabulated::Tabulated(
    const Array<State>& aStateArray,
    const Map<Shared<const CoordinateSubset>, Interpolator::Type>& anInterpolationTypeMap,
    const Shared<const Frame>& aFrameSPtr
)
    : Tabulated(aStateArray, anInterpolationTypeMap)
{
    frameSPtr_ = aFrameSPtr;
}

Tabulated* Tabulated::clone() const
{
    return new Tabulated(*this);
}

bool Tabulated::operator==(const Tabulated& aTabulatedModel) const
{
    if ((!this->isDefined()) || (!aTabulatedModel.isDefined()))
    {
        return false;
    }

    if (interpolators_.getSize() != aTabulatedModel.interpolators_.getSize())
    {
        return false;
    }

    // Compare the interpolation type of each coordinate, so that models with per-coordinate-subset interpolation
    // types are correctly distinguished.
    for (Index i = 0; i < interpolators_.getSize(); ++i)
    {
        if (interpolators_[i]->getInterpolationType() != aTabulatedModel.interpolators_[i]->getInterpolationType())
        {
            return false;
        }
    }

    const bool framesEqual = (frameSPtr_ == aTabulatedModel.frameSPtr_) ||
                             ((frameSPtr_ != nullptr) && (aTabulatedModel.frameSPtr_ != nullptr) &&
                              ((*frameSPtr_) == (*aTabulatedModel.frameSPtr_)));

    return framesEqual && firstState_ == aTabulatedModel.getFirstState() &&
           lastState_ == aTabulatedModel.getLastState();
}

bool Tabulated::operator!=(const Tabulated& aTabulatedModel) const
{
    return !((*this) == aTabulatedModel);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Tabulated& aTabulatedModel)
{
    aTabulatedModel.print(anOutputStream);

    return anOutputStream;
}

bool Tabulated::isDefined() const
{
    return !interpolators_.isEmpty() && firstState_.isDefined() && lastState_.isDefined();
}

const Shared<const Frame>& Tabulated::accessFrame() const
{
    return frameSPtr_;
}

Interval Tabulated::getInterval() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    return Interval::Closed(firstState_.accessInstant(), lastState_.accessInstant());
}

Interpolator::Type Tabulated::getInterpolationType() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    // Since all interpolators are of the same type, we can just return the type of the first one.
    return interpolators_[0]->getInterpolationType();
}

State Tabulated::getFirstState() const
{
    return firstState_;
}

State Tabulated::getLastState() const
{
    return lastState_;
}

State Tabulated::calculateStateAt(const Instant& anInstant) const
{
    using ostk::core::type::Index;
    using ostk::core::type::String;

    using ostk::astrodynamics::trajectory::state::CoordinateBroker;

    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    if (anInstant < firstState_.accessInstant() || anInstant > lastState_.accessInstant())
    {
        throw ostk::core::error::RuntimeError(String::Format(
            "Provided instant [{}] is outside of interpolation range [{}, {}].",
            anInstant.toString(),
            firstState_.accessInstant().toString(),
            lastState_.accessInstant().toString()
        ));
    }

    VectorXd interpolatedCoordinates(interpolators_.getSize());

    for (Index i = 0; i < interpolators_.getSize(); ++i)
    {
        interpolatedCoordinates(i) = interpolators_[i]->evaluate((anInstant - firstState_.accessInstant()).inSeconds());
    }

    const Shared<const Frame>& frame = firstState_.accessFrame();
    const Shared<const CoordinateBroker>& coordinatesBroker = firstState_.accessCoordinateBroker();

    return State(anInstant, interpolatedCoordinates, frame, coordinatesBroker).inFrame(frameSPtr_);
}

Array<State> Tabulated::calculateStatesAt(const Array<Instant>& anInstantArray) const
{
    using ostk::core::type::Index;

    if (anInstantArray.isEmpty())
    {
        return Array<State>::Empty();
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    Array<State> stateArray = Array<State>(anInstantArray.getSize(), State::Undefined());

    for (Index i = 0; i < anInstantArray.getSize(); ++i)
    {
        stateArray[i] = this->calculateStateAt(anInstantArray[i]);
    }

    return stateArray;
}

void Tabulated::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::type::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Tabulated") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Start instant:" << (this->isDefined() ? this->getInterval().accessStart().toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "End instant:" << (this->isDefined() ? this->getInterval().accessEnd().toString() : "Undefined");

    ostk::core::utils::Print::Separator(anOutputStream);

    {
        const State firstState =
            this->isDefined() ? this->calculateStateAt(this->getInterval().accessStart()) : State::Undefined();

        ostk::core::utils::Print::Line(anOutputStream)
            << "First state:"
            << (firstState.isDefined() ? String::Format(
                                             "{} - {} - {}",
                                             firstState.accessInstant().toString(),
                                             firstState.getPosition().toString(),
                                             firstState.getVelocity().toString()
                                         )
                                       : "Undefined");
    }

    {
        const State lastState =
            this->isDefined() ? this->calculateStateAt(this->getInterval().accessEnd()) : State::Undefined();

        ostk::core::utils::Print::Line(anOutputStream)
            << "Last state:"
            << (lastState.isDefined() ? String::Format(
                                            "{} - {} - {}",
                                            lastState.accessInstant().toString(),
                                            lastState.getPosition().toString(),
                                            lastState.getVelocity().toString()
                                        )
                                      : "Undefined");
    }

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Tabulated Tabulated::Default(const Array<State>& aStateArray)
{
    return Tabulated(aStateArray, Tabulated::DefaultInterpolationTypes());
}

Tabulated Tabulated::Default(const Array<State>& aStateArray, const Shared<const Frame>& aFrameSPtr)
{
    return Tabulated(aStateArray, Tabulated::DefaultInterpolationTypes(), aFrameSPtr);
}

Map<Shared<const CoordinateSubset>, Interpolator::Type> Tabulated::DefaultInterpolationTypes()
{
    static const Map<Shared<const CoordinateSubset>, Interpolator::Type> defaultInterpolationTypes = {
        {CartesianPosition::Default(), Interpolator::Type::BarycentricRational},
        {CartesianVelocity::Default(), Interpolator::Type::BarycentricRational},
        {CartesianAcceleration::Default(), Interpolator::Type::BarycentricRational},
        {AngularVelocity::Default(), Interpolator::Type::BarycentricRational},
        {CoordinateSubset::Mass(), Interpolator::Type::ZeroOrder},
        {CoordinateSubset::DragCoefficient(), Interpolator::Type::ZeroOrder},
        {CoordinateSubset::SurfaceArea(), Interpolator::Type::ZeroOrder},
        {CoordinateSubset::MassFlowRate(), Interpolator::Type::ZeroOrder},
        {CoordinateSubset::BallisticCoefficient(), Interpolator::Type::ZeroOrder},
    };

    return defaultInterpolationTypes;
}

bool Tabulated::operator==(const Model& aModel) const
{
    const Tabulated* tabulatedModelPtr = dynamic_cast<const Tabulated*>(&aModel);

    return (tabulatedModelPtr != nullptr) && this->operator==(*tabulatedModelPtr);
}

bool Tabulated::operator!=(const Model& aModel) const
{
    return !((*this) == aModel);
}

bool Tabulated::computeInterpolationData(
    const Array<State>& aStateArray, VectorXd& aTimestampVector, MatrixXd& aCoordinateMatrix
)
{
    if (aStateArray.getSize() < 2)
    {
        return false;
    }

    Array<State> stateArray = aStateArray;

    std::sort(
        stateArray.begin(),
        stateArray.end(),
        [](const auto& lhs, const auto& rhs)
        {
            return lhs.getInstant() < rhs.getInstant();
        }
    );

    firstState_ = stateArray.accessFirst();
    lastState_ = stateArray.accessLast();

    aTimestampVector.resize(stateArray.getSize());
    aCoordinateMatrix.resize(stateArray.getSize(), firstState_.getSize());

    for (Index i = 0; i < stateArray.getSize(); ++i)
    {
        aTimestampVector(i) = (stateArray[i].accessInstant() - firstState_.accessInstant()).inSeconds();

        aCoordinateMatrix.row(i) = stateArray[i].accessCoordinates();
    }

    return true;
}

}  // namespace model
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
