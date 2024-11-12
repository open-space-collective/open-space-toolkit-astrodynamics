/// Apache License 2.0

#include <chrono>
#include <nlopt.hpp>

#include <OpenSpaceToolkit/Core/Container/Triple.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Segment.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solver/TemporalConditionSolver.hpp>

using ostk::core::container::Map;
using ostk::core::container::Triple;
using ostk::core::type::Real;

using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::geometry::d3::object::Segment;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::MatrixXi;
using ostk::mathematics::object::Vector2d;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;
using ostk::mathematics::object::VectorXi;
using ArrayXb = Eigen::Array<bool, Eigen::Dynamic, 1>;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::environment::Object;
using ostk::physics::environment::object::celestial::Earth;

using ostk::astrodynamics::RootSolver;
using ostk::astrodynamics::solver::TemporalConditionSolver;

namespace ostk
{
namespace astrodynamics
{
namespace access
{

AccessTarget::Type AccessTarget::getType() const
{
    return type_;
}

Constraint AccessTarget::getConstraint() const
{
    return constraint_;
}

Trajectory AccessTarget::getTrajectory() const
{
    return trajectory_;
}

Position AccessTarget::getPosition() const
{
    if (type_ != Type::Fixed)
    {
        throw ostk::core::error::RuntimeError("Position is only defined for fixed targets.");
    }

    return trajectory_.getStateAt(Instant::J2000()).inFrame(Frame::ITRF()).getPosition();
}

LLA AccessTarget::getLLA(const Shared<const Celestial>& aCelestialSPtr) const
{
    return LLA::Cartesian(
        getPosition().accessCoordinates(), aCelestialSPtr->getEquatorialRadius(), aCelestialSPtr->getFlattening()
    );
}

Matrix3d AccessTarget::computeR_SEZ_ECEF(const Shared<const Celestial>& aCelestialSPtr) const
{
    const LLA lla = this->getLLA(aCelestialSPtr);

    // TBM: Move this to OSTk physics as a utility function
    const double sinLat = std::sin(lla.getLatitude().inRadians());
    const double cosLat = std::cos(lla.getLatitude().inRadians());
    const double sinLon = std::sin(lla.getLongitude().inRadians());
    const double cosLon = std::cos(lla.getLongitude().inRadians());

    Matrix3d SEZRotation;
    SEZRotation << sinLat * cosLon, sinLat * sinLon, -cosLat, -sinLon, cosLon, 0.0, cosLat * cosLon, cosLat * sinLon,
        sinLat;

    return SEZRotation;
}

AccessTarget AccessTarget::FromLLA(
    const Constraint& constraint, const LLA& anLLA, const Shared<const Celestial>& aCelestialSPtr
)
{
    return AccessTarget(
        AccessTarget::Type::Fixed,
        constraint,
        Trajectory::Position(Position::Meters(
            anLLA.toCartesian(aCelestialSPtr->getEquatorialRadius(), aCelestialSPtr->getFlattening()), Frame::ITRF()
        ))
    );
}

AccessTarget AccessTarget::FromPosition(const Constraint& constraint, const Position& aPosition)
{
    return AccessTarget(AccessTarget::Type::Fixed, constraint, Trajectory::Position(aPosition));
}

AccessTarget AccessTarget::FromTrajectory(const Constraint& constraint, const Trajectory& aTrajectory)
{
    return AccessTarget(AccessTarget::Type::Trajectory, constraint, aTrajectory);
}

AccessTarget::AccessTarget(
    const AccessTarget::Type& aType, const Constraint& aConstraint, const Trajectory& aTrajectory
)
    : type_(aType),
      constraint_(aConstraint),
      trajectory_(aTrajectory)
{
    if (!aTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Trajectory");
    }
}

Generator::Generator(
    const Environment& anEnvironment,
    const Duration& aStep,
    const Duration& aTolerance,
    const std::function<bool(const Access&)>& anAccessFilter,
    const std::function<bool(const State&, const State&)>& aStateFilter
)
    : environment_(anEnvironment),
      step_(aStep),
      tolerance_(aTolerance),
      accessFilter_(anAccessFilter),
      stateFilter_(aStateFilter)
{
}

bool Generator::isDefined() const
{
    return this->environment_.isDefined() && this->step_.isDefined() && this->tolerance_.isDefined();
}

Duration Generator::getStep() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return this->step_;
}

Duration Generator::getTolerance() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return this->tolerance_;
}

std::function<bool(const Access&)> Generator::getAccessFilter() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return this->accessFilter_;
}

