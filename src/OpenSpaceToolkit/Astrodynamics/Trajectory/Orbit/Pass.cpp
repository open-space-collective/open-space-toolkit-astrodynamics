/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Pass.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace orbit
{

Pass::Pass(
    const Pass::Type& aType,
    const Integer& aRevolutionNumber,
    const Interval& anInterval,
    const Instant& anInstantAtDescendingNode,
    const Instant& anInstantAtNorthPoint,
    const Instant& anInstantAtSouthPoint
)
    : type_(aType),
      revolutionNumber_(aRevolutionNumber),
      interval_(anInterval),
      instantAtDescendingNode_(anInstantAtDescendingNode),
      instantAtNorthPoint_(anInstantAtNorthPoint),
      instantAtSouthPoint_(anInstantAtSouthPoint)
{
    if ((aType == Pass::Type::Complete) && (!anInstantAtNorthPoint.isDefined() || !anInstantAtSouthPoint.isDefined()))
    {
        throw ostk::core::error::RuntimeError("Complete pass must have both north and south points defined.");
    }
}

bool Pass::operator==(const Pass& aPass) const
{
    if ((!this->isDefined()) || (!aPass.isDefined()))
    {
        return false;
    }

    bool isEqual = (type_ == aPass.type_) && (revolutionNumber_ == aPass.revolutionNumber_) &&
                   (interval_ == aPass.interval_) &&
                   (instantAtDescendingNode_.isDefined() == aPass.instantAtDescendingNode_.isDefined()) &&
                   (instantAtNorthPoint_.isDefined() == aPass.instantAtNorthPoint_.isDefined()) &&
                   (instantAtSouthPoint_.isDefined() == aPass.instantAtSouthPoint_.isDefined());

    if (instantAtDescendingNode_.isDefined() && aPass.instantAtDescendingNode_.isDefined())
    {
        isEqual = isEqual && (instantAtDescendingNode_ == aPass.instantAtDescendingNode_);
    }

    if (instantAtNorthPoint_.isDefined() && aPass.instantAtNorthPoint_.isDefined())
    {
        isEqual = isEqual && (instantAtNorthPoint_ == aPass.instantAtNorthPoint_);
    }

    if (instantAtSouthPoint_.isDefined() && aPass.instantAtSouthPoint_.isDefined())
    {
        isEqual = isEqual && (instantAtSouthPoint_ == aPass.instantAtSouthPoint_);
    }

    return isEqual;
}

bool Pass::operator!=(const Pass& aPass) const
{
    return !((*this) == aPass);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Pass& aPass)
{
    ostk::core::utils::Print::Header(anOutputStream, "Pass");

    ostk::core::utils::Print::Line(anOutputStream) << "Type:" << Pass::StringFromType(aPass.type_);
    ostk::core::utils::Print::Line(anOutputStream)
        << "Revolution #:" << (aPass.revolutionNumber_.isDefined() ? aPass.revolutionNumber_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Start time:" << (aPass.interval_.isDefined() ? aPass.interval_.accessStart().toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "End time:" << (aPass.interval_.isDefined() ? aPass.interval_.accessEnd().toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Duration:" << (aPass.interval_.isDefined() ? aPass.interval_.getDuration().toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Ascending node:" << (aPass.interval_.isDefined() ? aPass.interval_.accessStart().toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "North point:"
        << (aPass.instantAtNorthPoint_.isDefined() ? aPass.instantAtNorthPoint_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Descending node:"
        << (aPass.instantAtDescendingNode_.isDefined() ? aPass.instantAtDescendingNode_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "South point:"
        << (aPass.instantAtSouthPoint_.isDefined() ? aPass.instantAtSouthPoint_.toString() : "Undefined");

    ostk::core::utils::Print::Footer(anOutputStream);

    return anOutputStream;
}

bool Pass::isDefined() const
{
    return type_ != Pass::Type::Undefined;
}

bool Pass::isComplete() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    return type_ == Pass::Type::Complete;
}

Pass::Type Pass::getType() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    return type_;
}

Integer Pass::getRevolutionNumber() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    return revolutionNumber_;
}

Interval Pass::getInterval() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    return interval_;
}

const Instant& Pass::accessInstantAtAscendingNode() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    return interval_.accessStart();
}

const Instant& Pass::accessInstantAtDescendingNode() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    return instantAtDescendingNode_;
}

const Instant& Pass::accessInstantAtNorthPoint() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    return instantAtNorthPoint_;
}

const Instant& Pass::accessInstantAtSouthPoint() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    return instantAtSouthPoint_;
}

Pass Pass::Undefined()
{
    return {
        Pass::Type::Undefined,
        Integer::Undefined(),
        Interval::Undefined(),
        Instant::Undefined(),
        Instant::Undefined(),
        Instant::Undefined(),
    };
}

String Pass::StringFromType(const Pass::Type& aType)
{
    switch (aType)
    {
        case Pass::Type::Undefined:
            return "Undefined";

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
        case Pass::Phase::Undefined:
            return "Undefined";

        case Pass::Phase::Ascending:
            return "Ascending";

        case Pass::Phase::Descending:
            return "Descending";

        default:
            throw ostk::core::error::runtime::Wrong("Phase");
    }

    return String::Empty();
}

String Pass::StringFromQuarter(const Pass::Quarter& aQuarter)
{
    switch (aQuarter)
    {
        case Pass::Quarter::Undefined:
            return "Undefined";

        case Pass::Quarter::First:
            return "First";

        case Pass::Quarter::Second:
            return "Second";

        case Pass::Quarter::Third:
            return "Third";

        case Pass::Quarter::Fourth:
            return "Fourth";

        default:
            throw ostk::core::error::runtime::Wrong("Quarter");
    }

    return String::Empty();
}

}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
