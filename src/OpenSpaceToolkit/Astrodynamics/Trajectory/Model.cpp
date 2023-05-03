// Copyright © Loft Orbital Solutions Inc.

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

Model::Model() { }

Model::~Model() { }

std::ostream& operator<<(std::ostream& anOutputStream, const Model& aModel)
{
    aModel.print(anOutputStream);

    return anOutputStream;
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

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
