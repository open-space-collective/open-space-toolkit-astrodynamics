/// Apache License 2.0

#include <algorithm>
#include <cmath>
#include <iostream>

#include <sgp4/SGP4.h>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{
namespace model
{

using ostk::physics::coordinate::Transform;

class SGP4::Impl
{
   public:
    Impl(const TLE& aTle, const Shared<const Frame>& anOutputFrameSPtr);

    Impl(const SGP4::Impl& anImpl) = delete;

    SGP4::Impl& operator=(const SGP4::Impl& anImpl) = delete;

    State calculateStateAt(const Instant& anInstant) const;

   private:
    TLE tle_;
    Shared<const Frame> outputFrameSPtr_;
    libsgp4::SGP4 sgp4_;

    Shared<const Frame> temeFrameOfEpochSPtr_;
};

SGP4::Impl::Impl(const TLE& aTle, const Shared<const Frame>& anOutputFrameSPtr)
    : tle_(aTle),
      outputFrameSPtr_(anOutputFrameSPtr),
      sgp4_(libsgp4::Tle(tle_.getSatelliteName(), tle_.getFirstLine(), tle_.getSecondLine())),
      temeFrameOfEpochSPtr_(Frame::TEME())
{
}

State SGP4::Impl::calculateStateAt(const Instant& anInstant) const
{
    using ostk::mathematics::object::Vector3d;

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

    return state_TEME.inFrame(this->outputFrameSPtr_);
}

SGP4::SGP4(const TLE& aTle)
    : Model(),
      tle_(aTle),
      tleArray_(Array<TLE>::Empty()),
      outputFrameSPtr_(Frame::GCRF()),
      implUPtr_(std::make_unique<SGP4::Impl>(tle_, outputFrameSPtr_)),
      cachedTleIndex_(0)
{
}

SGP4::SGP4(const TLE& aTle, const Shared<const Frame>& anOutputFrameSPtr)
    : Model(),
      tle_(aTle),
      tleArray_(Array<TLE>::Empty()),
      outputFrameSPtr_(anOutputFrameSPtr),
      implUPtr_(std::make_unique<SGP4::Impl>(tle_, outputFrameSPtr_)),
      cachedTleIndex_(0)
{
}

SGP4::SGP4(const Array<TLE>& aTleArray)
    : Model(),
      tle_(TLE::Undefined()),
      tleArray_(aTleArray),
      outputFrameSPtr_(Frame::GCRF()),
      implUPtr_(nullptr),
      cachedTleIndex_(0)
{
    if (tleArray_.isEmpty())
    {
        throw ostk::core::error::RuntimeError("TLE array is empty.");
    }

    std::sort(
        tleArray_.begin(),
        tleArray_.end(),
        [](const TLE& a, const TLE& b)
        {
            return a.getEpoch() < b.getEpoch();
        }
    );

    tle_ = tleArray_.accessFirst();
    implUPtr_ = std::make_unique<SGP4::Impl>(tleArray_[cachedTleIndex_], outputFrameSPtr_);
}

SGP4::SGP4(const Array<TLE>& aTleArray, const Shared<const Frame>& anOutputFrameSPtr)
    : Model(),
      tle_(TLE::Undefined()),
      tleArray_(aTleArray),
      outputFrameSPtr_(anOutputFrameSPtr),
      implUPtr_(nullptr),
      cachedTleIndex_(0)
{
    if (tleArray_.isEmpty())
    {
        throw ostk::core::error::RuntimeError("TLE array is empty.");
    }

    std::sort(
        tleArray_.begin(),
        tleArray_.end(),
        [](const TLE& a, const TLE& b)
        {
            return a.getEpoch() < b.getEpoch();
        }
    );

    tle_ = tleArray_.accessFirst();
    implUPtr_ = std::make_unique<SGP4::Impl>(tleArray_[cachedTleIndex_], outputFrameSPtr_);
}

SGP4::SGP4(const SGP4& aSGP4Model)
    : Model(aSGP4Model),
      tle_(aSGP4Model.tle_),
      tleArray_(aSGP4Model.tleArray_),
      outputFrameSPtr_(aSGP4Model.outputFrameSPtr_),
      implUPtr_(nullptr),
      cachedTleIndex_(0)
{
    if (!tleArray_.isEmpty())
    {
        implUPtr_ = std::make_unique<SGP4::Impl>(tleArray_[cachedTleIndex_], outputFrameSPtr_);
    }
    else if (tle_.isDefined())
    {
        implUPtr_ = std::make_unique<SGP4::Impl>(tle_, outputFrameSPtr_);
    }
}

SGP4::~SGP4() {}

SGP4& SGP4::operator=(const SGP4& aSGP4Model)
{
    if (this != &aSGP4Model)
    {
        Model::operator=(aSGP4Model);

        this->tle_ = aSGP4Model.tle_;
        this->tleArray_ = aSGP4Model.tleArray_;
        this->outputFrameSPtr_ = aSGP4Model.outputFrameSPtr_;
        this->cachedTleIndex_ = 0;

        if (!tleArray_.isEmpty())
        {
            this->implUPtr_ = std::make_unique<SGP4::Impl>(tleArray_[cachedTleIndex_], outputFrameSPtr_);
        }
        else if (tle_.isDefined())
        {
            this->implUPtr_ = std::make_unique<SGP4::Impl>(tle_, outputFrameSPtr_);
        }
        else
        {
            this->implUPtr_ = nullptr;
        }
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

    return (this->tle_ == aSGP4Model.tle_) && (this->tleArray_ == aSGP4Model.tleArray_) &&
           (this->outputFrameSPtr_ == aSGP4Model.outputFrameSPtr_);
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
    return (this->tle_.isDefined() || !this->tleArray_.isEmpty()) && (this->implUPtr_ != nullptr);
}

TLE SGP4::getTle() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4");
    }

