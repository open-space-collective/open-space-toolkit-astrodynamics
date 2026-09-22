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

Model::OutOfBoundsError::OutOfBoundsError(const Instant& anInstant, const Interval& anInterval, const String& aMessage)
    : ostk::core::error::RuntimeError(aMessage),
      instant_(anInstant),
      interval_(anInterval)
{
}

Instant Model::OutOfBoundsError::getInstant() const
{
    return instant_;
}

Interval Model::OutOfBoundsError::getInterval() const
{
    return interval_;
}

Model::BeforeStartError::BeforeStartError(const Instant& anInstant, const Interval& anInterval)
    : OutOfBoundsError(
          anInstant,
          anInterval,
          String::Format(
              "Provided instant [{}] is before the start of the interval [{}, {}].",
              anInstant.toString(),
              anInterval.accessStart().toString(),
              anInterval.accessEnd().toString()
          )
      )
{
}

Model::AfterEndError::AfterEndError(const Instant& anInstant, const Interval& anInterval)
    : OutOfBoundsError(
          anInstant,
          anInterval,
          String::Format(
              "Provided instant [{}] is after the end of the interval [{}, {}].",
              anInstant.toString(),
              anInterval.accessStart().toString(),
              anInterval.accessEnd().toString()
          )
      )
{
}

Model::Model() {}

Model::~Model() {}

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
}  // namespace astrodynamics
}  // namespace ostk
