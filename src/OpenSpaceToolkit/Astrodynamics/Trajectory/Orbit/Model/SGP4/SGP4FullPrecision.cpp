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

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/SGP4FullPrecision.hpp>

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
/// @details Vallado's reference SGP4 (see thirdparty/vallado-sgp4/README.md).
class SGP4FullPrecision::Impl
{
   public:
    Impl(const SGP4FullPrecision& aSGP4FullPrecision)
        : satelliteRecord_(),
          temeFrameSPtr_(Frame::TEME())
    {
        // Carried around by the propagator, never used by it.
        static const char satelliteNumber[] = "00000";

        SGP4Funcs::sgp4init(
            wgs72,  // The gravity model the TLE mean elements are defined against.
            'i',    // Improved operation mode rather than AFSPC: better sidereal time, no node wrap.
            satelliteNumber,
            Impl::EpochFromInstant(aSGP4FullPrecision.getEpoch()),
            aSGP4FullPrecision.getBStarDragTerm(),
            0.0,  // First derivative of the mean motion: carried by the TLE, unused by SGP4.
            0.0,  // Second derivative of the mean motion: idem.
            aSGP4FullPrecision.getEccentricity(),
            aSGP4FullPrecision.getAop().inRadians(0.0, Real::TwoPi()),
            aSGP4FullPrecision.getInclination().inRadians(0.0, Real::TwoPi()),
            aSGP4FullPrecision.getMeanAnomaly().inRadians(0.0, Real::TwoPi()),
            aSGP4FullPrecision.getMeanMotion().in(RadianPerMinute),
            aSGP4FullPrecision.getRaan().inRadians(0.0, Real::TwoPi()),
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

        // copy the satelliteRecord to keep it thread safe
        elsetrec satelliteRecord = this->satelliteRecord_;

        if (!SGP4Funcs::sgp4(satelliteRecord, durationFromEpoch_min, x_TEME_km, v_TEME_kmps))
        {
            throw ostk::core::error::RuntimeError(
                "Cannot propagate SGP4 to [{}]: {}.", anInstant.toString(), Impl::ErrorMessage(satelliteRecord.error)
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
    elsetrec satelliteRecord_;
    Shared<const Frame> temeFrameSPtr_;

    static double EpochFromInstant(const Instant& anInstant)
    {
        // Days since 1949-12-31 00:00 UT, which is what the TLE epoch field is defined
        // against too. Only the SGP4 initialization reads it.
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

SGP4FullPrecision::SGP4FullPrecision(
    const Instant& anEpoch,
    const Angle& anInclination,
    const Angle& aRaan,
    const Real& anEccentricity,
    const Angle& anAop,
    const Angle& aMeanAnomaly,
    const Derived& aMeanMotion,
    const Real& aBStarDragTerm,
    const Integer& aRevolutionNumberAtEpoch,
    const Shared<const Frame>& anOutputFrameSPtr
)
    : epoch_(anEpoch),
      inclination_(anInclination),
      raan_(aRaan),
      eccentricity_(anEccentricity),
      aop_(anAop),
      meanAnomaly_(aMeanAnomaly),
      meanMotion_(aMeanMotion),
      bStarDragTerm_(aBStarDragTerm),
      revolutionNumberAtEpoch_(aRevolutionNumberAtEpoch),
      outputFrameSPtr_(anOutputFrameSPtr),
      implSPtr_(nullptr)
{
    if (this->isDefined())
    {
        this->implSPtr_ = std::make_shared<const Impl>(*this);
    }
}

SGP4FullPrecision* SGP4FullPrecision::clone() const
{
    return new SGP4FullPrecision(*this);
}

bool SGP4FullPrecision::operator==(const SGP4FullPrecision& aSGP4FullPrecision) const
{
    if ((!this->isDefined()) || (!aSGP4FullPrecision.isDefined()))
    {
        return false;
    }

    return (epoch_ == aSGP4FullPrecision.epoch_) && (inclination_ == aSGP4FullPrecision.inclination_) &&
           (raan_ == aSGP4FullPrecision.raan_) && (eccentricity_ == aSGP4FullPrecision.eccentricity_) &&
           (aop_ == aSGP4FullPrecision.aop_) && (meanAnomaly_ == aSGP4FullPrecision.meanAnomaly_) &&
           (meanMotion_ == aSGP4FullPrecision.meanMotion_) && (bStarDragTerm_ == aSGP4FullPrecision.bStarDragTerm_) &&
           (revolutionNumberAtEpoch_ == aSGP4FullPrecision.revolutionNumberAtEpoch_) &&
           (*outputFrameSPtr_ == *aSGP4FullPrecision.outputFrameSPtr_);
}

bool SGP4FullPrecision::operator!=(const SGP4FullPrecision& aSGP4FullPrecision) const
{
    return !((*this) == aSGP4FullPrecision);
}

std::ostream& operator<<(std::ostream& anOutputStream, const SGP4FullPrecision& aSGP4FullPrecision)
{
    aSGP4FullPrecision.print(anOutputStream);

    return anOutputStream;
}

bool SGP4FullPrecision::isDefined() const
{
    return epoch_.isDefined() && inclination_.isDefined() && raan_.isDefined() && eccentricity_.isDefined() &&
           aop_.isDefined() && meanAnomaly_.isDefined() && meanMotion_.isDefined() && bStarDragTerm_.isDefined() &&
           revolutionNumberAtEpoch_.isDefined() && outputFrameSPtr_;
}

Instant SGP4FullPrecision::getEpoch() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4FullPrecision");
    }

    return epoch_;
}

Angle SGP4FullPrecision::getInclination() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4FullPrecision");
    }

    return inclination_;
}

Angle SGP4FullPrecision::getRaan() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4FullPrecision");
    }

