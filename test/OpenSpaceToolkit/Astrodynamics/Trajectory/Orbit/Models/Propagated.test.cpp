////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp>

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

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, Constructor_Propagated)
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

//     //     const Length semiMajorAxis = Length::Kilometers(7000.0) ;
//     //     const Real eccentricity = 0.0 ;
//     //     const Angle inclination = Angle::Degrees(0.0) ;
//     //     const Angle raan = Angle::Degrees(0.0) ;
//     //     const Angle aop = Angle::Degrees(0.0) ;
//     //     const Angle trueAnomaly = Angle::Degrees(0.0) ;

//     //     const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

//     //     std::cout << coe << std::endl ;

//     //     const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;

//     //     const Kepler keplerianModel = { coe, epoch, earth, Kepler::PerturbationType::None } ;

//     //     std::cout << keplerianModel.getClassicalOrbitalElements() << std::endl ;

//     // }

//     {

//         const Earth earth = Earth::Analytical() ;

//         const Length semiMajorAxis = Length::Kilometers(7000.0) ;
//         const Real eccentricity = 0.0 ;
//         const Angle inclination = Angle::Degrees(0.0) ;
//         const Angle raan = Angle::Degrees(0.0) ;
//         const Angle aop = Angle::Degrees(0.0) ;
//         const Angle trueAnomaly = Angle::Degrees(0.0) ;

//         const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

//         std::cout << coe << std::endl ;

//         const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;

//         const Kepler keplerianModel = { coe, epoch, earth, Kepler::PerturbationType::None, true } ;

//         std::cout << keplerianModel.getClassicalOrbitalElements() << std::endl ;

//     }

// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, Test_1_Propagated)
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

    using ostk::astro::trajectory::orbit::models::Propagated ;

    using namespace boost::numeric::odeint;

    {        
        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup
        
        const Instant epochInstant = Instant::DateTime(DateTime::Parse("2021-06-30 17:26:46.742"), Scale::UTC) ; // Launch time
        
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2022-03-05 13:26:46.742"), Scale::UTC) ;

        Duration propDuration = Duration::Seconds(11345.0) ;

        std::vector< double > xStart(6);
        xStart[0] = -1260836.090852064; xStart[1] = 588280.4487875753; xStart[2] = 6745839.542136152; 
        xStart[3] = 7231.630244016249; xStart[4] = 2060.732438433088;  xStart[5] = 1167.816624957812;

        const Vector3d propPosition_refGCRF = {-1575658.863277743, 497462.0623771324, 6686886.954716642};
        const Vector3d propVelocity_refGCRF = {7155.923407265891, 2089.711281165372, 1526.340363065099};

        const State state = { startInstant, Position::Meters({ xStart[0], xStart[1], xStart[2] }, gcrfSPtr), Velocity::MetersPerSecond({ xStart[3], xStart[4], xStart[5] }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel = { state, epochInstant, gravitationalParameter, equatorialRadius, Propagated::GravPerturbationType::None, Propagated::AtmosPerturbationType::None, Propagated::ThirdBodyPerturbationType::None , false } ;
        std::cout << propagatedModel << std::endl;
        
        // Orbit setup

        const Orbit orbit = { propagatedModel, environment.accessCelestialObjectWithName("Earth") } ;

        const State state_GCRF = orbit.getStateAt(startInstant + propDuration) ;
        
        const int revNumber = orbit.getRevolutionNumberAt(startInstant + propDuration) ;
        
        // Results check 

        const Position position_GCRF = state_GCRF.accessPosition() ;
        const Velocity velocity_GCRF = state_GCRF.accessVelocity() ;

        const Real statePositionError_GCRF = (position_GCRF.accessCoordinates() - propPosition_refGCRF).norm() ;
        const Real stateVelocityError_GCRF = (velocity_GCRF.accessCoordinates() - propVelocity_refGCRF).norm() ;


        // Results console output

        std::cout << "**************************************" << std::endl;
        std::cout.setf(std::ios::scientific,std::ios::floatfield);
        std::cout << "Position error is: " << statePositionError_GCRF << "m" << std::endl;
        std::cout << "Velocity error is: " << stateVelocityError_GCRF <<  "m/s" << std::endl;
        std::cout.setf(std::ios::fixed,std::ios::floatfield);
        std::cout << "**************************************" << std::endl;
        

        // ASSERTS
        ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
        ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

        ASSERT_GT(1e-3, statePositionError_GCRF) ;
        ASSERT_GT(1e-6, stateVelocityError_GCRF) ;

        ASSERT_EQ(3763 , revNumber) ;

       



        // // Reference data setup

        // const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_1/Satellite Orbit.csv")), Table::Format::CSV, true) ;

        // // Orbit test

        // for (const auto& referenceRow : referenceData)
        // {

        //     const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC) ;

        //     const Vector3d propPosition_refGCRF = { referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal() } ;
        //     const Vector3d propVelocity_refGCRF = { referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal() } ;

        //     const Vector3d referencePosition_ITRF = { referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal() } ;
        //     const Vector3d referenceVelocity_ITRF = { referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal() } ;

        //     const Real referenceRevolutionNumber = referenceRow[13].accessReal() ;

        //     const State state_GCRF = orbit.getStateAt(instant) ;

        //     const Position position_GCRF = state_GCRF.accessPosition() ;
        //     const Velocity velocity_GCRF = state_GCRF.accessVelocity() ;

        //     ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
        //     ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

        //     ASSERT_GT(1e-3, (position_GCRF.accessCoordinates() - propPosition_refGCRF).norm()) ;
        //     ASSERT_GT(1e-6, (velocity_GCRF.accessCoordinates() - propVelocity_refGCRF).norm()) ;

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

