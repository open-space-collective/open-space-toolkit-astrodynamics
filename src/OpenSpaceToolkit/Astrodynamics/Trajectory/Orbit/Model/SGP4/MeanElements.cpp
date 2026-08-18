/// Apache License 2.0

#include <vallado-sgp4/SGP4.h>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/MeanElements.hpp>

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
namespace sgp4
{

using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::time::Scale;
using ostk::physics::unit::Time;

static const Derived::Unit RadianPerMinute = Derived::Unit::AngularVelocity(Angle::Unit::Radian, Time::Unit::Minute);

/// @brief Holds the vendored propagator, so it stays out of the public header.
///
/// @details Vallado's reference SGP4 (see thirdparty/vallado-sgp4/README.md). This is the
/// library's only SGP4: orbit::model::SGP4 decodes its TLE into a MeanElements and lands here
/// too, so the TLE format's rounding is applied once, when a TLE is written, and never sits
/// between an element set and its propagation.
class MeanElements::Impl
{
   public:
    Impl(const MeanElements& aMeanElements)
        : satelliteRecord_(),
          temeFrameSPtr_(Frame::TEME())
    {
        // Carried around by the propagator, never used by it.
        static const char satelliteNumber[] = "00000";

        SGP4Funcs::sgp4init(
            wgs72,  // The gravity model the TLE mean elements are defined against.
            'i',    // Improved operation mode rather than AFSPC: better sidereal time, no node wrap.
            satelliteNumber,
            Impl::EpochFromInstant(aMeanElements.getEpoch()),
            aMeanElements.getBStarDragTerm(),
            0.0,  // First derivative of the mean motion: carried by the TLE, unused by SGP4.
            0.0,  // Second derivative of the mean motion: idem.
            aMeanElements.getEccentricity(),
            aMeanElements.getAop().inRadians(0.0, Real::TwoPi()),
            aMeanElements.getInclination().inRadians(0.0, Real::TwoPi()),
            aMeanElements.getMeanAnomaly().inRadians(0.0, Real::TwoPi()),
            aMeanElements.getMeanMotion().in(RadianPerMinute),
            aMeanElements.getRaan().inRadians(0.0, Real::TwoPi()),
            this->satelliteRecord_
        );

        // sgp4init reports through the record, and ends by propagating to its own epoch, so
        // an element set that is already unpropagatable is caught here rather than later.
        if (this->satelliteRecord_.error != 0)
        {
            throw ostk::core::error::RuntimeError(
                "Cannot initialize SGP4 from the provided mean elements: {}.",
                Impl::ErrorMessage(this->satelliteRecord_.error)
            );
        }
    }

    State calculateStateAt(
        const Instant& anEpoch, const Instant& anInstant, const Shared<const Frame>& anOutputFrameSPtr
    ) const
    {
        const Real durationFromEpoch_min = Duration::Between(anEpoch, anInstant).inMinutes();

        double x_TEME_km[3] = {0.0, 0.0, 0.0};
        double v_TEME_kmps[3] = {0.0, 0.0, 0.0};

        // Deep-space runs carry an integrator state across calls, which is why the record is
        // mutable; sgp4() restarts it whenever the requested time is not a continuation. It
        // does mean a MeanElements -- and its copies, which share this -- is not reentrant.
        if (!SGP4Funcs::sgp4(this->satelliteRecord_, durationFromEpoch_min, x_TEME_km, v_TEME_kmps))
        {
            throw ostk::core::error::RuntimeError(
                "Cannot propagate SGP4 to [{}]: {}.",
                anInstant.toString(),
                Impl::ErrorMessage(this->satelliteRecord_.error)
            );
        }

        const Vector3d x_TEME_m = Vector3d(x_TEME_km[0], x_TEME_km[1], x_TEME_km[2]) * 1e3;
        const Vector3d v_TEME_mps = Vector3d(v_TEME_kmps[0], v_TEME_kmps[1], v_TEME_kmps[2]) * 1e3;

        const State state_TEME = {
            anInstant,
            Position(x_TEME_m, Position::Unit::Meter, this->temeFrameSPtr_),
            Velocity(v_TEME_mps, Velocity::Unit::MeterPerSecond, this->temeFrameSPtr_),
        };

        return state_TEME.inFrame(anOutputFrameSPtr);
    }

   private:
    mutable elsetrec satelliteRecord_;
    Shared<const Frame> temeFrameSPtr_;

    static double EpochFromInstant(const Instant& anInstant)
    {
        // Days since 1949-12-31 00:00 UT, which is what the TLE epoch field is defined
        // against too. Only the deep-space initialization reads it.
        static const double julianDateAtEpoch = 2433281.5;

        return anInstant.getJulianDate(Scale::UTC) - julianDateAtEpoch;
    }

