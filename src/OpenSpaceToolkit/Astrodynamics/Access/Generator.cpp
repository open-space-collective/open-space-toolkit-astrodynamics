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

const AccessTarget::Type& AccessTarget::accessType() const
{
    return type_;
}

const VisibilityCriterion& AccessTarget::accessVisibilityCriterion() const
{
    return visibilityCriterion_;
}

const Trajectory& AccessTarget::accessTrajectory() const
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
    const VisibilityCriterion& aVisibilityCriterion, const LLA& anLLA, const Shared<const Celestial>& aCelestialSPtr
)
{
    return AccessTarget(
        AccessTarget::Type::Fixed,
        aVisibilityCriterion,
        Trajectory::Position(Position::Meters(
            anLLA.toCartesian(aCelestialSPtr->getEquatorialRadius(), aCelestialSPtr->getFlattening()), Frame::ITRF()
        ))
    );
}

AccessTarget AccessTarget::FromPosition(const VisibilityCriterion& aVisibilityCriterion, const Position& aPosition)
{
    if (!aPosition.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Position");
    }

    return AccessTarget(AccessTarget::Type::Fixed, aVisibilityCriterion, Trajectory::Position(aPosition));
}

AccessTarget AccessTarget::FromTrajectory(
    const VisibilityCriterion& aVisibilityCriterion, const Trajectory& aTrajectory
)
{
    if (!aTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Trajectory");
    }

    return AccessTarget(AccessTarget::Type::Trajectory, aVisibilityCriterion, aTrajectory);
}

