/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

Model::Model()
    : validityInterval_(std::nullopt)
{
}

Model::~Model() {}

std::ostream& operator<<(std::ostream& anOutputStream, const Model& aModel)
{
    aModel.print(anOutputStream);

    return anOutputStream;
}

std::optional<Interval> Model::getValidityInterval() const
{
    return validityInterval_;
}

Array<State> Model::calculateStatesAt(const Array<Instant>& anInstantArray) const
{
    Array<State> stateArray = Array<State>::Empty();

    stateArray.reserve(anInstantArray.getSize());

    for (const auto& instant : anInstantArray)
    {
        stateArray.add(this->calculateStateAt(instant));
    }

    return stateArray;
}

void Model::setValidityInterval(const std::optional<Interval>& anInterval)
{
    validityInterval_ = anInterval;
}

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
