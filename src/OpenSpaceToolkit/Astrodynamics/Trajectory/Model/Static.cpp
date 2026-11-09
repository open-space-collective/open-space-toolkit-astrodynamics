/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Static.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace model
{

Static::Static(const Position& aPosition, const Shared<const Frame>& aFrameSPtr)
    : Model(),
      position_(aPosition),
      outputFrameSPtr_((aFrameSPtr != nullptr) ? aFrameSPtr : aPosition.accessFrame())
{
    if (aPosition.accessFrame()->isQuasiInertial())
    {
        throw ostk::core::error::runtime::Wrong(
            "Position Frame Quasi Inertial", aPosition.accessFrame()->isQuasiInertial()
        );
    }

    if (!outputFrameSPtr_->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }
}

Static* Static::clone() const
{
    return new Static(*this);
}

bool Static::operator==(const Static& aStaticModel) const
{
    if ((!this->isDefined()) || (!aStaticModel.isDefined()))
    {
        return false;
    }

    return (position_ == aStaticModel.position_) && ((*outputFrameSPtr_) == (*aStaticModel.outputFrameSPtr_));
}

bool Static::operator!=(const Static& aStaticModel) const
{
    return !((*this) == aStaticModel);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Static& aStaticModel)
{
    aStaticModel.print(anOutputStream);

    return anOutputStream;
}

bool Static::isDefined() const
{
    return position_.isDefined();
}

Shared<const Frame> Static::getFrame() const
{
    return outputFrameSPtr_;
}

State Static::calculateStateAt(const Instant& anInstant) const
{
    using ostk::physics::coordinate::Position;

    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Static");
    }

    return State(anInstant, position_, Velocity::MetersPerSecond({0.0, 0.0, 0.0}, position_.accessFrame()))
        .inFrame(outputFrameSPtr_);
}

void Static::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::type::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Static") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Position:" << (position_.isDefined() ? position_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream) << "Output Frame:" << outputFrameSPtr_->getName();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

bool Static::operator==(const Model& aModel) const
{
    const Static* staticModelPtr = dynamic_cast<const Static*>(&aModel);

    return (staticModelPtr != nullptr) && this->operator==(*staticModelPtr);
}

bool Static::operator!=(const Model& aModel) const
{
    return !((*this) == aModel);
}

}  // namespace model
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