    return raan_;
}

Real SGP4FullPrecision::getEccentricity() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4FullPrecision");
    }

    return eccentricity_;
}

Angle SGP4FullPrecision::getAop() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4FullPrecision");
    }

    return aop_;
}

Angle SGP4FullPrecision::getMeanAnomaly() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4FullPrecision");
    }

    return meanAnomaly_;
}

Derived SGP4FullPrecision::getMeanMotion() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4FullPrecision");
    }

    return meanMotion_;
}

Real SGP4FullPrecision::getBStarDragTerm() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4FullPrecision");
    }

    return bStarDragTerm_;
}

Integer SGP4FullPrecision::getRevolutionNumberAtEpoch() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4FullPrecision");
    }

    return revolutionNumberAtEpoch_;
}

Shared<const Frame> SGP4FullPrecision::getOutputFrame() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4FullPrecision");
    }

    return outputFrameSPtr_;
}

State SGP4FullPrecision::calculateStateAt(const Instant& anInstant) const
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4FullPrecision");
    }

    return this->implSPtr_->calculateStateAt(epoch_, anInstant, outputFrameSPtr_);
}

Array<State> SGP4FullPrecision::calculateStatesAt(const Array<Instant>& anInstantArray) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4FullPrecision");
    }

    const Impl& impl = *this->implSPtr_;

    Array<State> stateArray = Array<State>::Empty();
    stateArray.reserve(anInstantArray.getSize());

    for (const Instant& instant : anInstantArray)
    {
        if (!instant.isDefined())
        {
            throw ostk::core::error::runtime::Undefined("Instant");
        }

        stateArray.add(impl.calculateStateAt(epoch_, instant, outputFrameSPtr_));
    }

    return stateArray;
}

void SGP4FullPrecision::print(std::ostream& anOutputStream, bool displayDecorator) const
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
    ostk::core::utils::Print::Line(anOutputStream)
        << "Revolution Number:"
        << (revolutionNumberAtEpoch_.isDefined() ? revolutionNumberAtEpoch_.toString() : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

SGP4FullPrecision SGP4FullPrecision::Undefined()
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
        Integer::Undefined(),
    };
}

SGP4FullPrecision SGP4FullPrecision::FromTLE(const TLE& aTLE, const Shared<const Frame>& anOutputFrameSPtr)
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
        aTLE.getRevolutionNumberAtEpoch(),
        anOutputFrameSPtr,
    };
}

bool SGP4FullPrecision::operator==(const trajectory::Model& aModel) const
{
    const SGP4FullPrecision* aSGP4FullPrecisionPtr = dynamic_cast<const SGP4FullPrecision*>(&aModel);

    return (aSGP4FullPrecisionPtr != nullptr) && this->operator==(*aSGP4FullPrecisionPtr);
}

bool SGP4FullPrecision::operator!=(const trajectory::Model& aModel) const
{
    return !((*this) == aModel);
}

}  // namespace sgp4
}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
