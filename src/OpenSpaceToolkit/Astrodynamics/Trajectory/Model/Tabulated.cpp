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

Tabulated::Tabulated(
    const Array<State>& aStateArray,
    const Interpolator::Type& anInterpolationType,
    const Shared<const Frame>& aFrameSPtr
)
    : Model(),
      outputFrameSPtr_(aFrameSPtr)
{
    if (aFrameSPtr == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    VectorXd timestamps;
    MatrixXd coordinates;

    this->computeInterpolationData(aStateArray, timestamps, coordinates);

    interpolators_.reserve(coordinates.cols());

    for (Index i = 0; i < Size(coordinates.cols()); ++i)
    {
        interpolators_.add(Interpolator::GenerateInterpolator(anInterpolationType, timestamps, coordinates.col(i)));
    }
}

Tabulated::Tabulated(
    const Array<State>& aStateArray,
    const Map<Shared<const CoordinateSubset>, Interpolator::Type>& anInterpolationTypeMap,
    const Shared<const Frame>& aFrameSPtr
)
    : Model(),
      outputFrameSPtr_(aFrameSPtr)
{
    using ostk::core::type::String;

    using ostk::astrodynamics::trajectory::state::CoordinateBroker;

    if (aFrameSPtr == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    VectorXd timestamps;
    MatrixXd coordinates;

    this->computeInterpolationData(aStateArray, timestamps, coordinates);

    const Shared<const CoordinateBroker>& coordinatesBroker = firstState_->getCoordinateBroker();

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

    for (const auto& coordinateSubsetSPtr : coordinatesBroker->getSubsets())
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

Tabulated* Tabulated::clone() const
{
    return new Tabulated(*this);
}

bool Tabulated::operator==(const Tabulated& aTabulatedModel) const
{
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

    const bool framesEqual =
        ((outputFrameSPtr_ != nullptr) && (aTabulatedModel.outputFrameSPtr_ != nullptr) &&
         ((*outputFrameSPtr_) == (*aTabulatedModel.outputFrameSPtr_)));

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

const Shared<const Frame>& Tabulated::getFrame() const
{
    return outputFrameSPtr_;
}

Interval Tabulated::getInterval() const
{
    return Interval::Closed(firstState_->getInstant(), lastState_->getInstant());
}

Interpolator::Type Tabulated::getInterpolationType() const
{
    // Since all interpolators are of the same type, we can just return the type of the first one.
    return interpolators_[0]->getInterpolationType();
}

const State& Tabulated::getFirstState() const
{
    return firstState_.value();
}

const State& Tabulated::getLastState() const
{
    return lastState_.value();
}

State Tabulated::calculateStateAt(const Instant& anInstant) const
{
    using ostk::core::type::Index;
    using ostk::core::type::String;

    using ostk::astrodynamics::trajectory::state::CoordinateBroker;

    if (anInstant < firstState_->getInstant() || anInstant > lastState_->getInstant())
    {
        throw ostk::core::error::RuntimeError(String::Format(
            "Provided instant [{}] is outside of interpolation range [{}, {}].",
            anInstant.toString(),
            firstState_->getInstant().toString(),
            lastState_->getInstant().toString()
        ));
    }

    VectorXd interpolatedCoordinates(interpolators_.getSize());

    for (Index i = 0; i < interpolators_.getSize(); ++i)
    {
        interpolatedCoordinates(i) = interpolators_[i]->evaluate((anInstant - firstState_->getInstant()).inSeconds());
    }

    // The interpolators are built in the output frame, so the interpolated coordinates are already expressed in it.
    const Shared<const CoordinateBroker>& coordinatesBroker = firstState_->getCoordinateBroker();

    return State(anInstant, interpolatedCoordinates, outputFrameSPtr_, coordinatesBroker);
}

Array<State> Tabulated::calculateStatesAt(const Array<Instant>& anInstantArray) const
{
    using ostk::core::type::Index;

    if (anInstantArray.isEmpty())
    {
        return Array<State>::Empty();
    }

    Array<State> stateArray = Array<State>::Empty();
    stateArray.reserve(anInstantArray.getSize());

    for (const Instant& instant : anInstantArray)
    {
        stateArray.add(this->calculateStateAt(instant));
    }

    return stateArray;
}

void Tabulated::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::type::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Tabulated") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Start instant:" << this->getInterval().getStart().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "End instant:" << this->getInterval().getEnd().toString();

    ostk::core::utils::Print::Separator(anOutputStream);

    {
        const State firstState = this->calculateStateAt(this->getInterval().getStart());

        ostk::core::utils::Print::Line(anOutputStream) << "First state:"
                                                       << String::Format(
                                                              "{} - {} - {}",
                                                              firstState.getInstant().toString(),
                                                              firstState.getPosition().toString(),
                                                              firstState.getVelocity().toString()
                                                          );
    }

    {
        const State lastState = this->calculateStateAt(this->getInterval().getEnd());

        ostk::core::utils::Print::Line(anOutputStream) << "Last state:"
                                                       << String::Format(
                                                              "{} - {} - {}",
                                                              lastState.getInstant().toString(),
                                                              lastState.getPosition().toString(),
                                                              lastState.getVelocity().toString()
                                                          );
    }

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
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

void Tabulated::computeInterpolationData(
    const Array<State>& aStateArray, VectorXd& aTimestampVector, MatrixXd& aCoordinateMatrix
)
{
    if (aStateArray.getSize() < 2)
    {
        throw ostk::core::error::runtime::Wrong("State array", "At least two states are required.");
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

    // Cache the first and last states in their native frame, so getFirstState()/getLastState() preserve the
    // frame of the provided states.
    firstState_ = stateArray.getFirst();
    lastState_ = stateArray.getLast();

    aTimestampVector.resize(stateArray.getSize());
    aCoordinateMatrix.resize(stateArray.getSize(), firstState_->getSize());

    // Express each state in the output frame before extracting its coordinates, so that interpolation is performed
    // directly in the output frame and calculateStateAt requires no per-evaluation frame transform.
    for (Index i = 0; i < stateArray.getSize(); ++i)
    {
        const State stateInOutputFrame = stateArray[i].inFrame(outputFrameSPtr_);

        aTimestampVector(i) = (stateInOutputFrame.getInstant() - firstState_->getInstant()).inSeconds();

        aCoordinateMatrix.row(i) = stateInOutputFrame.getCoordinates();
    }
}

}  // namespace model
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
