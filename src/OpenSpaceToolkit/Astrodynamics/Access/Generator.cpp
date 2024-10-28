/// Apache License 2.0

#include <chrono>
#include <nlopt.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Segment.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solver/TemporalConditionSolver.hpp>

using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::geometry::d3::object::Segment;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

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

GroundTargetConfiguration::GroundTargetConfiguration(
    const Interval<Real>& anAzimuthInterval,
    const Interval<Real>& anElevationInterval,
    const Interval<Real>& aRangeInterval,
    const Position& aPosition
)
    : azimuthInterval_(anAzimuthInterval),
      elevationInterval_(anElevationInterval),
      rangeInterval_(aRangeInterval),
      position_(aPosition),
      lla_(LLA::Cartesian(
          aPosition.getCoordinates(),
          EarthGravitationalModel::EGM2008.equatorialRadius_,
          EarthGravitationalModel::EGM2008.flattening_
      ))
{
    if (aPosition.isDefined() && aPosition.accessFrame() != Frame::ITRF())
    {
        throw ostk::core::error::RuntimeError("The position frame must be ITRF.");
    }
}

GroundTargetConfiguration::GroundTargetConfiguration(
    const Interval<Real>& anAzimuthInterval,
    const Interval<Real>& anElevationInterval,
    const Interval<Real>& aRangeInterval,
    const LLA& aLLA
)
    : azimuthInterval_(anAzimuthInterval),
      elevationInterval_(anElevationInterval),
      rangeInterval_(aRangeInterval),
      position_(Position::Meters(
          aLLA.toCartesian(
              EarthGravitationalModel::EGM2008.equatorialRadius_, EarthGravitationalModel::EGM2008.flattening_
          ),
          Frame::ITRF()
      )),
      lla_(aLLA)
{
}

Trajectory GroundTargetConfiguration::getTrajectory() const
{
    return Trajectory::Position(position_);
}

Position GroundTargetConfiguration::getPosition() const
{
    return position_;
}

LLA GroundTargetConfiguration::getLLA() const
{
    return lla_;
}

Interval<Real> GroundTargetConfiguration::getAzimuthInterval() const
{
    return azimuthInterval_;
}

Interval<Real> GroundTargetConfiguration::getElevationInterval() const
{
    return elevationInterval_;
}

Interval<Real> GroundTargetConfiguration::getRangeInterval() const
{
    return rangeInterval_;
}

Matrix3d GroundTargetConfiguration::getR_SEZ_ECEF() const
{
    // TBM: Move this to OSTk physics as a utility function
    const double sinLat = std::sin(lla_.getLatitude().inRadians());
    const double cosLat = std::cos(lla_.getLatitude().inRadians());
    const double sinLon = std::sin(lla_.getLongitude().inRadians());
    const double cosLon = std::cos(lla_.getLongitude().inRadians());

    Matrix3d SEZRotation;
    SEZRotation << sinLat * cosLon, sinLat * sinLon, -cosLat, -sinLon, cosLon, 0.0, cosLat * cosLon, cosLat * sinLon,
        sinLat;

    return SEZRotation;
}

Generator::Generator(const Environment& anEnvironment, const Duration& aStep, const Duration& aTolerance)
    : environment_(anEnvironment),
      step_(aStep),
      tolerance_(aTolerance),
      aerFilter_({}),
      accessFilter_({}),
      stateFilter_({})
{
}

Generator::Generator(
    const Environment& anEnvironment,
    const std::function<bool(const AER&)>& anAerFilter,
    const std::function<bool(const Access&)>& anAccessFilter,
    const std::function<bool(const State&, const State&)>& aStateFilter,
    const Duration& aStep,
    const Duration& aTolerance
)
    : environment_(anEnvironment),
      step_(aStep),
      tolerance_(aTolerance),
      aerFilter_(anAerFilter),
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

std::function<bool(const AER&)> Generator::getAerFilter() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return this->aerFilter_;
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
    const Trajectory& aFromTrajectory, const Trajectory& aToTrajectory
) const
{
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

    GeneratorContext generatorContext = GeneratorContext(aFromTrajectory, aToTrajectory, environment_, *this);

    return [generatorContext](const Instant& anInstant) mutable -> bool
    {
        return generatorContext.isAccessActive(anInstant);
    };
}

Array<Access> Generator::computeAccesses(
    const physics::time::Interval& anInterval, const Trajectory& aFromTrajectory, const Trajectory& aToTrajectory
) const
{
    if (!anInterval.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Interval");
    }

    const TemporalConditionSolver temporalConditionSolver = {this->step_, this->tolerance_};

    const Array<physics::time::Interval> accessIntervals =
        temporalConditionSolver.solve(this->getConditionFunction(aFromTrajectory, aToTrajectory), anInterval);

    const Shared<const Celestial> earthSPtr = this->environment_.accessCelestialObjectWithName("Earth");

    return generateAccessesFromIntervals(accessIntervals, anInterval, aFromTrajectory, aToTrajectory);
}