    static String ErrorMessage(const int anErrorCode)
    {
        switch (anErrorCode)
        {
            case 1:
                return "mean eccentricity out of [0, 1) or mean semi-major axis below 0.95 Earth radii";
            case 2:
                return "mean motion below zero";
            case 3:
                return "perturbed eccentricity out of [0, 1)";
            case 4:
                return "semi-latus rectum below zero";
            case 5:
                return "epoch elements are sub-orbital";
            case 6:
                return "satellite has decayed";
            default:
                return String::Format("error code [{}]", anErrorCode);
        }
    }
};

MeanElements::MeanElements(
    const Instant& anEpoch,
    const Angle& anInclination,
    const Angle& aRaan,
    const Real& anEccentricity,
    const Angle& anAop,
    const Angle& aMeanAnomaly,
    const Derived& aMeanMotion,
    const Real& aBStarDragTerm
)
    : epoch_(anEpoch),
      inclination_(anInclination),
      raan_(aRaan),
      eccentricity_(anEccentricity),
      aop_(anAop),
      meanAnomaly_(aMeanAnomaly),
      meanMotion_(aMeanMotion),
      bStarDragTerm_(aBStarDragTerm),
      implSPtr_(nullptr)
{
}

bool MeanElements::operator==(const MeanElements& aMeanElements) const
{
    if ((!this->isDefined()) || (!aMeanElements.isDefined()))
    {
        return false;
    }

    return (epoch_ == aMeanElements.epoch_) && (inclination_ == aMeanElements.inclination_) &&
           (raan_ == aMeanElements.raan_) && (eccentricity_ == aMeanElements.eccentricity_) &&
           (aop_ == aMeanElements.aop_) && (meanAnomaly_ == aMeanElements.meanAnomaly_) &&
           (meanMotion_ == aMeanElements.meanMotion_) && (bStarDragTerm_ == aMeanElements.bStarDragTerm_);
}

bool MeanElements::operator!=(const MeanElements& aMeanElements) const
{
    return !((*this) == aMeanElements);
}

std::ostream& operator<<(std::ostream& anOutputStream, const MeanElements& aMeanElements)
{
    aMeanElements.print(anOutputStream);

    return anOutputStream;
}

bool MeanElements::isDefined() const
{
    return epoch_.isDefined() && inclination_.isDefined() && raan_.isDefined() && eccentricity_.isDefined() &&
           aop_.isDefined() && meanAnomaly_.isDefined() && meanMotion_.isDefined() && bStarDragTerm_.isDefined();
}

Instant MeanElements::getEpoch() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MeanElements");
    }

    return epoch_;
}

Angle MeanElements::getInclination() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MeanElements");
    }

    return inclination_;
}

Angle MeanElements::getRaan() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MeanElements");
    }

    return raan_;
}

Real MeanElements::getEccentricity() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MeanElements");
    }

    return eccentricity_;
}

Angle MeanElements::getAop() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MeanElements");
    }

    return aop_;
}

Angle MeanElements::getMeanAnomaly() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MeanElements");
    }

    return meanAnomaly_;
}

Derived MeanElements::getMeanMotion() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MeanElements");
    }

    return meanMotion_;
}

Real MeanElements::getBStarDragTerm() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MeanElements");
    }

    return bStarDragTerm_;
}

State MeanElements::calculateStateAt(const Instant& anInstant, const Shared<const Frame>& anOutputFrameSPtr) const
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (anOutputFrameSPtr == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Output frame");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MeanElements");
    }

    return this->accessImpl().calculateStateAt(epoch_, anInstant, anOutputFrameSPtr);
}

Array<State> MeanElements::calculateStatesAt(
    const Array<Instant>& anInstantArray, const Shared<const Frame>& anOutputFrameSPtr
) const
{
    if (anOutputFrameSPtr == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Output frame");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MeanElements");
    }

    const Impl& impl = this->accessImpl();

    Array<State> stateArray = Array<State>::Empty();
    stateArray.reserve(anInstantArray.getSize());

    for (const Instant& instant : anInstantArray)
    {
        if (!instant.isDefined())
        {
            throw ostk::core::error::runtime::Undefined("Instant");
        }

        stateArray.add(impl.calculateStateAt(epoch_, instant, anOutputFrameSPtr));
    }

    return stateArray;
}

void MeanElements::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "SGP4 Mean Elements") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Epoch:" << (epoch_.isDefined() ? epoch_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Inclination:" << (inclination_.isDefined() ? inclination_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream) << "RAAN:" << (raan_.isDefined() ? raan_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Eccentricity:" << (eccentricity_.isDefined() ? eccentricity_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream) << "AoP:" << (aop_.isDefined() ? aop_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Mean anomaly:" << (meanAnomaly_.isDefined() ? meanAnomaly_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Mean motion:" << (meanMotion_.isDefined() ? meanMotion_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "B*:" << (bStarDragTerm_.isDefined() ? bStarDragTerm_.toString() : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

MeanElements MeanElements::Undefined()
{
    return {
        Instant::Undefined(),
        Angle::Undefined(),
        Angle::Undefined(),
        Real::Undefined(),
        Angle::Undefined(),
        Angle::Undefined(),
        Derived::Undefined(),
        Real::Undefined(),
    };
}

MeanElements MeanElements::FromTLE(const TLE& aTLE)
{
    if (!aTLE.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return {
        aTLE.getEpoch(),
        aTLE.getInclination(),
        aTLE.getRaan(),
        aTLE.getEccentricity(),
        aTLE.getAop(),
        aTLE.getMeanAnomaly(),
        aTLE.getMeanMotion(),
        aTLE.getBStarDragTerm(),
    };
}

const MeanElements::Impl& MeanElements::accessImpl() const
{
    if (implSPtr_ == nullptr)
    {
        implSPtr_ = std::make_shared<const Impl>(*this);
    }

    return *implSPtr_;
}

}  // namespace sgp4
}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