std::function<bool(const State&, const State&)> Generator::getStateFilter() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return this->stateFilter_;
}

std::function<bool(const Instant&)> Generator::getConditionFunction(
    const AccessTarget& anAccessTarget, const Trajectory& aToTrajectory
) const
{
    const Trajectory aFromTrajectory = anAccessTarget.getTrajectory();

    if (!aFromTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("From Trajectory");
    }

    if (!aToTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("To Trajectory");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    const Constraint constraint = anAccessTarget.getConstraint();

    GeneratorContext generatorContext = GeneratorContext(aFromTrajectory, aToTrajectory, environment_, *this);

    return [generatorContext, constraint](const Instant& anInstant) mutable -> bool
    {
        return generatorContext.isAccessActive(anInstant, constraint);
    };
}

Array<Access> Generator::computeAccesses(
    const physics::time::Interval& anInterval, const AccessTarget& anAccessTarget, const Trajectory& aToTrajectory
) const
{
    if (!anInterval.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Interval");
    }

    if (anAccessTarget.getType() == AccessTarget::Type::Trajectory)
    {
        return this->computeAccessesForTrajectoryTarget(anInterval, anAccessTarget, aToTrajectory);
    }
    else
    {
        return this->computeAccessesForFixedTargets(anInterval, Array<AccessTarget> {anAccessTarget}, aToTrajectory)[0];
    }
}

Array<Array<Access>> Generator::computeAccesses(
    const physics::time::Interval& anInterval,
    const Array<AccessTarget>& someAccessTargets,
    const Trajectory& aToTrajectory
) const
{
    if (!anInterval.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Interval");
    }

    if (someAccessTargets.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Access targets");
    }

    if (std ::all_of(
            someAccessTargets.begin(),
            someAccessTargets.end(),
            [](const auto& accessTarget)
            {
                return accessTarget.getType() == AccessTarget::Type::Trajectory;
            }
        ))
    {
        Array<Array<Access>> accessesPerTarget;
        accessesPerTarget.reserve(someAccessTargets.getSize());

        for (const auto& groundTargetConfiguration : someAccessTargets)
        {
            accessesPerTarget.add(
                this->computeAccessesForTrajectoryTarget(anInterval, groundTargetConfiguration, aToTrajectory)
            );
        }

        return accessesPerTarget;
    }
    else if (std::all_of(
                 someAccessTargets.begin(),
                 someAccessTargets.end(),
                 [](const auto& accessTarget)
                 {
                     return accessTarget.getType() == AccessTarget::Type::Fixed;
                 }
             ))
    {
        return this->computeAccessesForFixedTargets(anInterval, someAccessTargets, aToTrajectory);
    }
    else
    {
        throw ostk::core::error::RuntimeError("All targets must be of same type.");
    }
}

void Generator::setStep(const Duration& aStep)
{
    if (!aStep.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Step");
    }

    this->step_ = aStep;
}

void Generator::setTolerance(const Duration& aTolerance)
{
    if (!aTolerance.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tolerance");
    }

    this->tolerance_ = aTolerance;
}

void Generator::setAccessFilter(const std::function<bool(const Access&)>& anAccessFilter)
{
    this->accessFilter_ = anAccessFilter;
}

void Generator::setStateFilter(const std::function<bool(const State&, const State&)>& aStateFilter)
{
    this->stateFilter_ = aStateFilter;
}

Generator Generator::Undefined()
{
    return {Environment::Undefined(), Duration::Undefined(), Duration::Undefined()};
}

Array<Access> Generator::computeAccessesForTrajectoryTarget(
    const physics::time::Interval& anInterval, const AccessTarget& anAccessTarget, const Trajectory& aToTrajectory
) const
{
    const TemporalConditionSolver temporalConditionSolver = {this->step_, this->tolerance_};

    const Array<physics::time::Interval> accessIntervals =
        temporalConditionSolver.solve(this->getConditionFunction(anAccessTarget, aToTrajectory), anInterval);

    const Trajectory aFromTrajectory = anAccessTarget.getTrajectory();

    return generateAccessesFromIntervals(accessIntervals, anInterval, aFromTrajectory, aToTrajectory);
}

Array<Array<Access>> Generator::computeAccessesForFixedTargets(
    const physics::time::Interval& anInterval,
    const Array<AccessTarget>& someAccessTargets,
    const Trajectory& aToTrajectory,
    const bool& coarse
) const
{
    if (stateFilter_)
    {
        throw ostk::core::error::RuntimeError("State filter is not supported for multiple ground targets.");
    }

    // create a stacked matrix of SEZ rotations for all ground targets
    const Index targetCount = someAccessTargets.getSize();
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> SEZRotations(3, 3 * targetCount);

    const Shared<const Celestial> earthSPtr = this->environment_.accessCelestialObjectWithName("Earth");

    for (Index i = 0; i < targetCount; ++i)
    {
        SEZRotations.block<3, 3>(0, 3 * i) = someAccessTargets[i].computeR_SEZ_ECEF(earthSPtr);
    }

    // create a stacked matrix of ITRF positions for all ground targets
    MatrixXd fromPositionCoordinates_ITRF = MatrixXd::Zero(3, targetCount);

    for (Index i = 0; i < targetCount; ++i)
    {
        fromPositionCoordinates_ITRF.col(i) = someAccessTargets[i].getPosition().getCoordinates();
    }

    const bool allAccessTargetsHaveMasks = std::all_of(
        someAccessTargets.begin(),
        someAccessTargets.end(),
        [](const auto& accessTarget)
        {
            return accessTarget.getConstraint().isMaskBased();
        }
    );

    std::function<ArrayXb(const VectorXd&, const VectorXd&, const VectorXd&)> aerFilter;

    if (!allAccessTargetsHaveMasks)
    {
        // create a stacked matrix of azimuth, elevation, and range bounds for all ground targets
        MatrixXd aerLowerBounds = MatrixXd::Zero(targetCount, 3);
        MatrixXd aerUpperBounds = MatrixXd::Zero(targetCount, 3);

        const Real degToRad = Real::Pi() / 180.0;

        for (Index i = 0; i < targetCount; ++i)
        {
            const Constraint::IntervalConstraint intervalConstraint =
                someAccessTargets[i].getConstraint().getIntervalConstraint().value();

            aerLowerBounds(i, 0) = intervalConstraint.azimuth.accessLowerBound() * degToRad;
            aerLowerBounds(i, 1) = intervalConstraint.elevation.accessLowerBound() * degToRad;
            aerLowerBounds(i, 2) = intervalConstraint.range.accessLowerBound();

            aerUpperBounds(i, 0) = intervalConstraint.azimuth.accessUpperBound() * degToRad;
            aerUpperBounds(i, 1) = intervalConstraint.elevation.accessUpperBound() * degToRad;
            aerUpperBounds(i, 2) = intervalConstraint.range.accessUpperBound();
        }

        aerFilter = [aerLowerBounds,
                     aerUpperBounds](const VectorXd& azimuths, const VectorXd& elevations, const VectorXd& ranges)
        {
            return (azimuths.array() > aerLowerBounds.col(0).array() &&
                    azimuths.array() < aerUpperBounds.col(0).array() &&
                    elevations.array() > aerLowerBounds.col(1).array() &&
                    elevations.array() < aerUpperBounds.col(1).array() &&
                    ranges.array() > aerLowerBounds.col(2).array() && ranges.array() < aerUpperBounds.col(2).array())
                .eval();
        };
    }
    else
    {
        VectorXd rangeLowerBounds = VectorXd::Zero(targetCount, 1);
        VectorXd rangeUpperBounds = VectorXd::Zero(targetCount, 1);

        for (Index i = 0; i < targetCount; ++i)
        {
            const Constraint::MaskConstraint constraint =
                someAccessTargets[i].getConstraint().getMaskConstraint().value();

            rangeLowerBounds(i) = constraint.range.accessLowerBound();
            rangeUpperBounds(i) = constraint.range.accessUpperBound();
        }

        aerFilter = [&someAccessTargets, rangeLowerBounds, rangeUpperBounds](
                        const VectorXd& azimuths_rad, const VectorXd& elevations_rad, const VectorXd& ranges_m
                    )
        {
            ArrayXb mask(azimuths_rad.rows());
            mask = ranges_m.array() > rangeLowerBounds.array() && ranges_m.array() < rangeUpperBounds.array();

            for (Eigen::Index i = 0; i < mask.rows(); ++i)
            {
                const Constraint::MaskConstraint constraint =
                    someAccessTargets[i].getConstraint().getMaskConstraint().value();

                const double& azimuth_rad = azimuths_rad(i);
                const double& elevation_rad = elevations_rad(i);

                const AER aer =
                    AER(Angle::Radians(azimuth_rad), Angle::Radians(elevation_rad), Length::Meters(ranges_m(i)));

                mask(i) = mask(i) && constraint.isSatisfied(aer);
            }

            return mask;
        };
    }

    const Array<Instant> instants = anInterval.generateGrid(this->step_);

    MatrixXi inAccessPerTarget = MatrixXi::Zero(instants.getSize(), targetCount);

    for (Index index = 0; index < instants.getSize(); ++index)
    {
        const Instant& instant = instants[index];

        // calculate target to satellite vector in ITRF
        const Vector3d toPositionCoordinates_ITRF =
            aToTrajectory.getStateAt(instant).inFrame(Frame::ITRF()).getPosition().getCoordinates();

        const MatrixXd dx = (-fromPositionCoordinates_ITRF).colwise() + toPositionCoordinates_ITRF;

        // calculate target to satellite vector in SEZ
        MatrixXd dx_SEZ = MatrixXd::Zero(3, dx.cols());
        for (Eigen::Index i = 0; i < dx.cols(); ++i)
        {
            dx_SEZ.col(i) = SEZRotations.block<3, 3>(0, 3 * i) * dx.col(i);
        }

        // calculate azimuth, elevation, and range
        const VectorXd range_m = dx_SEZ.colwise().norm();
        const VectorXd elevation_rad = (dx_SEZ.row(2).transpose().array() / range_m.array()).asin();
        VectorXd azimuth_rad = dx_SEZ.row(0).array().binaryExpr(
            dx_SEZ.row(1).array(),
            [](double x, double y)
            {
                return std::atan2(y, -x);
            }
        );

        azimuth_rad = azimuth_rad.unaryExpr(
            [](double x)
            {
                return x < 0.0 ? x + 2.0 * M_PI : x;
            }
        );

        // check if satellite is in access
        const auto inAccess = aerFilter(azimuth_rad, elevation_rad, range_m);
        inAccessPerTarget.row(index) = inAccess.cast<int>().transpose();
    }

    Array<Array<physics::time::Interval>> accessIntervalsPerTarget =
        Array<Array<physics::time::Interval>>(targetCount, Array<physics::time::Interval>::Empty());

    for (Index i = 0; i < targetCount; ++i)
    {
        accessIntervalsPerTarget[i] = ComputeIntervals(inAccessPerTarget.col(i), instants);
    }

    if (!coarse)
    {
        for (Index i = 0; i < accessIntervalsPerTarget.getSize(); ++i)
        {
            accessIntervalsPerTarget[i] = this->computePreciseCrossings(
                accessIntervalsPerTarget[i],
                anInterval,
                fromPositionCoordinates_ITRF.col(i),
                aToTrajectory,
                someAccessTargets[i]
            );
        }
    }

    Array<Array<Access>> accesses = Array<Array<Access>>(targetCount, Array<Access>::Empty());
    for (Index i = 0; i < accessIntervalsPerTarget.getSize(); ++i)
    {
        const Trajectory fromTrajectory = someAccessTargets[i].getTrajectory();
        accesses[i] =
            this->generateAccessesFromIntervals(accessIntervalsPerTarget[i], anInterval, fromTrajectory, aToTrajectory);
    }

    return accesses;
}

Array<Access> Generator::generateAccessesFromIntervals(
    const Array<physics::time::Interval>& someIntervals,
    const physics::time::Interval& anInterval,
    const Trajectory& aFromTrajectory,
    const Trajectory& aToTrajectory
) const
{
    const Shared<const Celestial> earthSPtr = this->environment_.accessCelestialObjectWithName("Earth");

    return someIntervals
        .map<Access>(
            [&anInterval, &aFromTrajectory, &aToTrajectory, &earthSPtr, this](
                const physics::time::Interval& anAccessInterval
            ) -> Access
            {
                return Generator::GenerateAccess(
                    anAccessInterval, anInterval, aFromTrajectory, aToTrajectory, earthSPtr, this->tolerance_
                );
            }
        )
        .getWhere(
            [this](const Access& anAccess) -> bool
            {
                return this->accessFilter_ ? this->accessFilter_(anAccess) : true;
            }
        );
}

Array<physics::time::Interval> Generator::computePreciseCrossings(
    const Array<physics::time::Interval>& accessIntervals,
    const physics::time::Interval& anAnalysisInterval,
    const Vector3d& fromPositionCoordinate_ITRF,
    const Trajectory& aToTrajectory,
    const AccessTarget& anAccessTarget
) const
{
    const RootSolver rootSolver = RootSolver(100, this->tolerance_.inSeconds());

    const Shared<const Celestial> earthSPtr = this->environment_.accessCelestialObjectWithName("Earth");

    const Matrix3d SEZRotation = anAccessTarget.computeR_SEZ_ECEF(earthSPtr);

    std::function<bool(const Instant&)> condition;

    const auto computeAER = [&fromPositionCoordinate_ITRF, &SEZRotation, &aToTrajectory](const Instant& instant
                            ) -> Triple<Real, Real, Real>
    {
        const Vector3d toPositionCoordinates_ITRF =
            aToTrajectory.getStateAt(instant).inFrame(Frame::ITRF()).getPosition().getCoordinates();

        const Vector3d dx = toPositionCoordinates_ITRF - fromPositionCoordinate_ITRF;

        const Vector3d dx_SEZ = SEZRotation * dx;

        const double range_m = dx_SEZ.norm();
        const double elevation_rad = std::asin(dx_SEZ(2) / range_m);
        double azimuth_rad = std::atan2(dx_SEZ(1), -dx_SEZ(0));
        azimuth_rad = azimuth_rad < 0.0 ? azimuth_rad + 2.0 * M_PI : azimuth_rad;

        return {azimuth_rad, elevation_rad, range_m};
    };

    if (anAccessTarget.getConstraint().isIntervalBased())
    {
        const Constraint::IntervalConstraint intervalConstraint =
            anAccessTarget.getConstraint().getIntervalConstraint().value();

        condition = [&computeAER, intervalConstraint](const Instant& instant) -> bool
        {
            const double& azimuthLowerBound = intervalConstraint.azimuth.accessLowerBound();
            const double& azimuthUpperBound = intervalConstraint.azimuth.accessUpperBound();
            const double& elevationLowerBound = intervalConstraint.elevation.accessLowerBound();
            const double& elevationUpperBound = intervalConstraint.elevation.accessUpperBound();
            const double& rangeLowerBound = intervalConstraint.range.accessLowerBound();
            const double& rangeUpperBound = intervalConstraint.range.accessUpperBound();

            const auto [azimuth_rad, elevation_rad, range] = computeAER(instant);

            return azimuth_rad > azimuthLowerBound && azimuth_rad < azimuthUpperBound &&
                   elevation_rad > elevationLowerBound && elevation_rad < elevationUpperBound &&
                   range > rangeLowerBound && range < rangeUpperBound;
        };
    }
    else
    {
        const Constraint::MaskConstraint maskConstraint = anAccessTarget.getConstraint().getMaskConstraint().value();

        condition = [&computeAER, maskConstraint](const Instant& instant) -> bool
        {
            const double& rangeLowerBound = maskConstraint.range.accessLowerBound();
            const double& rangeUpperBound = maskConstraint.range.accessUpperBound();

            const auto [azimuth_rad, elevation_rad, range_m] = computeAER(instant);

            const AER aer = AER(Angle::Radians(azimuth_rad), Angle::Radians(elevation_rad), Length::Meters(range_m));

            return maskConstraint.isSatisfied(aer) && range_m > rangeLowerBound && range_m < rangeUpperBound;
        };
    }

    Array<physics::time::Interval> preciseAccessIntervals = accessIntervals;

    for (Index i = 0; i < preciseAccessIntervals.getSize(); ++i)
    {
        const physics::time::Interval& interval = accessIntervals[i];

        const Instant lowerBoundPreviousInstant = interval.getStart() - this->step_;
        const Instant lowerBoundInstant = interval.getStart();

        Instant intervalStart = anAnalysisInterval.getStart();

        // compute start crossing if it is not the start of the requested analysis interval
        if (lowerBoundInstant != anAnalysisInterval.getStart())
        {
            const auto startCrossingDurationSeconds = rootSolver.solve(
                [&lowerBoundPreviousInstant, &condition](double aDurationInSeconds) -> double
                {
                    return condition(lowerBoundPreviousInstant + Duration::Seconds(aDurationInSeconds)) ? +1.0 : -1.0;
                },
                0.0,
                Duration::Between(lowerBoundPreviousInstant, lowerBoundInstant).inSeconds()
            );
            intervalStart = lowerBoundPreviousInstant + Duration::Seconds(startCrossingDurationSeconds.root);
        }

        const Instant upperBoundInstant = interval.getEnd();
        const Instant upperBoundNextInstant = interval.getEnd() + this->step_;

        Instant intervalEnd = anAnalysisInterval.getEnd();

        // compute end crossing if it is not the end of the requested analysis interval
        if (upperBoundInstant != anAnalysisInterval.getEnd())
        {
            const auto endCrossingDurationSeconds = rootSolver.solve(
                [&upperBoundInstant, &condition](double aDurationInSeconds) -> double
                {
                    return condition(upperBoundInstant + Duration::Seconds(aDurationInSeconds)) ? +1.0 : -1.0;
                },
                0.0,
                Duration::Between(upperBoundInstant, upperBoundNextInstant).inSeconds()
            );
            intervalEnd = upperBoundInstant + Duration::Seconds(endCrossingDurationSeconds.root);
        }

        preciseAccessIntervals[i] = physics::time::Interval::Closed(intervalStart, intervalEnd);
    }

    return preciseAccessIntervals;
}

Array<physics::time::Interval> Generator::ComputeIntervals(const VectorXi& inAccess, const Array<Instant>& instants)
{
    Array<physics::time::Interval> accessIntervals = Array<physics::time::Interval>::Empty();

    VectorXi padded = VectorXi::Zero(inAccess.size() + 2);  // adding zeros to start and end
    padded.segment(1, inAccess.size()) = inAccess;

    const Index paddedSize = padded.size() - 1;
    const VectorXi diff = padded.tail(paddedSize) - padded.head(paddedSize);

    Instant startInstant = Instant::Undefined();
    Instant endInstant = Instant::Undefined();

    for (Eigen::Index j = 0; j < diff.size() - 1; ++j)
    {
        if (!diff[j])
        {
            continue;
        }

        if (diff[j] == 1)
        {
            startInstant = instants[j];
        }

        if (diff[j] == -1)
        {
            endInstant = instants[j - 1];
            accessIntervals.add(physics::time::Interval::Closed(startInstant, endInstant));
        }
    }

    // account for last partial interval

    if (diff.tail<1>().value() == -1)
    {
        accessIntervals.add(physics::time::Interval::Closed(startInstant, instants.accessLast()));
    }

    return accessIntervals;
}

Access Generator::GenerateAccess(
    const physics::time::Interval& anAccessInterval,
    const physics::time::Interval& aGlobalInterval,
    const Trajectory& aFromTrajectory,
    const Trajectory& aToTrajectory,
    const Shared<const Celestial> anEarthSPtr,
    const Duration& aTolerance
)
{
    const Access::Type type = ((aGlobalInterval.accessStart() != anAccessInterval.accessStart()) &&
                               (aGlobalInterval.accessEnd() != anAccessInterval.accessEnd()))
                                ? Access::Type::Complete
                                : Access::Type::Partial;

    const Instant acquisitionOfSignal = anAccessInterval.getStart();
    const Instant timeOfClosestApproach =
        Generator::FindTimeOfClosestApproach(anAccessInterval, aFromTrajectory, aToTrajectory, aTolerance);
    const Instant lossOfSignal = anAccessInterval.getEnd();

    if (!timeOfClosestApproach.isDefined())
    {
        throw ostk::core::error::RuntimeError(
            "Cannot find TCA (solution did not converge): {} - {}.",
            acquisitionOfSignal.toString(),
            lossOfSignal.toString()
        );
    }

    const Angle maxElevation =
        timeOfClosestApproach.isDefined()
            ? Generator::CalculateElevationAt(timeOfClosestApproach, aFromTrajectory, aToTrajectory, anEarthSPtr)
            : Angle::Undefined();

    return Access {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};
}

Instant Generator::FindTimeOfClosestApproach(
    const physics::time::Interval& anAccessInterval,
    const Trajectory& aFromTrajectory,
    const Trajectory& aToTrajectory,
    const Duration& aTolerance
)
{
    struct Context
    {
        const Instant& startInstant;
        const std::function<Pair<State, State>(const Instant& anInstant)>& getStatesAt;
        const std::function<Pair<Position, Position>(const State& aFromState, const State& aToState)>&
            getPositionsFromStates;
    };

    const auto calculateRange = [](const std::vector<double>& x, std::vector<double>& aGradient, void* aDataContext
                                ) -> double
    {
        (void)aGradient;

        if (aDataContext == nullptr)
        {
            throw ostk::core::error::runtime::Wrong("Data context");
        }

        const Context* contextPtr = static_cast<const Context*>(aDataContext);

        const Instant queryInstant = contextPtr->startInstant + Duration::Seconds(x[0]);

        const auto [queryFromState, queryToState] = contextPtr->getStatesAt(queryInstant);
        const auto [queryFromPosition, queryToPosition] =
            contextPtr->getPositionsFromStates(queryFromState, queryToState);

        const Real squaredRange_m =
            (queryToPosition.accessCoordinates() - queryFromPosition.accessCoordinates()).squaredNorm();

        return squaredRange_m;
    };

    Context context = {
        anAccessInterval.getStart(),
        [&aFromTrajectory, &aToTrajectory](const Instant& anInstant) -> Pair<State, State>
        {
            return GeneratorContext::GetStatesAt(anInstant, aFromTrajectory, aToTrajectory);
        },
        GeneratorContext::GetPositionsFromStates
    };

    nlopt::opt optimizer = {nlopt::LN_COBYLA, 1};

    const std::vector<double> lowerBound = {0.0};
    const std::vector<double> upperBound = {anAccessInterval.getDuration().inSeconds()};

    optimizer.set_lower_bounds(lowerBound);
    optimizer.set_upper_bounds(upperBound);

    optimizer.set_min_objective(calculateRange, &context);

    optimizer.set_xtol_rel(aTolerance.inSeconds());

    std::vector<double> x = {0.0};

    try
    {
        double minimumSquaredRange;

        nlopt::result result = optimizer.optimize(x, minimumSquaredRange);

        switch (result)
        {
            case nlopt::STOPVAL_REACHED:
            case nlopt::FTOL_REACHED:
            case nlopt::XTOL_REACHED:
            case nlopt::MAXEVAL_REACHED:
            case nlopt::MAXTIME_REACHED:
                return anAccessInterval.getStart() + Duration::Seconds(x[0]);

            case nlopt::FAILURE:
            case nlopt::INVALID_ARGS:
            case nlopt::OUT_OF_MEMORY:
            case nlopt::ROUNDOFF_LIMITED:
            case nlopt::FORCED_STOP:
            default:
                return Instant::Undefined();
        }
    }
    catch (const std::exception& anException)
    {
        throw ostk::core::error::RuntimeError("Cannot find TCA (algorithm failed): [{}].", anException.what());
    }
}

Angle Generator::CalculateElevationAt(
    const Instant& anInstant,
    const Trajectory& aFromTrajectory,
    const Trajectory& aToTrajectory,
    const Shared<const Celestial> anEarthSPtr
)
{
    const auto [fromState, toState] = GeneratorContext::GetStatesAt(anInstant, aFromTrajectory, aToTrajectory);
    const auto [fromPosition, toPosition] = GeneratorContext::GetPositionsFromStates(fromState, toState);

    const AER aer = GeneratorContext::CalculateAer(anInstant, fromPosition, toPosition, anEarthSPtr);

    return aer.getElevation();
}

GeneratorContext::GeneratorContext(
    const Trajectory& aFromTrajectory,
    const Trajectory& aToTrajectory,
    const Environment& anEnvironment,
    const Generator& aGenerator
)
    : fromTrajectory_(aFromTrajectory),
      toTrajectory_(aToTrajectory),
      environment_(anEnvironment),
      earthSPtr_(environment_.accessCelestialObjectWithName("Earth")),  // [TBR] This is Earth specific
      generator_(aGenerator)
{
}

bool GeneratorContext::isAccessActive(const Instant& anInstant, const Constraint& aConstraint)
{
    this->environment_.setInstant(anInstant);

    const auto [fromState, toState] =
        GeneratorContext::GetStatesAt(anInstant, this->fromTrajectory_, this->toTrajectory_);

    if (this->generator_.getStateFilter() && (!this->generator_.getStateFilter()(fromState, toState)))
    {
        return false;
    }

    const auto [fromPosition, toPosition] = GeneratorContext::GetPositionsFromStates(fromState, toState);

    // Line of sight
    // TBI: Remove this check as it is redundant

    static const Shared<const Frame> commonFrameSPtr = Frame::GCRF();

    const Point fromPositionCoordinates = Point::Vector(fromPosition.accessCoordinates());
    const Point toPositionCoordinates = Point::Vector(toPosition.accessCoordinates());

    if (fromPositionCoordinates != toPositionCoordinates)
    {
        const Segment fromToSegment = {fromPositionCoordinates, toPositionCoordinates};

        const Object::Geometry fromToSegmentGeometry = {fromToSegment, commonFrameSPtr};

        const bool lineOfSight = !this->environment_.intersects(fromToSegmentGeometry);

        if (!lineOfSight)
        {
            return false;
        }
    }

    const AER aer = GeneratorContext::CalculateAer(anInstant, fromPosition, toPosition, earthSPtr_);

    return aConstraint.isSatisfied(aer);
}

Pair<State, State> GeneratorContext::GetStatesAt(
    const Instant& anInstant, const Trajectory& aFromTrajectory, const Trajectory& aToTrajectory
)
{
    const State fromState = aFromTrajectory.getStateAt(anInstant);
    const State toState = aToTrajectory.getStateAt(anInstant);

    return {fromState, toState};
}

Pair<Position, Position> GeneratorContext::GetPositionsFromStates(const State& aFromState, const State& aToState)
{
    if (aFromState.accessInstant() != aToState.accessInstant())
    {
        throw ostk::core::error::RuntimeError("Cannot get positions from states at different instants.");
    }

    static const Shared<const Frame> commonFrameSPtr = Frame::GCRF();

    const Position fromPosition = aFromState.getPosition().inFrame(commonFrameSPtr, aFromState.accessInstant());
    const Position toPosition = aToState.getPosition().inFrame(commonFrameSPtr, aFromState.accessInstant());

    return {fromPosition, toPosition};
}

AER GeneratorContext::CalculateAer(
    const Instant& anInstant,
    const Position& aFromPosition,
    const Position& aToPosition,
    const Shared<const Celestial> anEarthSPtr
)
{
    // [TBM] This logic is Earth-specific
    const Vector3d referenceCoordinates_ITRF = aFromPosition.inFrame(Frame::ITRF(), anInstant).accessCoordinates();

    const LLA referencePoint_LLA =
        LLA::Cartesian(referenceCoordinates_ITRF, anEarthSPtr->getEquatorialRadius(), anEarthSPtr->getFlattening());

    const Shared<const Frame> nedFrameSPtr = anEarthSPtr->getFrameAt(referencePoint_LLA, Earth::FrameType::NED);

    const Position fromPosition_NED = aFromPosition.inFrame(nedFrameSPtr, anInstant);
    const Position toPosition_NED = aToPosition.inFrame(nedFrameSPtr, anInstant);

    return AER::FromPositionToPosition(fromPosition_NED, toPosition_NED, true);
}

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk
