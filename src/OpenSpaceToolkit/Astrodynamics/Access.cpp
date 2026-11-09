/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Access.hpp>

namespace ostk
{
namespace astrodynamics
{

Access::Access(
    const Access::Type& aType,
    const Instant& anAcquisitionOfSignal,
    const std::optional<Instant>& aTimeOfClosestApproach,
    const Instant& aLossOfSignal,
    const std::optional<Angle>& aMaxElevation
)
    : type_(aType),
      acquisitionOfSignal_(anAcquisitionOfSignal),
      timeOfClosestApproach_(aTimeOfClosestApproach),
      lossOfSignal_(aLossOfSignal),
      maxElevation_(aMaxElevation)
{
    if (type_ == Access::Type::Complete)
    {
        if (!timeOfClosestApproach_.has_value())
        {
            throw ostk::core::error::RuntimeError("A complete access requires a time of closest approach.");
        }

        if (timeOfClosestApproach_.value() < acquisitionOfSignal_)
        {
            throw ostk::core::error::RuntimeError(
                "TCA [{}] < AOS [{}]", timeOfClosestApproach_.value().toString(), acquisitionOfSignal_.toString()
            );
        }

        if (lossOfSignal_ < timeOfClosestApproach_.value())
        {
            throw ostk::core::error::RuntimeError(
                "LOS [{}] < TCA [{}]", lossOfSignal_.toString(), timeOfClosestApproach_.value().toString()
            );
        }
    }

    else if (type_ == Access::Type::Partial)
    {
        if (lossOfSignal_ < acquisitionOfSignal_)
        {
            throw ostk::core::error::RuntimeError(
                "LOS [{}] < AOS [{}]", lossOfSignal_.toString(), acquisitionOfSignal_.toString()
            );
        }
    }
}

bool Access::operator==(const Access& anAccess) const
{
    return (type_ == anAccess.type_) && (acquisitionOfSignal_ == anAccess.acquisitionOfSignal_) &&
           (timeOfClosestApproach_ == anAccess.timeOfClosestApproach_) && (lossOfSignal_ == anAccess.lossOfSignal_) &&
           (maxElevation_ == anAccess.maxElevation_);
}

bool Access::operator!=(const Access& anAccess) const
{
    return !((*this) == anAccess);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Access& anAccess)
{
    using ostk::physics::time::Duration;

    ostk::core::utils::Print::Header(anOutputStream, "Access");

    ostk::core::utils::Print::Line(anOutputStream) << "Type:" << Access::StringFromType(anAccess.type_);

    ostk::core::utils::Print::Line(anOutputStream)
        << "Acquisition Of Signal:" << anAccess.acquisitionOfSignal_.toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Time of Closest Approach:"
        << (anAccess.timeOfClosestApproach_.has_value() ? anAccess.timeOfClosestApproach_.value().toString()
                                                        : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream) << "Loss Of Signal:" << anAccess.lossOfSignal_.toString();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Duration:" << Duration::Between(anAccess.acquisitionOfSignal_, anAccess.lossOfSignal_).toString();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Maximum Elevation:"
        << (anAccess.maxElevation_.has_value() ? anAccess.maxElevation_->toString() : "Undefined");

    ostk::core::utils::Print::Footer(anOutputStream);

    return anOutputStream;
}

bool Access::isComplete() const
{
    return type_ == Access::Type::Complete;
}

Access::Type Access::getType() const
{
    return type_;
}

const Instant& Access::getAcquisitionOfSignal() const
{
    return acquisitionOfSignal_;
}

const std::optional<Instant>& Access::getTimeOfClosestApproach() const
{
    return timeOfClosestApproach_;
}

const Instant& Access::getLossOfSignal() const
{
    return lossOfSignal_;
}

Interval Access::getInterval() const
{
    return Interval::Closed(acquisitionOfSignal_, lossOfSignal_);
}

Duration Access::getDuration() const
{
    return Duration::Between(acquisitionOfSignal_, lossOfSignal_);
}

const std::optional<Angle>& Access::getMaxElevation() const
{
    return maxElevation_;
}

String Access::StringFromType(const Access::Type& aType)
{
    switch (aType)
    {
        case Access::Type::Complete:
            return "Complete";

        case Access::Type::Partial:
            return "Partial";

        default:
            break;
    }

    throw ostk::core::error::runtime::Wrong("Type");

    return String::Empty();
}

}  // namespace astrodynamics
}  // namespace ostk
