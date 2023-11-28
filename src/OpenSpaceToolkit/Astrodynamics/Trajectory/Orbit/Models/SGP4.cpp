/// Apache License 2.0

#include <iostream>

#include <sgp4/SGP4.h>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace models
{

using ostk::physics::coord::Transform;

class SGP4::Impl
{
   public:
    Impl(const TLE& aTle);

    Impl(const SGP4::Impl& anImpl) = delete;

    SGP4::Impl& operator=(const SGP4::Impl& anImpl) = delete;

    State calculateStateAt(const Instant& anInstant) const;

   private:
    const TLE& tle_;
    libsgp4::SGP4 sgp4_;

    Shared<const Frame> temeFrameOfEpochSPtr_;
};

SGP4::Impl::Impl(const TLE& aTle)
    : tle_(aTle),
      sgp4_(libsgp4::Tle(tle_.getSatelliteName(), tle_.getFirstLine(), tle_.getSecondLine())),
      temeFrameOfEpochSPtr_(Frame::TEMEOfEpoch(tle_.getEpoch()))
{
}

State SGP4::Impl::calculateStateAt(const Instant& anInstant) const
{
    using ostk::math::object::Vector3d;

    using ostk::physics::time::Duration;

    const Real durationFromEpoch_min = Duration::Between(this->tle_.getEpoch(), anInstant).inMinutes();

    const libsgp4::Eci xv_TEME = this->sgp4_.FindPosition(durationFromEpoch_min);

    const libsgp4::Vector x_TEME_km = xv_TEME.Position();
    const libsgp4::Vector v_TEME_kmps = xv_TEME.Velocity();

    const Vector3d x_TEME_m = Vector3d(x_TEME_km.x, x_TEME_km.y, x_TEME_km.z) * 1e3;
    const Vector3d v_TEME_mps = Vector3d(v_TEME_kmps.x, v_TEME_kmps.y, v_TEME_kmps.z) * 1e3;

    const Position position_TEME = {x_TEME_m, Position::Unit::Meter, this->temeFrameOfEpochSPtr_};
    const Velocity velocity_TEME = {v_TEME_mps, Velocity::Unit::MeterPerSecond, this->temeFrameOfEpochSPtr_};

    const State state_TEME = {anInstant, position_TEME, velocity_TEME};

    static const Shared<const Frame> gcrfSPtr = Frame::GCRF();

    const State state_GCRF = state_TEME.inFrame(gcrfSPtr);

    return state_GCRF;
}

SGP4::SGP4(const TLE& aTle)
    : Model(),
      tle_(aTle),
      implUPtr_(std::make_unique<SGP4::Impl>(tle_))
{
}

SGP4::SGP4(const SGP4& aSGP4Model)
    : Model(aSGP4Model),
      tle_(aSGP4Model.tle_),
      implUPtr_(std::make_unique<SGP4::Impl>(tle_))
{
}

SGP4::~SGP4() {}

SGP4& SGP4::operator=(const SGP4& aSGP4Model)
{
    if (this != &aSGP4Model)
    {
        Model::operator=(aSGP4Model);

        this->tle_ = aSGP4Model.tle_;

        this->implUPtr_ = std::make_unique<SGP4::Impl>(tle_);
    }

    return *this;
}

SGP4* SGP4::clone() const
{
    return new SGP4(*this);
}

bool SGP4::operator==(const SGP4& aSGP4Model) const
{
    if ((!this->isDefined()) || (!aSGP4Model.isDefined()))
    {
        return false;
    }

    return this->tle_ == aSGP4Model.tle_;
}

bool SGP4::operator!=(const SGP4& aSGP4Model) const
{
    return !((*this) == aSGP4Model);
}

std::ostream& operator<<(std::ostream& anOutputStream, const SGP4& aSGP4Model)
{
    aSGP4Model.print(anOutputStream);

    return anOutputStream;
}

bool SGP4::isDefined() const
{
    return this->tle_.isDefined() && (this->implUPtr_ != nullptr);
}

TLE SGP4::getTle() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4");
    }

    return this->tle_;
}

Instant SGP4::getEpoch() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4");
    }

    return this->tle_.getEpoch();
}

Integer SGP4::getRevolutionNumberAtEpoch() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4");
    }

    return this->tle_.getRevolutionNumberAtEpoch();
}

State SGP4::calculateStateAt(const Instant& anInstant) const
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4");
    }

    return this->implUPtr_->calculateStateAt(anInstant);
}

Integer SGP4::calculateRevolutionNumberAt(const Instant& anInstant) const
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4");
    }

    if (anInstant == this->tle_.getEpoch())
    {
        return this->getRevolutionNumberAtEpoch();
    }

    return Integer::Undefined();
}

void SGP4::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "SGP4") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Epoch:" << (this->getEpoch().isDefined() ? this->getEpoch().toString() : "Undefined");

    ostk::core::utils::Print::Separator(anOutputStream, "Two-Line Elements");

    // tle_.print(anOutputStream, false) ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

bool SGP4::operator==(const trajectory::Model& aModel) const
{
    const SGP4* aSGP4ModelPtr = dynamic_cast<const SGP4*>(&aModel);

    return (aSGP4ModelPtr != nullptr) && this->operator==(*aSGP4ModelPtr);
}

bool SGP4::operator!=(const trajectory::Model& aModel) const
{
    return !((*this) == aModel);
}

}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
