/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Nadir.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace model
{

using ostk::core::type::String;

using ostk::physics::coordinate::Position;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

Nadir::Nadir(const Orbit& anOrbit, const Duration& aStepSize)
    : Model(),
      orbit_(anOrbit),
      stepSize_(aStepSize)
{
}

Nadir* Nadir::clone() const
{
    return new Nadir(*this);
}

bool Nadir::operator==(const Nadir& aNadirModel) const
{
    if ((!this->isDefined()) || (!aNadirModel.isDefined()))
    {
        return false;
    }

    return orbit_ == aNadirModel.orbit_;
}

bool Nadir::operator!=(const Nadir& aNadirModel) const
{
    return !((*this) == aNadirModel);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Nadir& aNadirModel)
{
    aNadirModel.print(anOutputStream);

    return anOutputStream;
}

bool Nadir::isDefined() const
{
    return orbit_.isDefined();
}

Orbit Nadir::getOrbit() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Nadir");
    }

    return orbit_;
}

Duration Nadir::getStepSize() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Nadir");
    }

    return stepSize_;
}

State Nadir::calculateStateAt(const Instant& anInstant) const
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Nadir");
    }

    // interpolate the velocity using polynomial interpolation

    const Array<Instant> instants = {
        anInstant - stepSize_ * 2,
        anInstant - stepSize_,
        anInstant,
        anInstant + stepSize_,
        anInstant + stepSize_ * 2,
    };

    VectorXd times(instants.getSize());
    MatrixXd positionCoordinates(3, instants.getSize());

    Position position = Position::Undefined();

    const Shared<const Frame> celestialBodyFrame = orbit_.accessCelestialObject()->accessFrame();

    Size i = 0;
    for (const auto& instant : instants)
    {
        times(i) = (instant - anInstant).inSeconds();

        const State state = orbit_.getStateAt(instant);

        const LLA lla =
            LLA::FromPosition(state.getPosition().inFrame(celestialBodyFrame, instant), orbit_.accessCelestialObject())
                .onSurface();

        const physics::coordinate::Position positionAtInstant =
            physics::coordinate::Position::FromLLA(lla, orbit_.accessCelestialObject()).inFrame(Frame::GCRF(), instant);

        if (instant == anInstant)
        {
            position = positionAtInstant;
        }

        positionCoordinates.col(i) = positionAtInstant.getCoordinates();

        ++i;
    }

    const Array<Shared<const Interpolator>> interpolators = {
        Interpolator::GenerateInterpolator(Interpolator::Type::CubicSpline, times, positionCoordinates.row(0)),
        Interpolator::GenerateInterpolator(Interpolator::Type::CubicSpline, times, positionCoordinates.row(1)),
        Interpolator::GenerateInterpolator(Interpolator::Type::CubicSpline, times, positionCoordinates.row(2)),
    };

    const Vector3d velocityCoordinates = {
        interpolators[0]->computeDerivative(0.0),
        interpolators[1]->computeDerivative(0.0),
        interpolators[2]->computeDerivative(0.0),
    };

    const Velocity velocity = Velocity::MetersPerSecond(velocityCoordinates, Frame::GCRF());

    return State(anInstant, position, velocity);
}

void Nadir::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Nadir") : void();

    orbit_.print(anOutputStream, false);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

bool Nadir::operator==(const Model& aModel) const
{
    const Nadir* staticModelPtr = dynamic_cast<const Nadir*>(&aModel);

    return (staticModelPtr != nullptr) && this->operator==(*staticModelPtr);
}

bool Nadir::operator!=(const Model& aModel) const
{
    return !((*this) == aModel);
}

}  // namespace model
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
