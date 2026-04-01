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

using ostk::core::container::Pair;

using ostk::physics::coordinate::Transform;
using ostk::physics::time::Interval;

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
      tleArray_({aTle}),
      validityIntervals_(),
      outputFrameSPtr_(Frame::GCRF()),
      implUPtr_(std::make_unique<SGP4::Impl>(aTle, outputFrameSPtr_)),
      cachedTleIndex_(0)
{
}

SGP4::SGP4(const TLE& aTle, const Shared<const Frame>& anOutputFrameSPtr)
    : Model(),
      tleArray_({aTle}),
      validityIntervals_(),
      outputFrameSPtr_(anOutputFrameSPtr),
      implUPtr_(std::make_unique<SGP4::Impl>(aTle, outputFrameSPtr_)),
      cachedTleIndex_(0)
{
}

SGP4::SGP4(const Array<TLE>& aTleArray, const Shared<const Frame>& anOutputFrameSPtr)
    : Model(),
      tleArray_(aTleArray),
      validityIntervals_(),
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

    validityIntervals_ = SGP4::GenerateIntervalsFromEpochs(tleArray_);

    implUPtr_ = std::make_unique<SGP4::Impl>(tleArray_[cachedTleIndex_], outputFrameSPtr_);
}

SGP4::SGP4(const Array<Pair<TLE, Interval>>& aTleIntervalArray, const Shared<const Frame>& anOutputFrameSPtr)
    : Model(),
      tleArray_(),
      validityIntervals_(),
      outputFrameSPtr_(anOutputFrameSPtr),
      implUPtr_(nullptr),
      cachedTleIndex_(0)
{
    if (aTleIntervalArray.isEmpty())
    {
        throw ostk::core::error::RuntimeError("TLE-Interval array is empty.");
    }

    // Build parallel arrays and sort by TLE epoch
    Array<Pair<TLE, Interval>> sortedPairs = aTleIntervalArray;
    std::sort(
        sortedPairs.begin(),
        sortedPairs.end(),
        [](const Pair<TLE, Interval>& a, const Pair<TLE, Interval>& b)
        {
            return a.first.getEpoch() < b.first.getEpoch();
        }
    );

    for (const auto& pair : sortedPairs)
    {
        tleArray_.add(pair.first);
        validityIntervals_.add(pair.second);
    }

    implUPtr_ = std::make_unique<SGP4::Impl>(tleArray_[cachedTleIndex_], outputFrameSPtr_);
}

SGP4::SGP4(const SGP4& aSGP4Model)
    : Model(aSGP4Model),
      tleArray_(aSGP4Model.tleArray_),
      validityIntervals_(aSGP4Model.validityIntervals_),
      outputFrameSPtr_(aSGP4Model.outputFrameSPtr_),
      implUPtr_(nullptr),
      cachedTleIndex_(0)
{
    implUPtr_ = std::make_unique<SGP4::Impl>(tleArray_[cachedTleIndex_], outputFrameSPtr_);
}

SGP4::~SGP4() {}

SGP4& SGP4::operator=(const SGP4& aSGP4Model)
{
    if (this != &aSGP4Model)
    {
        Model::operator=(aSGP4Model);

        this->tleArray_ = aSGP4Model.tleArray_;
        this->validityIntervals_ = aSGP4Model.validityIntervals_;
        this->outputFrameSPtr_ = aSGP4Model.outputFrameSPtr_;
        this->cachedTleIndex_ = 0;

        if (!tleArray_.isEmpty())
        {
            this->implUPtr_ = std::make_unique<SGP4::Impl>(tleArray_[cachedTleIndex_], outputFrameSPtr_);
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

    return (this->tleArray_ == aSGP4Model.tleArray_) && (this->validityIntervals_ == aSGP4Model.validityIntervals_) &&
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
    return (!this->tleArray_.isEmpty()) && (this->implUPtr_ != nullptr);
}

TLE SGP4::getTle() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4");
    }

    if (this->tleArray_.getSize() > 1)
    {
        throw ostk::core::error::RuntimeError("Multiple TLEs exist in the model. Use getTles() instead.");
    }

    return this->tleArray_.accessFirst();
}

Array<TLE> SGP4::getTles() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4");
    }

    return this->tleArray_;
}

Array<Interval> SGP4::getValidityIntervals() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4");
    }

    return this->validityIntervals_;
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

    if (this->tleArray_.getSize() > 1)
    {
        throw ostk::core::error::RuntimeError("Multiple TLEs exist in the model. Cannot get epoch.");
    }

    return this->tleArray_.accessFirst().getEpoch();
}

Integer SGP4::getRevolutionNumberAtEpoch() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4");
    }

    if (this->tleArray_.getSize() > 1)
    {
        throw ostk::core::error::RuntimeError("Multiple TLEs exist in the model. Cannot get revolution number at epoch."
        );
    }

    return this->tleArray_.accessFirst().getRevolutionNumberAtEpoch();
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
        const Size tleIndex = this->findTleIndexForInstant(anInstant);
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

    if (this->isDefined() && tleArray_.getSize() == 1)
    {
        ostk::core::utils::Print::Line(anOutputStream) << "Epoch:" << this->getEpoch().toString();
    }
    else
    {
        ostk::core::utils::Print::Line(anOutputStream) << "Epoch:"
                                                       << "Undefined";
    }

    if (!tleArray_.isEmpty())
    {
        ostk::core::utils::Print::Line(anOutputStream) << "TLE count:" << tleArray_.getSize();
    }

    ostk::core::utils::Print::Separator(anOutputStream, "Two-Line Elements");

    for (const auto& tle : tleArray_)
    {
        tle.print(anOutputStream, false);
    }

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Size SGP4::findTleIndexForInstant(const Instant& anInstant) const
{
    for (Size i = 0; i < validityIntervals_.getSize(); ++i)
    {
        if (validityIntervals_[i].contains(anInstant))
        {
            return i;
        }
    }

    throw ostk::core::error::RuntimeError(
        "No TLE validity interval contains the requested instant [{}].", anInstant.toString()
    );
}

Array<Interval> SGP4::GenerateIntervalsFromEpochs(const Array<TLE>& aTleArray)
{
    using ostk::physics::time::Duration;

    Array<Interval> intervals = Array<Interval>::Empty();
    intervals.reserve(aTleArray.getSize());

    if (aTleArray.getSize() == 1)
    {
        // Single TLE: interval spans far past to far future
        const Instant farPast = aTleArray[0].getEpoch() - Duration::Days(36525.0);
        const Instant farFuture = aTleArray[0].getEpoch() + Duration::Days(36525.0);
        intervals.add(Interval::Closed(farPast, farFuture));
        return intervals;
    }

    for (Size i = 0; i < aTleArray.getSize(); ++i)
    {
        Instant start = Instant::Undefined();
        Instant end = Instant::Undefined();

        if (i == 0)
        {
            // First TLE: start at far past
            start = aTleArray[0].getEpoch() - Duration::Days(36525.0);
        }
        else
        {
            // Midpoint between previous and current epoch
            const Duration halfGap = Duration::Between(aTleArray[i - 1].getEpoch(), aTleArray[i].getEpoch()) / 2.0;
            start = aTleArray[i - 1].getEpoch() + halfGap;
        }

        if (i == aTleArray.getSize() - 1)
        {
            // Last TLE: end at far future
            end = aTleArray[i].getEpoch() + Duration::Days(36525.0);
        }
        else
        {
            // Midpoint between current and next epoch
            const Duration halfGap = Duration::Between(aTleArray[i].getEpoch(), aTleArray[i + 1].getEpoch()) / 2.0;
            end = aTleArray[i].getEpoch() + halfGap;
        }

        // Use HalfOpenRight [start, end) for all except the last, which is Closed [start, end]
        if (i < aTleArray.getSize() - 1)
        {
            intervals.add(Interval::HalfOpenRight(start, end));
        }
        else
        {
            intervals.add(Interval::Closed(start, end));
        }
    }

    return intervals;
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