Array<Array<Access>> Generator::computeAccessesWithGroundTargets(
    const physics::time::Interval& anInterval,
    const Array<GroundTargetConfiguration>& someGroundTargetConfigurations,
    const Trajectory& aToTrajectory
) const
{
    // create a stacked matrix of SEZ rotations for all ground targets
    const Index targetCount = someGroundTargetConfigurations.getSize();
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> SEZRotations(3, 3 * targetCount);

    for (Index i = 0; i < targetCount; ++i)
    {
        SEZRotations.block(0, 3 * i, 3, 3) = someGroundTargetConfigurations[i].getR_SEZ_ECEF();
    }

    // create a stacked matrix of ITRF positions for all ground targets
    MatrixXd fromPositionCoordinates_ITRF = MatrixXd::Zero(3, targetCount);

    for (Index i = 0; i < targetCount; ++i)
    {
        fromPositionCoordinates_ITRF.col(i) = someGroundTargetConfigurations[i].getPosition().getCoordinates();
    }

    // create a stacked matrix of azimuth, elevation, and range bounds for all ground targets
    MatrixXd aerLowerBounds = MatrixXd::Zero(targetCount, 3);
    MatrixXd aerUpperBounds = MatrixXd::Zero(targetCount, 3);

    for (Index i = 0; i < targetCount; ++i)
    {
        aerLowerBounds(i, 0) =
            someGroundTargetConfigurations[i].getAzimuthInterval().accessLowerBound() * Real::Pi() / 180.0;
        aerLowerBounds(i, 1) =
            someGroundTargetConfigurations[i].getElevationInterval().accessLowerBound() * Real::Pi() / 180.0;
        aerLowerBounds(i, 2) =
            someGroundTargetConfigurations[i].getRangeInterval().accessLowerBound() * Real::Pi() / 180.0;

        aerUpperBounds(i, 0) =
            someGroundTargetConfigurations[i].getAzimuthInterval().accessUpperBound() * Real::Pi() / 180.0;
        aerUpperBounds(i, 1) =
            someGroundTargetConfigurations[i].getElevationInterval().accessUpperBound() * Real::Pi() / 180.0;
        aerUpperBounds(i, 2) =
            someGroundTargetConfigurations[i].getRangeInterval().accessUpperBound() * Real::Pi() / 180.0;
    }

    // initialize solver and condition
    const RootSolver rootSolver = RootSolver(100, this->tolerance_.inSeconds());

    const auto createAccessCondition =
        [&fromPositionCoordinates_ITRF, &SEZRotations, &aToTrajectory, &aerLowerBounds, &aerUpperBounds](
            const Index& targetIdx
        )
    {
        const Vector3d fromPositionCoordinate_ITRF = fromPositionCoordinates_ITRF.col(targetIdx);

        const Matrix3d SEZRotation = SEZRotations.block(0, 3 * targetIdx, 3, 3);

        const double azimuthLowerBound = aerLowerBounds(targetIdx, 0);
        const double azimuthUpperBound = aerUpperBounds(targetIdx, 0);
        const double elevationLowerBound = aerLowerBounds(targetIdx, 1);
        const double elevationUpperBound = aerUpperBounds(targetIdx, 1);
        const double rangeLowerBound = aerLowerBounds(targetIdx, 2);
        const double rangeUpperBound = aerUpperBounds(targetIdx, 2);

        return [fromPositionCoordinate_ITRF,
                SEZRotation,
                azimuthLowerBound,
                azimuthUpperBound,
                elevationLowerBound,
                elevationUpperBound,
                rangeLowerBound,
                rangeUpperBound,
                &aToTrajectory](const Instant& instant) -> bool
        {
            const auto toPositionCoordinates_ITRF =
                aToTrajectory.getStateAt(instant).inFrame(Frame::ITRF()).getPosition().getCoordinates();

            const VectorXd dx = toPositionCoordinates_ITRF - fromPositionCoordinate_ITRF;

            const MatrixXd dx_SEZ = SEZRotation * dx;

            const double range = dx_SEZ.norm();
            const double elevation_rad = std::asin(dx_SEZ(2) / range);
            double azimuth_rad = std::atan2(dx_SEZ(1), dx_SEZ(0));

            azimuth_rad = azimuth_rad < 0.0 ? azimuth_rad + 2.0 * M_PI : azimuth_rad;

            return azimuth_rad > azimuthLowerBound && azimuth_rad < azimuthUpperBound &&
                   elevation_rad > elevationLowerBound && elevation_rad < elevationUpperBound &&
                   range > rangeLowerBound && range < rangeUpperBound;
        };
    };

    // initialize containers
    const Array<Instant> instants = anInterval.generateGrid(this->step_);

    Array<std::function<bool(const Instant&)>> conditionArray(targetCount, nullptr);
    for (Index i = 0; i < targetCount; ++i)
    {
        conditionArray[i] = createAccessCondition(i);
    }

    Array<Array<physics::time::Interval>> accessIntervals =
        Array<Array<physics::time::Interval>>(targetCount, Array<physics::time::Interval>::Empty());
    Array<bool> previousAccessStates = Array<bool>(targetCount, false);

    for (Index index = 0; index < instants.getSize(); ++index)
    {
        const Instant& instant = instants[index];

        // calculate target to satellite vector in ITRF
        const Vector3d toPositionCoordinates_ITRF =
            aToTrajectory.getStateAt(instant).inFrame(Frame::ITRF()).getPosition().getCoordinates();

        const MatrixXd dx =
            toPositionCoordinates_ITRF.replicate(1, fromPositionCoordinates_ITRF.cols()) - fromPositionCoordinates_ITRF;

        // calculate target to satellite vector in SEZ
        MatrixXd dx_SEZ = MatrixXd::Zero(3, dx.cols());
        for (Index i = 0; i < (Index)dx.cols(); ++i)
        {
            dx_SEZ.col(i) = SEZRotations.block<3, 3>(0, 3 * i) * dx.col(i);
        }

        // calculate azimuth, elevation, and range
        const VectorXd range = dx_SEZ.colwise().norm();
        const VectorXd elevation_rad = (dx_SEZ.row(2).transpose().array() / range.array()).asin();
        VectorXd azimuth_rad = dx_SEZ.row(0).array().binaryExpr(
            dx_SEZ.row(1).array(),
            [](double x, double y)
            {
                return std::atan2(y, x);
            }
        );

        azimuth_rad = azimuth_rad.unaryExpr(
            [](double x)
            {
                return x < 0.0 ? x + 2.0 * M_PI : x;
            }
        );

        // check if satellite is in access
        const auto inAccess =
            (azimuth_rad.array() > aerLowerBounds.col(0).array() &&
             azimuth_rad.array() < aerUpperBounds.col(0).array() &&
             elevation_rad.array() > aerLowerBounds.col(1).array() &&
             elevation_rad.array() < aerUpperBounds.col(1).array() && range.array() > aerLowerBounds.col(2).array() &&
             range.array() < aerUpperBounds.col(2).array())
                .eval();

        // First instant handling
        if (index == 0)
        {
            for (Index i = 0; i < (Index)inAccess.size(); ++i)
            {
                previousAccessStates[i] = inAccess[i];
                if (inAccess[i])
                {
                    accessIntervals[i].add(physics::time::Interval::Closed(instant, instant));
                }
            }
            continue;
        }

        const Instant& previousInstant = instants[index - 1];

        // Check for state changes and find exact crossings

        for (Index i = 0; i < (Index)inAccess.size(); ++i)
        {
            const bool currentAccess = inAccess[i];

            if (currentAccess != previousAccessStates[i])
            {
                // Find exact crossing time
                const auto condition = conditionArray[i];

                const auto result = rootSolver.solve(
                    [&previousInstant, &condition](double aDurationInSeconds) -> double
                    {
                        return condition(previousInstant + Duration::Seconds(aDurationInSeconds)) ? +1.0 : -1.0;
                    },
                    0.0,
                    Duration::Between(previousInstant, instant).inSeconds()
                );

                const Instant crossingTime = previousInstant + Duration::Seconds(result.root);

                if (currentAccess)  // Rising edge
                {
                    accessIntervals[i].add(physics::time::Interval::Closed(crossingTime, instant));
                }
                else  // Falling edge
                {
                    if (!accessIntervals[i].isEmpty())
                    {
                        const physics::time::Interval& lastInterval = accessIntervals[i].accessLast();
                        accessIntervals[i].accessLast() =
                            physics::time::Interval::Closed(lastInterval.getStart(), crossingTime);
                    }
                }
            }
            else if (currentAccess)  // Continuing access
            {
                if (!accessIntervals[i].isEmpty())
                {
                    const physics::time::Interval& lastInterval = accessIntervals[i].accessLast();
                    accessIntervals[i].accessLast() = physics::time::Interval::Closed(lastInterval.getStart(), instant);
                }
            }

            previousAccessStates[i] = currentAccess;
        }
    }

    const Shared<const Celestial> earthSPtr = this->environment_.accessCelestialObjectWithName("Earth");

    Array<Array<Access>> accesses = Array<Array<Access>>(targetCount, Array<Access>::Empty());
    for (Index i = 0; i < accessIntervals.getSize(); ++i)
    {
        const Trajectory fromTrajectory = someGroundTargetConfigurations[i].getTrajectory();
        accesses[i] =
            this->generateAccessesFromIntervals(accessIntervals[i], anInterval, fromTrajectory, aToTrajectory);
    }

    return accesses;
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

void Generator::setAerFilter(const std::function<bool(const AER&)>& anAerFilter)
{
    this->aerFilter_ = anAerFilter;
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
    return {Environment::Undefined()};
}

Generator Generator::AerRanges(
    const Interval<Real>& anAzimuthRange,
    const Interval<Real>& anElevationRange,
    const Interval<Real>& aRangeRange,
    const Environment& anEnvironment
)
{
    using ostk::core::type::Real;

    const Interval<Real> azimuthRange_deg = anAzimuthRange;
    const Interval<Real> elevationRange_deg = anElevationRange;
    const Interval<Real> rangeRange_m = aRangeRange;

    const std::function<bool(const AER&)> aerFilter =
        [azimuthRange_deg, elevationRange_deg, rangeRange_m](const AER& anAER) -> bool
    {
        return ((!azimuthRange_deg.isDefined()) || azimuthRange_deg.contains(anAER.getAzimuth().inDegrees(0.0, +360.0))
               ) &&
               ((!elevationRange_deg.isDefined()) ||
                elevationRange_deg.contains(anAER.getElevation().inDegrees(-180.0, +180.0))) &&
               ((!rangeRange_m.isDefined()) || rangeRange_m.contains(anAER.getRange().inMeters()));
    };

    return {anEnvironment, aerFilter};
}

Generator Generator::AerMask(
    const Map<Real, Real>& anAzimuthElevationMask, const Interval<Real>& aRangeRange, const Environment& anEnvironment
)
{
    using ostk::core::container::Map;
    using ostk::core::type::Real;

    using ostk::mathematics::object::Vector2d;

    if ((anAzimuthElevationMask.empty()) || (anAzimuthElevationMask.begin()->first < 0.0) ||
        (anAzimuthElevationMask.rbegin()->first > 360.0))
    {
        throw ostk::core::error::runtime::Wrong("Azimuth-Elevation Mask");
    }

    for (const auto& azimuthElevationPair : anAzimuthElevationMask)
    {
        if ((azimuthElevationPair.second).abs() > 90.0)
        {
            throw ostk::core::error::runtime::Wrong("Azimuth-Elevation Mask");
        }
    }

    Map<Real, Real> anAzimuthElevationMask_deg = anAzimuthElevationMask;
    const Interval<Real> rangeRange_m = aRangeRange;

    if (anAzimuthElevationMask_deg.begin()->first != 0.0)
    {
        anAzimuthElevationMask_deg.insert({0.0, anAzimuthElevationMask_deg.begin()->second});
    }

    if (anAzimuthElevationMask_deg.rbegin()->first != 360.0)
    {
        anAzimuthElevationMask_deg.insert({360.0, anAzimuthElevationMask_deg.begin()->second});
    }

    const std::function<bool(const AER&)> aerFilter = [anAzimuthElevationMask_deg,
                                                       rangeRange_m](const AER& anAER) -> bool
    {
        const Real azimuth = anAER.getAzimuth().inDegrees(0.0, +360.0);
        const Real elevation = anAER.getElevation().inDegrees(-180.0, +180.0);

        auto itLow = anAzimuthElevationMask_deg.lower_bound(azimuth);
        itLow--;
        auto itUp = anAzimuthElevationMask_deg.upper_bound(azimuth);

        // Vector between the two successive mask data points with bounding azimuth values

        const Vector2d lowToUpVector = {itUp->first - itLow->first, itUp->second - itLow->second};

        // Vector from data point with azimuth lower bound to tested point

        const Vector2d lowToPointVector = {azimuth - itLow->first, elevation - itLow->second};

        // If the determinant of these two vectors is positive, the tested point lies above the function defined by the
        // mask

        return (lowToUpVector[0] * lowToPointVector[1] - lowToUpVector[1] * lowToPointVector[0] >= 0.0) &&
               ((!rangeRange_m.isDefined()) || rangeRange_m.contains(anAER.getRange().inMeters()));
    };

    return {anEnvironment, aerFilter};
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
                throw ostk::core::error::RuntimeError("Cannot find TCA (solution did not converge).");
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

bool GeneratorContext::isAccessActive(const Instant& anInstant)
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

    // AER filtering

    if (this->generator_.getAerFilter())
    {
        const AER aer = GeneratorContext::CalculateAer(anInstant, fromPosition, toPosition, this->earthSPtr_);

        if (!this->generator_.getAerFilter()(aer))
        {
            return false;
        }
    }

    return true;
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
