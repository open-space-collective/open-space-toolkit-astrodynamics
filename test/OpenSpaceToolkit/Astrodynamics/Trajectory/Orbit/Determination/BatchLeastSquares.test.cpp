////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Determination/BatchLeastSquares.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Determination/BatchLeastSquares.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Containers/Dictionary.hpp>
#include <OpenSpaceToolkit/Core/Containers/Object.hpp>
#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Pair.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Determination_BatchLeastSquares, Constructor)
{

    FAIL() ;

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Determination_BatchLeastSquares, GenerateOrbit)
{

    FAIL() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Determination_BatchLeastSquares, Test_1)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Integer ;
    using ostk::core::types::Size ;
    using ostk::core::types::Real ;
    using ostk::core::types::String ;
    using ostk::core::ctnr::Pair ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Object ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;

    using ostk::astro::trajectory::State ;
    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::orbit::determination::BatchLeastSquares ;

    {

        const Environment environment = Environment::Default() ;

        const File configurationFile = File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Determination/BatchLeastSquares/Test_1/Configuration.json")) ;
        const File referenceDataFile = File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Determination/BatchLeastSquares/Test_1/EllipticalInclinedOrbit t_UTC x_GCRF v_GCRF rev.csv")) ;

        const Object configurationObject = Object::Load(configurationFile, Object::Format::JSON) ;
        const Table referenceDataTable = Table::Load(referenceDataFile, Table::Format::CSV, true) ;

        const Integer decimation = configurationObject["decimation"].accessInteger() ;
        const Size maxMeasurementCount = configurationObject["maxMeasurementCount"].accessInteger() ;
        const Real positionTolerance_m = configurationObject["positionTolerance_m"].accessReal() ;
        const Real positionMeasurementError_m = configurationObject["positionMeasurementError_m"].accessReal() ;

        Integer index = 0 ;

        Array<Pair<Instant, Position>> trueStates = Array<Pair<Instant, Position>>::Empty() ;
        Array<Pair<Instant, Position>> measuredStates = Array<Pair<Instant, Position>>::Empty() ;

        for (const auto& referenceDataRow : referenceDataTable)
        {

            if (index % decimation == 0)
            {

                const Instant instant = Instant::DateTime(DateTime::Parse(referenceDataRow[0].accessString()), Scale::UTC) ;
                const Position truePosition = Position::Meters({ referenceDataRow[1].accessReal(), referenceDataRow[2].accessReal(), referenceDataRow[3].accessReal() }, Frame::GCRF()) ;

                trueStates.add({ instant, truePosition }) ;

                const Vector3d measurementError = positionMeasurementError_m * Vector3d::Random() ;
                const Position measuredPosition = Position::Meters(truePosition.getCoordinates() + measurementError, Frame::GCRF()) ;

                measuredStates.add({ instant, measuredPosition }) ;

            }

            if (measuredStates.getSize() >= maxMeasurementCount)
            {
                break ;
            }

            index++ ;

        }

        for (const auto& measuredState : measuredStates)
        {
            std::cout << measuredState.first.toString() << std::endl ;
        }

        for (const auto& scenarioConfigurationObject : configurationObject["scenarios"].accessArray())
        {

            BatchLeastSquares::Configuration configuration ;

            configuration.algorithm = scenarioConfigurationObject["algorithm"].accessString() ;

            if (scenarioConfigurationObject.accessDictionary().hasValueForKey("initialState"))
            {

                configuration.initialState(0) = scenarioConfigurationObject["initialState"][0].accessReal() ;
                configuration.initialState(1) = scenarioConfigurationObject["initialState"][1].accessReal() ;
                configuration.initialState(2) = scenarioConfigurationObject["initialState"][2].accessReal() ;
                configuration.initialState(3) = scenarioConfigurationObject["initialState"][3].accessReal() ;
                configuration.initialState(4) = scenarioConfigurationObject["initialState"][4].accessReal() ;
                configuration.initialState(5) = scenarioConfigurationObject["initialState"][5].accessReal() ;

            }

            if (scenarioConfigurationObject.accessDictionary().hasValueForKey("stateLowerBound"))
            {

                configuration.stateLowerBound(0) = scenarioConfigurationObject["stateLowerBound"][0].accessReal() ;
                configuration.stateLowerBound(1) = scenarioConfigurationObject["stateLowerBound"][1].accessReal() ;
                configuration.stateLowerBound(2) = scenarioConfigurationObject["stateLowerBound"][2].accessReal() ;
                configuration.stateLowerBound(3) = scenarioConfigurationObject["stateLowerBound"][3].accessReal() ;
                configuration.stateLowerBound(4) = scenarioConfigurationObject["stateLowerBound"][4].accessReal() ;
                configuration.stateLowerBound(5) = scenarioConfigurationObject["stateLowerBound"][5].accessReal() ;

            }

            if (scenarioConfigurationObject.accessDictionary().hasValueForKey("stateUpperBound"))
            {

                configuration.stateUpperBound(0) = scenarioConfigurationObject["stateUpperBound"][0].accessReal() ;
                configuration.stateUpperBound(1) = scenarioConfigurationObject["stateUpperBound"][1].accessReal() ;
                configuration.stateUpperBound(2) = scenarioConfigurationObject["stateUpperBound"][2].accessReal() ;
                configuration.stateUpperBound(3) = scenarioConfigurationObject["stateUpperBound"][3].accessReal() ;
                configuration.stateUpperBound(4) = scenarioConfigurationObject["stateUpperBound"][4].accessReal() ;
                configuration.stateUpperBound(5) = scenarioConfigurationObject["stateUpperBound"][5].accessReal() ;

            }

            if (scenarioConfigurationObject.accessDictionary().hasValueForKey("stoppingFunctionValue"))
            {
                configuration.stoppingFunctionValue = scenarioConfigurationObject["stoppingFunctionValue"].accessReal() ;
            }

            if (scenarioConfigurationObject.accessDictionary().hasValueForKey("relativeStateTolerance"))
            {
                configuration.relativeStateTolerance = scenarioConfigurationObject["relativeStateTolerance"].accessReal() ;
            }

            if (scenarioConfigurationObject.accessDictionary().hasValueForKey("relativeFunctionTolerance"))
            {
                configuration.relativeFunctionTolerance = scenarioConfigurationObject["relativeFunctionTolerance"].accessReal() ;
            }

            if (scenarioConfigurationObject.accessDictionary().hasValueForKey("absoluteFunctionTolerance"))
            {
                configuration.absoluteFunctionTolerance = scenarioConfigurationObject["absoluteFunctionTolerance"].accessReal() ;
            }

            if (scenarioConfigurationObject.accessDictionary().hasValueForKey("maximumEvaluationCount"))
            {
                configuration.maximumEvaluationCount = scenarioConfigurationObject["maximumEvaluationCount"].accessInteger() ;
            }

            if (scenarioConfigurationObject.accessDictionary().hasValueForKey("maximumDuration"))
            {
                configuration.maximumDuration = Duration::Parse(scenarioConfigurationObject["maximumDuration"].accessString()) ;
            }

            if (scenarioConfigurationObject.accessDictionary().hasValueForKey("verbose"))
            {
                configuration.verbose = scenarioConfigurationObject["verbose"].accessBoolean() ;
            }

            const BatchLeastSquares leastSquaresOrbitDeterminator = { environment, configuration } ;

            const BatchLeastSquares::Output estimatedOrbitOutput = leastSquaresOrbitDeterminator.estimateOrbit(measuredStates) ;

            const Orbit& estimatedOrbit = estimatedOrbitOutput.orbit ;

            bool isOk = true ;

            Real maxMeasuredDeviation = Real::Undefined() ;
            Real maxTrueDeviation = Real::Undefined() ;

            if (estimatedOrbit.isDefined())
            {

                ASSERT_TRUE(estimatedOrbit.isDefined()) ;

                for (const auto& measuredState : measuredStates)
                {

                    const auto [ instant, measuredPosition ] = measuredState ;

                    const State estimatedState = estimatedOrbit.getStateAt(instant) ;

                    // std::cout << instant.toString() << std::endl ;
                    // std::cout << measuredPosition << std::endl ;
                    // std::cout << estimatedState << std::endl ;

                    // ASSERT_TRUE(estimatedState.isDefined()) ;

                    // ASSERT_EQ(*measuredPosition.accessFrame(), *estimatedState.getPosition().accessFrame()) ;
                    // ASSERT_EQ(*measuredPosition.accessFrame(), *estimatedState.getVelocity().accessFrame()) ;

                    const String positionReport = String::Format
                    (
                        "{}: {} ≈ {} Δ {} > {} [m]",
                        instant.toString(),
                        estimatedState.getPosition().accessCoordinates().toString(12),
                        measuredPosition.accessCoordinates().toString(12),
                        Real((estimatedState.getPosition().accessCoordinates() - measuredPosition.accessCoordinates()).norm()).toString(12),
                        positionTolerance_m.toString(12)
                    ) ;

                    // ASSERT_TRUE(estimatedState.getPosition().accessCoordinates().isNear(measuredPosition.accessCoordinates(), positionTolerance_m)) << positionReport ;

                    const Real deviation = (estimatedState.getPosition().accessCoordinates() - measuredPosition.accessCoordinates()).norm() ;

                    if ((!maxMeasuredDeviation.isDefined()) || (deviation > maxMeasuredDeviation))
                    {
                        maxMeasuredDeviation = deviation ;
                    }

                    // if (!estimatedState.getPosition().accessCoordinates().isNear(measuredPosition.accessCoordinates(), positionTolerance_m))
                    // {
                    //     isOk = false ;
                    // }

                }

                for (const auto& trueState : trueStates)
                {

                    const auto [ instant, truePosition ] = trueState ;

                    const State estimatedState = estimatedOrbit.getStateAt(instant) ;

                    // std::cout << instant.toString() << std::endl ;
                    // std::cout << truePosition << std::endl ;
                    // std::cout << estimatedState << std::endl ;

                    // ASSERT_TRUE(estimatedState.isDefined()) ;

                    // ASSERT_EQ(*truePosition.accessFrame(), *estimatedState.getPosition().accessFrame()) ;
                    // ASSERT_EQ(*truePosition.accessFrame(), *estimatedState.getVelocity().accessFrame()) ;

                    const String positionReport = String::Format
                    (
                        "{}: {} ≈ {} Δ {} > {} [m]",
                        instant.toString(),
                        estimatedState.getPosition().accessCoordinates().toString(12),
                        truePosition.accessCoordinates().toString(12),
                        Real((estimatedState.getPosition().accessCoordinates() - truePosition.accessCoordinates()).norm()).toString(12),
                        positionTolerance_m.toString(12)
                    ) ;

                    // ASSERT_TRUE(estimatedState.getPosition().accessCoordinates().isNear(truePosition.accessCoordinates(), positionTolerance_m)) << positionReport ;

                    const Real deviation = (estimatedState.getPosition().accessCoordinates() - truePosition.accessCoordinates()).norm() ;

                    if ((!maxTrueDeviation.isDefined()) || (deviation > maxTrueDeviation))
                    {
                        maxTrueDeviation = deviation ;
                    }

                    // if (!estimatedState.getPosition().accessCoordinates().isNear(truePosition.accessCoordinates(), positionTolerance_m))
                    // {
                    //     isOk = false ;
                    // }

                }

            }
            else
            {
                isOk = false ;
            }

            if (isOk)
            {

                std::cout << "OK!" << std::endl ;

                std::cout << scenarioConfigurationObject.accessDictionary() << std::endl ;

                std::cout << "Maximum deviation (measured): " << maxMeasuredDeviation.toString(12) << " [m]" << std::endl ;
                std::cout << "Maximum deviation (true): " << maxTrueDeviation.toString(12) << " [m]" << std::endl ;

                std::cout << "Residuals: " << estimatedOrbitOutput.residuals.toString(12) << " [m]" << std::endl ;
                std::cout << "Evaluation count: " << estimatedOrbitOutput.evaluationCount << std::endl ;
                std::cout << "Message: " << estimatedOrbitOutput.message << std::endl ;

            }

        }

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
