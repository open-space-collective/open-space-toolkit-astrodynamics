////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/CustomProp.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
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
#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <Global.test.hpp>

#include <boost/numeric/odeint.hpp>
#include <boost/array.hpp>
#include <boost/operators.hpp>

#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_CustomProp, Test_BasicProp)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::trajectory::orbit::models::CustomProp ;

    using namespace boost::numeric::odeint;


    // typedef std::vector< double > state_type; /* The type of container used to hold the state vector */

    {        
        // Initial propagation experimentation with CustomProp function without instantiating CustomProp

        double startEpoch = 0.0;
        double endEpoch = 11345.0;
        double initialTimeStep = 5.0;

        CustomProp::state_type xECI(6);
        xECI[0] = -2.577031509124861e+06; 
        xECI[1] = -1.530158746164186e+06;
        xECI[2] = -6.237029236139196e+06; 
        xECI[3] = -6.763265635655096e+03;
        xECI[4] = -1.356683663338969e+03; 
        xECI[5] = 3.132719003502420e+03;

        CustomProp::state_type xEndTwoBodyTrueECI(6); // Need to change these to match 2 body prop final state answer
        xEndTwoBodyTrueECI[0] = -2195455.00352205;
        xEndTwoBodyTrueECI[1] = -1451724.59333227;
        xEndTwoBodyTrueECI[2] = -6399889.71639439;
        xEndTwoBodyTrueECI[3] = -6923.342794492;
        xEndTwoBodyTrueECI[4] = -1456.66369389355;
        xEndTwoBodyTrueECI[5] = 2708.90840312134;

        integrate ( CustomProp::TwoBodyDynamics , xECI , startEpoch , endEpoch , initialTimeStep , CustomProp::PropLog );
        
        std::cout << "Answer is:" << std::endl;
        std::cout << "Pos (" << xECI[0] << ", " << xECI[1] << ", " << xECI[2] << ")" << std::endl;
        std::cout << "Vel (" << xECI[3] << ", " << xECI[4] << ", " << xECI[5] << ")" << std::endl;
        
        double posErrTolerance = 10.0; 
        double velErrTolerance = 0.1; 
        double statePosError = sqrt(pow(xEndTwoBodyTrueECI[0]-xECI[0],2) + pow(xEndTwoBodyTrueECI[1]-xECI[1],2) + pow(xEndTwoBodyTrueECI[2]-xECI[2],2));
        double stateVelError = sqrt(pow(xEndTwoBodyTrueECI[3]-xECI[3],2) + pow(xEndTwoBodyTrueECI[4]-xECI[4],2) + pow(xEndTwoBodyTrueECI[5]-xECI[5],2));

        std::cout << "Position error is: " << statePosError << std::endl;
        std::cout << "Velocity error is: " << stateVelError << std::endl;

        ASSERT_GT(posErrTolerance,statePosError) ; // Assert position error
        ASSERT_GT(velErrTolerance,stateVelError) ; // Assert velocity error




        // // Propagation experimentation by instantiating CustomProp

        // // Environment setup

        // const Environment environment = Environment::Default() ;

        // // Orbital model setup

        // const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        // const Real eccentricity = 0.0 ;
        // const Angle inclination = Angle::Degrees(45.0) ;
        // const Angle raan = Angle::Degrees(0.0) ;
        // const Angle aop = Angle::Degrees(0.0) ;
        // const Angle trueAnomaly = Angle::Degrees(0.0) ;

        // const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        // const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;
        // const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        // const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;
        // const Real J2 = Earth::Models::EGM2008::J2 ;
        // const Real J4 = Earth::Models::EGM2008::J4 ;

        // const Kepler keplerianModel = { coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None } ;

        // // Orbit setup

        // const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

        // // Reference data setup

        // const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_1/Satellite Orbit.csv")), Table::Format::CSV, true) ;

        // // Orbit test

        // for (const auto& referenceRow : referenceData)
        // {

        //     const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC) ;

        //     const Vector3d referencePosition_GCRF = { referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal() } ;
        //     const Vector3d referenceVelocity_GCRF = { referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal() } ;

        //     const Vector3d referencePosition_ITRF = { referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal() } ;
        //     const Vector3d referenceVelocity_ITRF = { referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal() } ;

        //     const Real referenceRevolutionNumber = referenceRow[13].accessReal() ;

        //     const State state_GCRF = orbit.getStateAt(instant) ;

        //     const Position position_GCRF = state_GCRF.accessPosition() ;
        //     const Velocity velocity_GCRF = state_GCRF.accessVelocity() ;

        //     ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
        //     ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

        //     ASSERT_GT(1e-3, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm()) ;
        //     ASSERT_GT(1e-6, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm()) ;

        //     const Shared<const Frame> itrfFrame = Frame::ITRF() ;

        //     const State state_ITRF = state_GCRF.inFrame(itrfFrame) ;

        //     const Position position_ITRF = state_ITRF.accessPosition() ;
        //     const Velocity velocity_ITRF = state_ITRF.accessVelocity() ;

        //     ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame()) ;
        //     ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame()) ;

        //     ASSERT_GT(1e-1, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm()) ;
        //     ASSERT_GT(1e-0, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm()) ;

        //     ASSERT_EQ(referenceRevolutionNumber.floor(), orbit.getRevolutionNumberAt(instant)) ;

        // }


    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, Test_1)
