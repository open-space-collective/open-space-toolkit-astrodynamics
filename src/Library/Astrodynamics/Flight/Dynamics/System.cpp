////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Flight/Dynamics/System.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Flight/Dynamics/System.hpp>

#include <Library/Physics/Time/Duration.hpp>

#include <Library/Mathematics/Objects/Vector.hpp>

#include <Library/Core/Error.hpp>
#include <Library/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace flight
{
namespace dynamics
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                System::System                              (   const   Mass&                       aMass                                       )
                                :   mass_(aMass),
                                    instant_(Instant::Undefined()),
                                    state_({})
{

}

                                System::System                              (   const   Mass&                       aMass,
                                                                                const   Instant&                    anInstant,
                                                                                const   std::vector<double>&        aState                                      )
                                :   mass_(aMass),
                                    instant_(anInstant),
                                    state_(aState)
{

}

                                System::System                              (   const   System&                     aSystem                                     )
                                :   mass_(aSystem.mass_),
                                    instant_(aSystem.instant_),
                                    state_(aSystem.state_)
{

}

                                System::~System                             ( )
{

}

System&                         System::operator =                          (   const   System&                     aSystem                                     )
{

    if (this != &aSystem)
    {

        mass_ = aSystem.mass_ ;
        instant_ = aSystem.instant_ ;
        state_ = aSystem.state_ ;

    }

    return *this ;

}

void                            System::operator ()                         (   const   std::vector<double>&        aState,
                                                                                        std::vector<double>&        aDerivative,
                                                                                const   double                      aTime                                       ) const
{

    using library::math::obj::Vector3d ;

    using library::physics::time::Duration ;

    // std::cout << "System :: Time: " << aTime << " [s]" << std::endl ;

    const Instant instant = instant_ + Duration::Seconds(aTime) ;

    // std::cout << "System :: Instant: " << instant.toString() << std::endl ;

    (void) instant ; // [TBR]

    const Vector3d x_GCRF = { aState[0], aState[1], aState[2] } ;
    const Vector3d v_GCRF = { aState[3], aState[4], aState[5] } ;

    // std::cout << "x_GCRF = " << x_GCRF.toString() << std::endl ;
    // std::cout << "v_GCRF = " << v_GCRF.toString() << std::endl ;

    // coord::State                state_GCRF(time, x_GCRF, v_GCRF, Quaternion::Unit(), Vector3d::Zero(), Frame::Type::GCRF) ;

    // environmentPtr_->setTime(time) ;

    // const Vector3d environmentForce_GCRF = { 0.0, 0.0, 0.0 } ; // environmentPtr_->calculateTotalForce_GCRF(state_GCRF, mass_) ;
    const Vector3d environmentForce_GCRF = - (398600441500000.0 * mass_.inKilograms()) / std::pow(x_GCRF.norm(), 2) * x_GCRF.normalized() ;

    const Vector3d force_GCRF = environmentForce_GCRF ;

    const Vector3d acceleration_GCRF = (1.0 / mass_.inKilograms()) * force_GCRF ;

    // std::cout << "acceleration_GCRF = " << acceleration_GCRF.toString() << std::endl ;

    aDerivative[0] = v_GCRF.x() ; // dx_dt_x
    aDerivative[1] = v_GCRF.y() ; // dx_dt_y
    aDerivative[2] = v_GCRF.z() ; // dx_dt_z

    aDerivative[3] = acceleration_GCRF.x() ; // dv_dt_x
    aDerivative[4] = acceleration_GCRF.y() ; // dv_dt_y
    aDerivative[5] = acceleration_GCRF.z() ; // dv_dt_z

}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   System&                     aSystem                                     )
{

    aSystem.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            System::isDefined                           ( ) const
{
    return mass_.isDefined() ;
}

Mass                            System::getMass                             ( ) const
{
    return mass_ ;
}

Instant                         System::getInstant                          ( ) const
{
    return instant_ ;
}

std::vector<double>             System::getState                            ( ) const
{
    return state_ ;
}

void                            System::print                               (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    using library::core::types::String ;

    displayDecorator ? library::core::utils::Print::Header(anOutputStream, "System") : void () ;

    library::core::utils::Print::Line(anOutputStream) << "Instant:" << instant_.toString() ;

    String stateString = String::Empty() ;

    for (const auto& stateItem : state_)
    {

        if (!stateString.isEmpty())
        {
            stateString += ", " ;
        }

        stateString += String::Format("[{}]", Real(stateItem).toString(12)) ;

    }

    library::core::utils::Print::Line(anOutputStream) << "State:" << stateString ;

    displayDecorator ? library::core::utils::Print::Footer(anOutputStream) : void () ;

}

std::vector<double>&            System::accessState                         ( )
{
    return state_ ;
}

void                            System::setInstant                          (   const   Instant&                    anInstant                                   )
{
    instant_ = anInstant ;
}

void                            System::setState                            (   const   std::vector<double>&        aState                                      )
{
    state_ = aState ;
}

System                          System::Undefined                           ( )
{
    return { Mass::Undefined(), Instant::Undefined(), {} } ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