AccessTarget::AccessTarget(
    const AccessTarget::Type& aType, const VisibilityCriterion& aVisibilityCriterion, const Trajectory& aTrajectory
)
    : type_(aType),
      visibilityCriterion_(aVisibilityCriterion),
      trajectory_(aTrajectory)
{
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
    if (!aToTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("To Trajectory");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return [&anAccessTarget, &aToTrajectory, this](const Instant& anInstant) mutable -> bool
    {
        const State fromState = anAccessTarget.accessTrajectory().getStateAt(anInstant);
        const State toState = aToTrajectory.getStateAt(anInstant);

        if (this->getStateFilter() && (!this->getStateFilter()(fromState, toState)))
        {
            return false;
        }

        const Position fromPosition = fromState.getPosition();
        const Position toPosition = toState.getPosition();

        const Position fromPosition_ITRF = fromPosition.inFrame(Frame::ITRF(), anInstant);
        const Position toPosition_ITRF = toPosition.inFrame(Frame::ITRF(), anInstant);

        const VisibilityCriterion& visibilityCriterion = anAccessTarget.accessVisibilityCriterion();

        if (visibilityCriterion.is<VisibilityCriterion::LineOfSight>())
        {
            return visibilityCriterion.as<VisibilityCriterion::LineOfSight>().value().isSatisfied(
                anInstant, fromPosition_ITRF.accessCoordinates(), toPosition_ITRF.accessCoordinates()
            );
        }

        const AER aer = Generator::CalculateAer(
            anInstant, fromPosition, toPosition, this->environment_.accessCelestialObjectWithName("Earth")
        );

        if (visibilityCriterion.is<VisibilityCriterion::AERMask>())
        {
            return visibilityCriterion.as<VisibilityCriterion::AERMask>().value().isSatisfied(aer);
        }

        if (visibilityCriterion.is<VisibilityCriterion::AERInterval>())
        {
            return visibilityCriterion.as<VisibilityCriterion::AERInterval>().value().isSatisfied(aer);
        }

        if (visibilityCriterion.is<VisibilityCriterion::ElevationInterval>())
        {
            return visibilityCriterion.as<VisibilityCriterion::ElevationInterval>().value().isSatisfied(
                aer.getElevation()
            );
        }

        return false;
    };
}

Array<Access> Generator::computeAccesses(
    const physics::time::Interval& anInterval,
    const AccessTarget& anAccessTarget,
    const Trajectory& aToTrajectory,
    const bool& coarse
) const
{
    if (!anInterval.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Interval");
    }

    if (!aToTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("To Trajectory");
    }

    if (anAccessTarget.accessType() == AccessTarget::Type::Trajectory)
    {
        if (coarse)
        {
            throw ostk::core::error::RuntimeError("Coarse mode is not supported for trajectory targets.");
        }

        return this->computeAccessesForTrajectoryTarget(anInterval, anAccessTarget, aToTrajectory);
    }

    return this->computeAccessesForFixedTargets(
        anInterval, Array<AccessTarget> {anAccessTarget}, aToTrajectory, coarse
    )[0];
}

Array<Array<Access>> Generator::computeAccesses(
    const physics::time::Interval& anInterval,
    const Array<AccessTarget>& someAccessTargets,
    const Trajectory& aToTrajectory,
    const bool& coarse
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

    if (!aToTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("To Trajectory");
    }

    if (std ::all_of(
            someAccessTargets.begin(),
            someAccessTargets.end(),
            [](const auto& accessTarget)
            {
                return accessTarget.accessType() == AccessTarget::Type::Trajectory;
            }
        ))
    {
        if (coarse)
        {
            throw ostk::core::error::RuntimeError("Coarse mode is not supported for trajectory targets.");
        }

        Array<Array<Access>> accessesPerTarget;
        accessesPerTarget.reserve(someAccessTargets.getSize());

        for (const auto& accessTarget : someAccessTargets)
        {
            accessesPerTarget.add(this->computeAccessesForTrajectoryTarget(anInterval, accessTarget, aToTrajectory));
        }

        return accessesPerTarget;
    }

    if (std::all_of(
            someAccessTargets.begin(),
            someAccessTargets.end(),
            [](const auto& accessTarget)
            {
                return accessTarget.accessType() == AccessTarget::Type::Fixed;
            }
        ))
    {
        return this->computeAccessesForFixedTargets(anInterval, someAccessTargets, aToTrajectory, coarse);
    }

    throw ostk::core::error::RuntimeError("All targets must be of same type.");

    return {};
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

    const Trajectory& fromTrajectory = anAccessTarget.accessTrajectory();

    return generateAccessesFromIntervals(accessIntervals, anInterval, fromTrajectory, aToTrajectory);
}

Array<Array<Access>> Generator::computeAccessesForFixedTargets(
    const physics::time::Interval& anInterval,
    const Array<AccessTarget>& someAccessTargets,
    const Trajectory& aToTrajectory,
    const bool& coarse
) const
{
    // create a stacked matrix of SEZ rotations for all access targets

    const Index targetCount = someAccessTargets.getSize();
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> SEZRotations(3, 3 * targetCount);

    const Shared<const Celestial> earthSPtr = this->environment_.accessCelestialObjectWithName("Earth");

    for (Index i = 0; i < targetCount; ++i)
    {
        SEZRotations.block<3, 3>(0, 3 * i) = someAccessTargets[i].computeR_SEZ_ECEF(earthSPtr);
    }

    // create a stacked matrix of ITRF positions for all access targets

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
            return accessTarget.accessVisibilityCriterion().template is<VisibilityCriterion::AERMask>();
        }
    );
    const bool allAccessTargetsHaveAERIntervals = std::all_of(
        someAccessTargets.begin(),
        someAccessTargets.end(),
        [](const auto& accessTarget)
        {
            return accessTarget.accessVisibilityCriterion().template is<VisibilityCriterion::AERInterval>();
        }
    );
    const bool allAccessTargetsHaveLineOfSight = std::all_of(
        someAccessTargets.begin(),
        someAccessTargets.end(),
        [](const auto& accessTarget)
        {
            return accessTarget.accessVisibilityCriterion().template is<VisibilityCriterion::LineOfSight>();
        }
    );
    const bool allAccessTargetsHaveElevationIntervals = std::all_of(
        someAccessTargets.begin(),
        someAccessTargets.end(),
        [](const auto& accessTarget)
        {
            return accessTarget.accessVisibilityCriterion().template is<VisibilityCriterion::ElevationInterval>();
        }
    );

    const auto computeAer = [&SEZRotations, &fromPositionCoordinates_ITRF](const Vector3d& aToPositionCoordinates_ITRF
                            ) -> Triple<VectorXd, VectorXd, VectorXd>
    {
        const MatrixXd dx = (-fromPositionCoordinates_ITRF).colwise() + aToPositionCoordinates_ITRF;

        // TBI: See if this can be vectorized
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

        return {azimuth_rad, elevation_rad, range_m};
    };

    const auto computeElevations = [&fromPositionCoordinates_ITRF](const Vector3d& aToPositionCoordinates_ITRF
                                   ) -> VectorXd
    {
        const MatrixXd dx = (-fromPositionCoordinates_ITRF).colwise() + aToPositionCoordinates_ITRF;
        const MatrixXd fromPositionDirection_ITRF = fromPositionCoordinates_ITRF.colwise().normalized();

        // columnwise dot product of dx and fromPositionDirection_ITRF
        const VectorXd dx_Z =
            (dx.cwiseProduct(fromPositionDirection_ITRF)).colwise().sum().array() / dx.colwise().norm().array();

        return dx_Z.array().asin();
    };

    std::function<ArrayXb(const MatrixXd&, const Vector3d&, const Instant&)> visibilityCriterionFilter;

    if (allAccessTargetsHaveAERIntervals)
    {
        // create a stacked matrix of azimuth, elevation, and range bounds for all access targets
        MatrixXd aerLowerBounds = MatrixXd::Zero(targetCount, 3);
        MatrixXd aerUpperBounds = MatrixXd::Zero(targetCount, 3);

        for (Index i = 0; i < targetCount; ++i)
        {
            const VisibilityCriterion::AERInterval visibilityCriterion =
                someAccessTargets[i].accessVisibilityCriterion().as<VisibilityCriterion::AERInterval>().value();

            aerLowerBounds(i, 0) = visibilityCriterion.azimuth.accessLowerBound();
            aerLowerBounds(i, 1) = visibilityCriterion.elevation.accessLowerBound();
            aerLowerBounds(i, 2) = visibilityCriterion.range.accessLowerBound();

            aerUpperBounds(i, 0) = visibilityCriterion.azimuth.accessUpperBound();
            aerUpperBounds(i, 1) = visibilityCriterion.elevation.accessUpperBound();
            aerUpperBounds(i, 2) = visibilityCriterion.range.accessUpperBound();
        }

        visibilityCriterionFilter = [aerLowerBounds, aerUpperBounds, &computeAer](
                                        const MatrixXd& aFromPositionCoordinates_ITRF,
                                        const Vector3d& aToPositionCoordinates_ITRF,
                                        const Instant& anInstant
                                    )
        {
            (void)anInstant;
            (void)aFromPositionCoordinates_ITRF;

            const auto [azimuths, elevations, ranges] = computeAer(aToPositionCoordinates_ITRF);

            return (azimuths.array() > aerLowerBounds.col(0).array() &&
                    azimuths.array() < aerUpperBounds.col(0).array() &&
                    elevations.array() > aerLowerBounds.col(1).array() &&
                    elevations.array() < aerUpperBounds.col(1).array() &&
                    ranges.array() > aerLowerBounds.col(2).array() && ranges.array() < aerUpperBounds.col(2).array())
                .eval();
        };
    }
    else if (allAccessTargetsHaveMasks)
    {
        visibilityCriterionFilter = [&someAccessTargets, &computeAer](
                                        const MatrixXd& aFromPositionCoordinates_ITRF,
                                        const Vector3d& aToPositionCoordinates_ITRF,
                                        const Instant& anInstant
                                    )
        {
            (void)anInstant;
            (void)aFromPositionCoordinates_ITRF;

            const auto [azimuths_rad, elevations_rad, ranges_m] = computeAer(aToPositionCoordinates_ITRF);

            ArrayXb mask(azimuths_rad.rows());
            for (Eigen::Index i = 0; i < mask.rows(); ++i)
            {
                const VisibilityCriterion::AERMask visibilityCriterion =
                    someAccessTargets[i].accessVisibilityCriterion().as<VisibilityCriterion::AERMask>().value();

                const double& azimuth_rad = azimuths_rad(i);
                const double& elevation_rad = elevations_rad(i);
                const double& range_m = ranges_m(i);

                mask(i) = visibilityCriterion.isSatisfied(azimuth_rad, elevation_rad, range_m);
            }

            return mask;
        };
    }
    else if (allAccessTargetsHaveLineOfSight)
    {
        visibilityCriterionFilter = [&someAccessTargets](
                                        const MatrixXd& aFromPositionCoordinates_ITRF,
                                        const Vector3d& aToPositionCoordinates_ITRF,
                                        const Instant& anInstant
                                    )
        {
            ArrayXb mask(aFromPositionCoordinates_ITRF.cols());

            for (Eigen::Index i = 0; i < mask.rows(); ++i)
            {
                const VisibilityCriterion::LineOfSight visibilityCriterion =
                    someAccessTargets[i].accessVisibilityCriterion().as<VisibilityCriterion::LineOfSight>().value();

                const Vector3d& fromPositionCoordinate_ITRF = aFromPositionCoordinates_ITRF.col(i);

                mask(i) = visibilityCriterion.isSatisfied(
                    anInstant, fromPositionCoordinate_ITRF, aToPositionCoordinates_ITRF
                );
            }

            return mask;
        };
    }
    else if (allAccessTargetsHaveElevationIntervals)
    {
        // create a stacked matrix of azimuth, elevation, and range bounds for all ground targets
        VectorXd elevationLowerBounds = VectorXd::Zero(targetCount);
        VectorXd elevationUpperBounds = VectorXd::Zero(targetCount);

        for (Index i = 0; i < targetCount; ++i)
        {
            const VisibilityCriterion::ElevationInterval visibilityCriterion =
                someAccessTargets[i].accessVisibilityCriterion().as<VisibilityCriterion::ElevationInterval>().value();

            elevationLowerBounds(i) = visibilityCriterion.elevation.accessLowerBound();
            elevationUpperBounds(i) = visibilityCriterion.elevation.accessUpperBound();
        }

        visibilityCriterionFilter = [elevationLowerBounds, elevationUpperBounds, &computeElevations](
                                        const MatrixXd& aFromPositionCoordinates_ITRF,
                                        const Vector3d& aToPositionCoordinates_ITRF,
                                        const Instant& anInstant
                                    )
        {
            (void)anInstant;
            (void)aFromPositionCoordinates_ITRF;

            const VectorXd elevations = computeElevations(aToPositionCoordinates_ITRF);

            return (elevations.array() > elevationLowerBounds.array() &&
                    elevations.array() < elevationUpperBounds.array())
                .eval();
        };
    }
    else
    {
        throw ostk::core::error::RuntimeError("All access targets must have the same type of Visibility Criteria.");
    }

    const Array<Instant> instants = anInterval.generateGrid(this->step_);

    MatrixXi inAccessPerTarget = MatrixXi::Zero(instants.getSize(), targetCount);

    for (Index index = 0; index < instants.getSize(); ++index)
    {
        const Instant& instant = instants[index];

        const State toTrajectoryState = aToTrajectory.getStateAt(instant);

        // calculate target to satellite vector in ITRF
        const Vector3d toPositionCoordinates_ITRF =
            toTrajectoryState.inFrame(Frame::ITRF()).getPosition().getCoordinates();

        // check if satellite is in access
        auto inAccess = visibilityCriterionFilter(fromPositionCoordinates_ITRF, toPositionCoordinates_ITRF, instant);

        if (this->getStateFilter())
        {
            for (Index i = 0; i < targetCount; ++i)
            {
                const State fromState = someAccessTargets[i].accessTrajectory().getStateAt(instant);

                inAccess(i) = inAccess(i) && this->getStateFilter()(fromState, toTrajectoryState);
            }
        }

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
        const Trajectory& fromTrajectory = someAccessTargets[i].accessTrajectory();
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
                    anAccessInterval, anInterval, aFromTrajectory, aToTrajectory, this->tolerance_
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

    if (anAccessTarget.accessVisibilityCriterion().is<VisibilityCriterion::AERInterval>())
    {
        const VisibilityCriterion::AERInterval visibilityCriterion =
            anAccessTarget.accessVisibilityCriterion().as<VisibilityCriterion::AERInterval>().value();

        condition = [&computeAER, visibilityCriterion](const Instant& instant) -> bool
        {
            const auto [azimuth_rad, elevation_rad, range_m] = computeAER(instant);

            return visibilityCriterion.isSatisfied(azimuth_rad, elevation_rad, range_m);
        };
    }
    else if (anAccessTarget.accessVisibilityCriterion().is<VisibilityCriterion::AERMask>())
    {
        const VisibilityCriterion::AERMask visibilityCriterion =
            anAccessTarget.accessVisibilityCriterion().as<VisibilityCriterion::AERMask>().value();

        condition = [&computeAER, visibilityCriterion](const Instant& instant) -> bool
        {
            const auto [azimuth_rad, elevation_rad, range_m] = computeAER(instant);

            return visibilityCriterion.isSatisfied(azimuth_rad, elevation_rad, range_m);
        };
    }
    else if (anAccessTarget.accessVisibilityCriterion().is<VisibilityCriterion::LineOfSight>())
    {
        const VisibilityCriterion::LineOfSight visibilityCriterion =
            anAccessTarget.accessVisibilityCriterion().as<VisibilityCriterion::LineOfSight>().value();

        condition = [&fromPositionCoordinate_ITRF, &aToTrajectory, visibilityCriterion](const Instant& instant) -> bool
        {
            const Vector3d toPositionCoordinates_ITRF =
                aToTrajectory.getStateAt(instant).inFrame(Frame::ITRF()).getPosition().getCoordinates();

            return visibilityCriterion.isSatisfied(instant, fromPositionCoordinate_ITRF, toPositionCoordinates_ITRF);
        };
    }
    else if (anAccessTarget.accessVisibilityCriterion().is<VisibilityCriterion::ElevationInterval>())
    {
        const VisibilityCriterion::ElevationInterval visibilityCriterion =
            anAccessTarget.accessVisibilityCriterion().as<VisibilityCriterion::ElevationInterval>().value();

        condition = [&fromPositionCoordinate_ITRF, &aToTrajectory, visibilityCriterion](const Instant& instant) -> bool
        {
            const Vector3d toPositionCoordinates_ITRF =
                aToTrajectory.getStateAt(instant).inFrame(Frame::ITRF()).getPosition().getCoordinates();

            const Vector3d dx = toPositionCoordinates_ITRF - fromPositionCoordinate_ITRF;

            const double elevation_rad = std::asin(dx.dot(fromPositionCoordinate_ITRF.normalized()) / dx.norm());

            return visibilityCriterion.isSatisfied(elevation_rad);
        };
    }
    else
    {
        throw ostk::core::error::RuntimeError("VisibilityCriterion type not supported.");
    }

    Array<physics::time::Interval> preciseAccessIntervals = accessIntervals;

    for (Index i = 0; i < preciseAccessIntervals.getSize(); ++i)
    {
        const physics::time::Interval& interval = accessIntervals[i];

        // If the analysisInterval is Closed on the right-half, then the final Instant is possibly less than `step_`
        // after the previous Instant. If we take `step_` back, we could "overshoot" the previous coarse step and
        // potentially miss a gap in the access.
        //
        // e.g. if `step_` = 10s:
        // t  | coarse step | inAccess
        // 0  |      y      | true
        // 10 |      y      | true
        // 20 |      y      | true
        // 25 |             | true
        // 26 |             | false
        // 30 |      y      | false
        // 35 |      y      | true   <- end of analysisInterval
        //
        // At t=35, if we took a 10s back to t=25, then both endpoints appear to be in an access, and the root finding
        // will fail. Instead, we should start from the end of the previous Interval at t=20, and take a 10s forward.
        const Instant lowerBoundPreviousInstant =
            i == 0 ? (interval.getStart() - this->step_) : (accessIntervals[i - 1].getEnd() + this->step_);

        const Instant lowerBoundInstant = interval.getStart();

        Instant intervalStart = anAnalysisInterval.getStart();

        // Compute start crossing if both bounding instants are within the analysis interval
        if (lowerBoundPreviousInstant >= anAnalysisInterval.getStart())
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
        const Instant upperBoundNextInstant = std::min(interval.getEnd() + this->step_, anAnalysisInterval.getEnd());

        Instant intervalEnd = anAnalysisInterval.getEnd();

        // Compute end crossing if both bounding instants are within the analysis interval
        if (upperBoundNextInstant <= anAnalysisInterval.getEnd() && upperBoundNextInstant != upperBoundInstant)
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

    if (!timeOfClosestApproach.isDefined() and type == Access::Type::Complete)
    {
        throw ostk::core::error::RuntimeError(
            "Cannot find TCA (solution did not converge): {} - {}.",
            acquisitionOfSignal.toString(),
            lossOfSignal.toString()
        );
    }

    const Angle maxElevation =
        timeOfClosestApproach.isDefined()
            ? Generator::CalculateElevationAt(timeOfClosestApproach, aFromTrajectory, aToTrajectory)
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
    };

    const auto calculateRange = [](const std::vector<double>& x, std::vector<double>& aGradient, void* aDataContext
                                ) -> double
    {
        (void)aGradient;
        if (aDataContext == nullptr)
        {
            throw ostk::core::error::runtime::Wrong("Data context", "nullptr");
        }

        const Context* contextPtr = static_cast<const Context*>(aDataContext);

        const Instant queryInstant = contextPtr->startInstant + Duration::Seconds(x[0]);

        const auto [queryFromState, queryToState] = contextPtr->getStatesAt(queryInstant);

        const Vector3d deltaPosition =
            queryFromState.getPosition().accessCoordinates() - queryToState.getPosition().accessCoordinates();

        const Real rangeSquared = deltaPosition.squaredNorm();

        return rangeSquared;
    };

    Context context = {
        anAccessInterval.getStart(),
        [&aFromTrajectory, &aToTrajectory](const Instant& anInstant) -> Pair<State, State>
        {
            return {
                aFromTrajectory.getStateAt(anInstant),
                aToTrajectory.getStateAt(anInstant),
            };
        },
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
    const Instant& anInstant, const Trajectory& aFromTrajectory, const Trajectory& aToTrajectory
)
{
    const Vector3d fromPositionCoordinates_ITRF =
        aFromTrajectory.getStateAt(anInstant).inFrame(Frame::ITRF()).getPosition().getCoordinates();
    const Vector3d toPositionCoordinates_ITRF =
        aToTrajectory.getStateAt(anInstant).inFrame(Frame::ITRF()).getPosition().getCoordinates();

    const Vector3d dx = toPositionCoordinates_ITRF - fromPositionCoordinates_ITRF;

    return Angle::Radians(std::asin(dx.dot(fromPositionCoordinates_ITRF.normalized()) / dx.norm()));
}

AER Generator::CalculateAer(
    const Instant& anInstant,
    const Position& aFromPosition,
    const Position& aToPosition,
    const Shared<const Celestial>& anEarthSPtr
)
{
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
