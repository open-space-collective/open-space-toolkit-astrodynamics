/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Pass.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{

Pass::Pass(
    const std::optional<int>& aRevolutionNumber,
    const std::optional<Instant>& anInstantAtAscendingNode,
    const std::optional<Instant>& anInstantAtNorthPoint,
    const std::optional<Instant>& anInstantAtDescendingNode,
    const std::optional<Instant>& anInstantAtSouthPoint,
    const std::optional<Instant>& anInstantAtPassBreak
)
    : type_(
          (anInstantAtAscendingNode.has_value() && anInstantAtNorthPoint.has_value() &&
           anInstantAtDescendingNode.has_value() && anInstantAtSouthPoint.has_value() &&
           anInstantAtPassBreak.has_value())
              ? Pass::Type::Complete
              : Pass::Type::Partial
      ),
      revolutionNumber_(aRevolutionNumber),
      instantAtAscendingNode_(anInstantAtAscendingNode),
      instantAtNorthPoint_(anInstantAtNorthPoint),
      instantAtDescendingNode_(anInstantAtDescendingNode),
      instantAtSouthPoint_(anInstantAtSouthPoint),
      instantAtPassBreak_(anInstantAtPassBreak)
{
    // The order of the instants is only verified along the leading run of known instants.

    if (instantAtAscendingNode_.has_value() && instantAtNorthPoint_.has_value())
    {
        if (instantAtAscendingNode_.value() > instantAtNorthPoint_.value())
        {
            throw ostk::core::error::RuntimeError("Ascending node must be before north point.");
        }

        if (instantAtDescendingNode_.has_value())
        {
            if (instantAtNorthPoint_.value() > instantAtDescendingNode_.value())
            {
                throw ostk::core::error::RuntimeError("North point must be before descending node.");
            }

            if (instantAtSouthPoint_.has_value())
            {
                if (instantAtDescendingNode_.value() > instantAtSouthPoint_.value())
                {
                    throw ostk::core::error::RuntimeError("Descending node must be before south point.");
                }

                if (instantAtPassBreak_.has_value())
                {
                    if (instantAtSouthPoint_.value() > instantAtPassBreak_.value())
                    {
                        throw ostk::core::error::RuntimeError("South point must be before pass break.");
                    }
                }
            }
        }
    }
}

bool Pass::operator==(const Pass& aPass) const
{
    return (type_ == aPass.type_) && revolutionNumber_.has_value() && aPass.revolutionNumber_.has_value() &&
           (revolutionNumber_.value() == aPass.revolutionNumber_.value()) &&
           (instantAtAscendingNode_ == aPass.instantAtAscendingNode_) &&
           (instantAtDescendingNode_ == aPass.instantAtDescendingNode_) &&
           (instantAtNorthPoint_ == aPass.instantAtNorthPoint_) &&
           (instantAtSouthPoint_ == aPass.instantAtSouthPoint_) && (instantAtPassBreak_ == aPass.instantAtPassBreak_);
}

bool Pass::operator!=(const Pass& aPass) const
{
    return !((*this) == aPass);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Pass& aPass)
{
    aPass.print(anOutputStream);

    return anOutputStream;
}

bool Pass::isComplete() const
{
    return type_ == Pass::Type::Complete;
}

Pass::Type Pass::getType() const
{
    return type_;
}

const std::optional<int>& Pass::getRevolutionNumber() const
{
    return revolutionNumber_;
}

std::optional<Duration> Pass::getDuration() const
{
    if (type_ == Pass::Type::Complete)
    {
        return instantAtPassBreak_.value() - instantAtAscendingNode_.value();
    }

    return std::nullopt;
}

std::optional<Instant> Pass::getStartInstant() const
{
    for (const std::optional<Instant>* instantPtr : {
             &instantAtAscendingNode_,
             &instantAtNorthPoint_,
             &instantAtDescendingNode_,
             &instantAtSouthPoint_,
             &instantAtPassBreak_,
         })
    {
        if (instantPtr->has_value())
        {
            return *instantPtr;
        }
    }

    return std::nullopt;
}

std::optional<Instant> Pass::getEndInstant() const
{
    for (const std::optional<Instant>* instantPtr : {
             &instantAtPassBreak_,
             &instantAtSouthPoint_,
             &instantAtDescendingNode_,
             &instantAtNorthPoint_,
             &instantAtAscendingNode_,
         })
    {
        if (instantPtr->has_value())
        {
            return *instantPtr;
        }
    }

    return std::nullopt;
}

std::optional<Interval> Pass::getInterval() const
{
    if (type_ == Pass::Type::Complete)
    {
        return Interval::Closed(instantAtAscendingNode_.value(), instantAtPassBreak_.value());
    }

    return std::nullopt;
}

const std::optional<Instant>& Pass::getInstantAtAscendingNode() const
{
    return instantAtAscendingNode_;
}

const std::optional<Instant>& Pass::getInstantAtNorthPoint() const
{
    return instantAtNorthPoint_;
}

const std::optional<Instant>& Pass::getInstantAtDescendingNode() const
{
    return instantAtDescendingNode_;
}

const std::optional<Instant>& Pass::getInstantAtSouthPoint() const
{
    return instantAtSouthPoint_;
}

const std::optional<Instant>& Pass::getInstantAtPassBreak() const
{
    return instantAtPassBreak_;
}

void Pass::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Pass") : void();

    const auto instantToString = [](const std::optional<Instant>& anInstant) -> String
    {
        return anInstant.has_value() ? anInstant.value().toString() : "Undefined";
    };

    const std::optional<Duration> duration = this->getDuration();

    ostk::core::utils::Print::Line(anOutputStream) << "Type:" << Pass::StringFromType(type_);
    ostk::core::utils::Print::Line(anOutputStream)
        << "Revolution #:" << (revolutionNumber_.has_value() ? std::to_string(revolutionNumber_.value()) : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Duration:" << (duration.has_value() ? duration.value().toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream) << "Ascending node:" << instantToString(instantAtAscendingNode_);
    ostk::core::utils::Print::Line(anOutputStream) << "North point:" << instantToString(instantAtNorthPoint_);
    ostk::core::utils::Print::Line(anOutputStream) << "Descending node:" << instantToString(instantAtDescendingNode_);
    ostk::core::utils::Print::Line(anOutputStream) << "South point:" << instantToString(instantAtSouthPoint_);
    ostk::core::utils::Print::Line(anOutputStream) << "Pass break:" << instantToString(instantAtPassBreak_);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

String Pass::StringFromType(const Pass::Type& aType)
{
    switch (aType)
    {
        case Pass::Type::Complete:
            return "Complete";

        case Pass::Type::Partial:
            return "Partial";

        default:
            throw ostk::core::error::runtime::Wrong("Type");
    }

    return String::Empty();
}

String Pass::StringFromPhase(const Pass::Phase& aPhase)
{
    switch (aPhase)
    {
        case Pass::Phase::Ascending:
            return "Ascending";

        case Pass::Phase::Descending:
            return "Descending";

        default:
            throw ostk::core::error::runtime::Wrong("Phase");
    }

    return String::Empty();
}

}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
