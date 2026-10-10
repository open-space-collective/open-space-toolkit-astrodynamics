/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{
namespace profile
{

Model::Model()
    : validityInterval_(std::nullopt)
{
}

Model::~Model() {}

bool Model::operator!=(const Model& aModel) const
{
    return !((*this) == aModel);
}

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
    if (anInstantArray.isEmpty())
    {
        return Array<State>::Empty();
    }

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

}  // namespace profile
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