// {

//     using ostk::core::types::Shared ;
//     using ostk::core::types::Real ;
//     using ostk::core::ctnr::Array ;
//     using ostk::core::ctnr::Table ;
//     using ostk::core::fs::Path ;
//     using ostk::core::fs::File ;

//     using ostk::math::obj::Vector3d ;

//     using ostk::physics::units::Length ;
//     using ostk::physics::units::Angle ;
//     using ostk::physics::units::Derived ;
//     using ostk::physics::time::Scale ;
//     using ostk::physics::time::Instant ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::time::Interval ;
//     using ostk::physics::time::DateTime ;
//     using ostk::physics::coord::Frame ;
//     using ostk::physics::coord::Position ;
//     using ostk::physics::coord::Velocity ;
//     using ostk::physics::Environment ;
//     using ostk::physics::env::obj::celest::Earth ;

//     using ostk::astro::trajectory::Orbit ;
//     using ostk::astro::trajectory::State ;
//     using ostk::astro::trajectory::orbit::models::Kepler ;
//     using ostk::astro::trajectory::orbit::models::kepler::COE ;

//     {

//         // Environment setup

//         const Environment environment = Environment::Default() ;

//         // Orbital model setup

//         const Length semiMajorAxECIis = Length::Kilometers(7000.0) ;
//         const Real eccentricity = 0.0 ;
//         const Angle inclination = Angle::Degrees(45.0) ;
//         const Angle raan = Angle::Degrees(0.0) ;
//         const Angle aop = Angle::Degrees(0.0) ;
//         const Angle trueAnomaly = Angle::Degrees(0.0) ;

//         const COE coe = { semiMajorAxECIis, eccentricity, inclination, raan, aop, trueAnomaly } ;

//         const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;
//         const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
//         const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;
//         const Real J2 = Earth::Models::EGM2008::J2 ;
//         const Real J4 = Earth::Models::EGM2008::J4 ;

//         const Kepler keplerianModel = { coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None } ;

//         // Orbit setup

//         const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

//         // Reference data setup

//         const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_1/Satellite Orbit.csv")), Table::Format::CSV, true) ;

//         // Orbit test

//         for (const auto& referenceRow : referenceData)
//         {

//             const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC) ;

//             const Vector3d referencePosition_GCRF = { referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal() } ;
//             const Vector3d referenceVelocity_GCRF = { referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal() } ;

//             const Vector3d referencePosition_ITRF = { referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal() } ;
//             const Vector3d referenceVelocity_ITRF = { referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal() } ;

//             const Real referenceRevolutionNumber = referenceRow[13].accessReal() ;

//             const State state_GCRF = orbit.getStateAt(instant) ;

//             const Position position_GCRF = state_GCRF.accessPosition() ;
//             const Velocity velocity_GCRF = state_GCRF.accessVelocity() ;

//             ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
//             ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

//             ASSERT_GT(1e-3, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm()) ;
//             ASSERT_GT(1e-6, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm()) ;

//             const Shared<const Frame> itrfFrame = Frame::ITRF() ;

//             const State state_ITRF = state_GCRF.inFrame(itrfFrame) ;

//             const Position position_ITRF = state_ITRF.accessPosition() ;
//             const Velocity velocity_ITRF = state_ITRF.accessVelocity() ;

//             ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame()) ;
//             ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame()) ;

//             ASSERT_GT(1e-1, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm()) ;
//             ASSERT_GT(1e-0, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm()) ;

//             ASSERT_EQ(referenceRevolutionNumber.floor(), orbit.getRevolutionNumberAt(instant)) ;

