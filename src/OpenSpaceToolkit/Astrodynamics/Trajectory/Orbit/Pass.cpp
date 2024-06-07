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
    const Integer& aRevolutionNumber,
    const Instant& anInstantAtAscendingNode,
    const Instant& anInstantAtNorthPoint,
    const Instant& anInstantAtDescendingNode,
    const Instant& anInstantAtSouthPoint,
    const Instant& anInstantAtPassBreak
)
    : revolutionNumber_(aRevolutionNumber),
      instantAtAscendingNode_(anInstantAtAscendingNode),
      instantAtNorthPoint_(anInstantAtNorthPoint),
      instantAtDescendingNode_(anInstantAtDescendingNode),
      instantAtSouthPoint_(anInstantAtSouthPoint),
      instantAtPassBreak_(anInstantAtPassBreak)
{
    if (!anInstantAtNorthPoint.isDefined() && !anInstantAtSouthPoint.isDefined() &&
        !anInstantAtDescendingNode.isDefined() && !anInstantAtAscendingNode.isDefined() &&
        !anInstantAtPassBreak.isDefined() && !revolutionNumber_.isDefined())
    {
        type_ = Pass::Type::Undefined;
    }
    else if ((!anInstantAtNorthPoint.isDefined() || !anInstantAtSouthPoint.isDefined() ||
              !anInstantAtDescendingNode.isDefined() || !anInstantAtAscendingNode.isDefined() ||
              !anInstantAtPassBreak.isDefined()) &&
             revolutionNumber_.isDefined())
    {
        type_ = Pass::Type::Partial;
    }
    else
    {
        type_ = Pass::Type::Complete;
    }

    if (instantAtAscendingNode_.isDefined())
    {
        if (instantAtNorthPoint_.isDefined())
        {
            if (instantAtAscendingNode_ > instantAtNorthPoint_)
            {
                throw ostk::core::error::RuntimeError("Ascending node must be before north point.");
            }

            if (instantAtDescendingNode_.isDefined())
            {
                if (instantAtNorthPoint_ > instantAtDescendingNode_)
                {
                    throw ostk::core::error::RuntimeError("North point must be before descending node.");
                }

                if (instantAtSouthPoint_.isDefined())
                {
                    if (instantAtDescendingNode_ > instantAtSouthPoint_)
                    {
                        throw ostk::core::error::RuntimeError("Descending node must be before south point.");
                    }

                    if (instantAtPassBreak_.isDefined())
                    {
                        if (instantAtSouthPoint_ > instantAtPassBreak_)
                        {
                            throw ostk::core::error::RuntimeError("South point must be before pass break.");
                        }
                    }
                }
            }
        }
    }
}

bool Pass::operator==(const Pass& aPass) const
{
    if ((!this->isDefined()) || (!aPass.isDefined()))
    {
        return false;
    }

    bool isEqual = (type_ == aPass.type_) && (revolutionNumber_ == aPass.revolutionNumber_) &&
                   (instantAtAscendingNode_.isDefined() == aPass.instantAtAscendingNode_.isDefined()) &&
                   (instantAtDescendingNode_.isDefined() == aPass.instantAtDescendingNode_.isDefined()) &&
                   (instantAtNorthPoint_.isDefined() == aPass.instantAtNorthPoint_.isDefined()) &&
                   (instantAtSouthPoint_.isDefined() == aPass.instantAtSouthPoint_.isDefined()) &&
                   (instantAtPassBreak_.isDefined() == aPass.instantAtPassBreak_.isDefined());

    if (instantAtAscendingNode_.isDefined() && aPass.instantAtAscendingNode_.isDefined())
    {
        isEqual = isEqual && (instantAtAscendingNode_ == aPass.instantAtAscendingNode_);
    }

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

    if (instantAtPassBreak_.isDefined() && aPass.instantAtPassBreak_.isDefined())
    {
        isEqual = isEqual && (instantAtPassBreak_ == aPass.instantAtPassBreak_);
    }

    return isEqual;
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

Duration Pass::getDuration() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    if (type_ == Pass::Type::Complete)
    {
        return instantAtPassBreak_ - instantAtAscendingNode_;
    }

    return Duration::Undefined();
}

Instant Pass::getStartInstant() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    if (instantAtAscendingNode_.isDefined())
    {
        return instantAtAscendingNode_;
    }

    if (instantAtNorthPoint_.isDefined())
    {
        return instantAtNorthPoint_;
    }

    if (instantAtDescendingNode_.isDefined())
    {
        return instantAtDescendingNode_;
    }

    if (instantAtSouthPoint_.isDefined())
    {
        return instantAtSouthPoint_;
    }

    return instantAtPassBreak_;
}

Instant Pass::getEndInstant() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    if (instantAtPassBreak_.isDefined())
    {
        return instantAtPassBreak_;
    }

    if (instantAtSouthPoint_.isDefined())
    {
        return instantAtSouthPoint_;
    }

    if (instantAtDescendingNode_.isDefined())
    {
        return instantAtDescendingNode_;
    }

    if (instantAtNorthPoint_.isDefined())
    {
        return instantAtNorthPoint_;
    }

    return instantAtAscendingNode_;
}

const Instant& Pass::accessInstantAtAscendingNode() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    return instantAtAscendingNode_;
}

const Instant& Pass::accessInstantAtNorthPoint() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    return instantAtNorthPoint_;
}

const Instant& Pass::accessInstantAtDescendingNode() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    return instantAtDescendingNode_;
}

const Instant& Pass::accessInstantAtSouthPoint() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    return instantAtSouthPoint_;
}

const Instant& Pass::accessInstantAtPassBreak() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Pass");
    }

    return instantAtPassBreak_;
}

void Pass::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Pass") : void();

    const Duration duration = this->isDefined() ? this->getDuration() : Duration::Undefined();

    ostk::core::utils::Print::Line(anOutputStream) << "Type:" << Pass::StringFromType(type_);
    ostk::core::utils::Print::Line(anOutputStream)
        << "Revolution #:" << (revolutionNumber_.isDefined() ? revolutionNumber_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Duration:" << (duration.isDefined() ? duration.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Ascending node:"
        << (instantAtAscendingNode_.isDefined() ? instantAtAscendingNode_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "North point:" << (instantAtNorthPoint_.isDefined() ? instantAtNorthPoint_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Descending node:"
        << (instantAtDescendingNode_.isDefined() ? instantAtDescendingNode_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "South point:" << (instantAtSouthPoint_.isDefined() ? instantAtSouthPoint_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Pass break:" << (instantAtPassBreak_.isDefined() ? instantAtPassBreak_.toString() : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Pass Pass::Undefined()
{
    return {
        Integer::Undefined(),
        Instant::Undefined(),
        Instant::Undefined(),
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

}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
