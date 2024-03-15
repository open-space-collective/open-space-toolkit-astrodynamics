/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;
using ostk::core::type::Real;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::Dynamics;
using TabulatedDynamics = ostk::astrodynamics::dynamics::Tabulated;
using ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_Validation_ManeuverValidation : public ::testing::Test
{
   protected:
    const Array<Instant> defaultInstants_ = {
        Instant::J2000(),
        Instant::J2000() + Duration::Seconds(1.0),
        Instant::J2000() + Duration::Seconds(5.0),
        Instant::J2000() + Duration::Seconds(7.0),
    };

    const Array<Vector3d> defaultAccelerationProfileDefaultFrame_ = {
        {1.0e-3, 0.0e-3, 0.0e-3},
        {0.0e-3, 1.0e-3, 0.0e-3},
        {0.0e-3, 0.0e-3, 1.0e-3},
        {1.0e-3, 1.0e-3, 1.0e-3},
    };

    const Shared<const Frame> defaultFrameSPtr_ = Frame::GCRF();
    const Shared<const Frame> secondFrameSPtr_ = Frame::ITRF();

    const Array<Real> defaultMassFlowRateProfile_ = {-1.0e-5, -1.1e-5, -0.9e-5, -1.0e-5};
    const Real defaultConstantMassFlowRate_ = -0.5e-5;

    const Maneuver defaultManeuver_ = {
        defaultInstants_, defaultAccelerationProfileDefaultFrame_, defaultFrameSPtr_, defaultMassFlowRateProfile_
    };
};

// TBI: add safeguards, test the limits of interpolation, and test the limits of the frame