    return this->tle_;
}

Array<TLE> SGP4::getTles() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4");
    }

    return this->tleArray_;
}

Shared<const Frame> SGP4::getOutputFrame() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4");
    }

    return this->outputFrameSPtr_;
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

    if (tleArray_.getSize() > 1)
    {
        const Size tleIndex = this->findClosestTleIndex(anInstant);
        this->ensureImplForTleIndex(tleIndex);
    }

    return this->implUPtr_->calculateStateAt(anInstant);
}

Array<State> SGP4::calculateStatesAt(const Array<Instant>& anInstantArray) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4");
    }

    Array<State> stateArray = Array<State>::Empty();
    stateArray.reserve(anInstantArray.getSize());

    for (const auto& instant : anInstantArray)
    {
        stateArray.add(this->calculateStateAt(instant));
    }

    return stateArray;
}

void SGP4::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "SGP4") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Epoch:" << (this->isDefined() ? this->getEpoch().toString() : "Undefined");

    if (!tleArray_.isEmpty())
    {
        ostk::core::utils::Print::Line(anOutputStream) << "TLE count:" << tleArray_.getSize();
    }

    ostk::core::utils::Print::Separator(anOutputStream, "Two-Line Elements");

    // tle_.print(anOutputStream, false);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Size SGP4::findClosestTleIndex(const Instant& anInstant) const
{
    using ostk::physics::time::Duration;

    const auto it = std::lower_bound(
        tleArray_.begin(),
        tleArray_.end(),
        anInstant,
        [](const TLE& tle, const Instant& instant)
        {
            return tle.getEpoch() < instant;
        }
    );

    if (it == tleArray_.begin())
    {
        return 0;
    }

    if (it == tleArray_.end())
    {
        return tleArray_.getSize() - 1;
    }

    const auto prevIt = std::prev(it);
    const Duration durationToPrev = Duration::Between(prevIt->getEpoch(), anInstant).getAbsolute();
    const Duration durationToNext = Duration::Between(anInstant, it->getEpoch()).getAbsolute();

    if (durationToPrev <= durationToNext)
    {
        return static_cast<Size>(std::distance(tleArray_.begin(), prevIt));
    }

    return static_cast<Size>(std::distance(tleArray_.begin(), it));
}

void SGP4::ensureImplForTleIndex(const Size& aTleIndex) const
{
    if (aTleIndex != cachedTleIndex_ || implUPtr_ == nullptr)
    {
        cachedTleIndex_ = aTleIndex;
        implUPtr_ = std::make_unique<SGP4::Impl>(tleArray_[aTleIndex], outputFrameSPtr_);
    }
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

}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
