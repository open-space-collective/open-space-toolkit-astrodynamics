/// Apache License 2.0

#include <iostream>

#include <sgp4/SGP4.h>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>

#include <heyoka/model/sgp4.hpp>

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

    return state_TEME.inFrame(Frame::GCRF());
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

Array<State> SGP4::calculateStatesAt(const Array<Instant>& anInstantArray) const
{
    using ostk::core::type::Real;
    using ostk::mathematics::object::Vector3d;
    using ostk::physics::coordinate::Frame;
    using ostk::physics::coordinate::Position;
    using ostk::physics::coordinate::Velocity;
    using ostk::physics::time::Scale;

    if (anInstantArray.isEmpty())
    {
        return Array<State>::Empty();
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SGP4");
    }

    // Prepare inputs for heyoka
    const Real meanMotion =
        this->tle_.getMeanMotion()
            .in(Derived::Unit::AngularVelocity(Angle::Unit::Radian, Time::Unit::Day));  // rad/day
    const Real eccentricity = this->tle_.getEccentricity();
    const Real inclination = this->tle_.getInclination().inRadians();
    const Real raan = this->tle_.getRaan().inRadians();
    const Real aop = this->tle_.getAop().inRadians();
    const Real meanAnomaly = this->tle_.getMeanAnomaly().inRadians();
    const Real bStar = this->tle_.getBStarDragTerm();
    const Real julianDay = this->tle_.getEpoch().getJulianDate(Scale::UTC);
    const Real correction = 0.0;

    // Construct the input data vector (flattened 9x1 array, but heyoka uses SoA so if 1 sat, just 9 elements)
    // The order is: mean_motion, eccentricity, inclination, raan, aop, mean_anomaly, b_star, julian_day, correction
    std::vector<double> tleData = {
        meanMotion, eccentricity, inclination, raan, aop, meanAnomaly, bStar, julianDay, correction};

    // Create propagator
    heyoka::model::sgp4_propagator<double> propagator(tleData, 1);

    // Prepare times (Minutes since epoch)
    // heyoka interprets floating point arrays as minutes since epoch
    std::vector<double> dates;
    dates.reserve(anInstantArray.getSize());

    const Instant epoch = this->tle_.getEpoch();

    for (const auto& instant : anInstantArray)
    {
        dates.push_back(ostk::physics::time::Duration::Between(epoch, instant).inMinutes());
    }

    // Output buffers
    // heyoka expects pre-allocated output vectors or resizes them.
    // Based on typical C++ output arguments, we pass empty vectors and expect heyoka to fill them.
    std::vector<double> outPos;
    std::vector<double> outVel;

    // Propagate
    // The heyoka C++ API for sgp4_propagator typically uses the call operator.
    propagator(dates, outPos, outVel);

    // Convert results back to States
    Array<State> states = Array<State>::Empty();
    states.reserve(anInstantArray.getSize());

    Shared<const Frame> temeFrame = Frame::TEME();
    Shared<const Frame> gcrfFrame = Frame::GCRF();

    for (size_t i = 0; i < anInstantArray.getSize(); ++i)
    {
        // For a single satellite in batch mode, output is organized by date.
        // outPos: x(t0), y(t0), z(t0), x(t1), y(t1), z(t1), ...
        // outVel: vx(t0), vy(t0), vz(t0), vx(t1), vy(t1), vz(t1), ...

        const double x_km = outPos[3 * i + 0];
        const double y_km = outPos[3 * i + 1];
        const double z_km = outPos[3 * i + 2];

        const double vx_kmps = outVel[3 * i + 0];
        const double vy_kmps = outVel[3 * i + 1];
        const double vz_kmps = outVel[3 * i + 2];

        Vector3d position_TEME_m(x_km * 1000.0, y_km * 1000.0, z_km * 1000.0);
        Vector3d velocity_TEME_mps(vx_kmps * 1000.0, vy_kmps * 1000.0, vz_kmps * 1000.0);

        Position position(position_TEME_m, Position::Unit::Meter, temeFrame);
        Velocity velocity(velocity_TEME_mps, Velocity::Unit::MeterPerSecond, temeFrame);

        State stateTEME(anInstantArray[i], position, velocity);
        states.add(stateTEME.inFrame(gcrfFrame));
    }

    return states;
}

void SGP4::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "SGP4") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Epoch:" << (this->getEpoch().isDefined() ? this->getEpoch().toString() : "Undefined");

    ostk::core::utils::Print::Separator(anOutputStream, "Two-Line Elements");

    // tle_.print(anOutputStream, false);

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

}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