//             // std::cout << "xECI @ GCRF = " << referencePosition_GCRF.toString(10) << " / " << position_GCRF.accessCoordinates().toString(10) << std::endl ;
//             // std::cout << "xECI @ ITRF = " << referencePosition_ITRF.toString(10) << " / " << position_ITRF.accessCoordinates().toString(10) << std::endl ;
//             // std::cout << "dxECI = " << (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm() << " - " << (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm() << std::endl ;

//             // std::cout << "v @ GCRF = " << referenceVelocity_GCRF.toString(10) << " / " << velocity_GCRF.accessCoordinates().toString(10) << std::endl ;
//             // std::cout << "v @ ITRF = " << referenceVelocity_ITRF.toString(10) << " / " << velocity_ITRF.accessCoordinates().toString(10) << std::endl ;
//             // std::cout << "dv = " << (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm() << " - " << (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm() << std::endl ;

//             // std::cout << "dxECI | dv = " << Real((position_GCRF.accessCoordinates() - referencePosition_GCRF).norm()).toString(12) << " - " << Real((position_ITRF.accessCoordinates() - referencePosition_ITRF).norm()).toString(12) << " | " << Real((velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm()).toString(12) << " - " << Real((velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm()).toString(12) << std::endl ;

//         }

//     }

// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, Constructor)
// {

//     using ostk::core::types::Shared ;
//     using ostk::core::types::Real ;
//     using ostk::core::ctnr::Array ;
//     using ostk::core::ctnr::Table ;
//     using ostk::core::fs::Path ;
//     using ostk::core::fs::File ;

//     using ostk::math::obj::Vector3d ;

//     using ostk::physics::units::Length ;
//     using ostk::physics::units::Angle ;
//     using ostk::physics::units::Derived ;
//     using ostk::physics::time::Scale ;
//     using ostk::physics::time::Instant ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::time::Interval ;
//     using ostk::physics::time::DateTime ;
//     using ostk::physics::coord::Frame ;
//     using ostk::physics::coord::Position ;
//     using ostk::physics::coord::Velocity ;
//     using ostk::physics::env::obj::celest::Earth ;

//     using ostk::astro::trajectory::Orbit ;
//     using ostk::astro::trajectory::State ;
//     using ostk::astro::trajectory::orbit::models::Kepler ;
//     using ostk::astro::trajectory::orbit::models::kepler::COE ;

//     // {

//     //     const Earth earth = Earth::Analytical() ;

//     //     const Length semiMajorAxECIis = Length::Kilometers(7000.0) ;
//     //     const Real eccentricity = 0.0 ;
//     //     const Angle inclination = Angle::Degrees(0.0) ;
//     //     const Angle raan = Angle::Degrees(0.0) ;
//     //     const Angle aop = Angle::Degrees(0.0) ;
//     //     const Angle trueAnomaly = Angle::Degrees(0.0) ;

//     //     const COE coe = { semiMajorAxECIis, eccentricity, inclination, raan, aop, trueAnomaly } ;

//     //     std::cout << coe << std::endl ;

//     //     const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;

//     //     const Kepler keplerianModel = { coe, epoch, earth, Kepler::PerturbationType::None } ;

//     //     std::cout << keplerianModel.getClassicalOrbitalElements() << std::endl ;

//     // }

//     {

//         const Earth earth = Earth::Analytical() ;

//         const Length semiMajorAxECIis = Length::Kilometers(7000.0) ;
//         const Real eccentricity = 0.0 ;
//         const Angle inclination = Angle::Degrees(0.0) ;
//         const Angle raan = Angle::Degrees(0.0) ;
//         const Angle aop = Angle::Degrees(0.0) ;
//         const Angle trueAnomaly = Angle::Degrees(0.0) ;

//         const COE coe = { semiMajorAxECIis, eccentricity, inclination, raan, aop, trueAnomaly } ;

//         std::cout << coe << std::endl ;

//         const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;

//         const Kepler keplerianModel = { coe, epoch, earth, Kepler::PerturbationType::None, true } ;

//         std::cout << keplerianModel.getClassicalOrbitalElements() << std::endl ;

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, EqualToOperator)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, NotEqualToOperator)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, StreamOperator)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, IsDefined)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetClassicalOrbitalElements)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetEpoch)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetRevolutionNumberAtEpoch)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetGravitationalParameter)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetEquatorialRadius)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetJ2)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetJ4)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetPerturbationType)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, CalculateStateAt)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, CalculateRevolutionNumberAt)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, StringFromPerturbationType)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
