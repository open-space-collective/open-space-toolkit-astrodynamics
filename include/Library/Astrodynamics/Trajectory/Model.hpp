////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Model.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Trajectory_Model__
#define __Library_Astrodynamics_Trajectory_Model__

#include <Library/Astrodynamics/Trajectory/State.hpp>

#include <Library/Physics/Time/Instant.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::physics::time::Instant ;

using library::astro::trajectory::State ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Trajectory model (abstract)

class Model
{

    public:

                                Model                                       ( ) ;

        virtual                 ~Model                                      ( ) = 0 ;

        virtual Model*          clone                                       ( ) const = 0 ;

        virtual bool            operator ==                                 (   const   Model&                      aModel                                      ) const = 0 ;

        virtual bool            operator !=                                 (   const   Model&                      aModel                                      ) const = 0 ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Model&                      aModel                                      ) ;

        virtual bool            isDefined                                   ( ) const = 0 ;

        /// @brief              Returns true if model can be converted to type
        ///
        /// @return             True if model can be converted to type

        template <class Type>
        bool                    is                                          ( ) const
        {
            return dynamic_cast<const Type*>(this) != nullptr ;
        }

        /// @brief              Access model as its underlying type
        ///
        /// @return             Reference to underlying type

        template <class Type>
        const Type&             as                                          ( ) const
        {

            const Type* modelPtr = dynamic_cast<const Type*>(this) ;

            if (modelPtr == nullptr)
            {
                throw library::core::error::RuntimeError("Cannot convert model to underlying type.") ;
            }

            return *modelPtr ;

        }

        virtual State           calculateStateAt                            (   const   Instant&                    anInstant                                   ) const = 0 ;

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const = 0 ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////