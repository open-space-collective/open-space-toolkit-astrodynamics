////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Models/SGP4.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Models/SGP4.hpp>

#include <Library/Physics/Coordinate/Transform.hpp>

#include <Library/Core/Error.hpp>
#include <Library/Core/Utilities.hpp>

#include <sgp4/SGP4.h>

#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace models
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::physics::coord::Transform ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SGP4::Impl
{

    public:

                                Impl                                        (   const   TLE&                        aTle                                        ) ;

                                Impl                                        (   const   SGP4::Impl&                 anImpl                                      ) = delete ;

        SGP4::Impl&             operator =                                  (   const   SGP4::Impl&                 anImpl                                      ) = delete ;

        State                   calculateStateAt                            (   const   Instant&                    anInstant                                   ) const ;

    private:

        const TLE&              tle_ ;
        ::SGP4                  sgp4_ ;

        Shared<const Frame>     temeFrameOfEpochSPtr_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                SGP4::Impl::Impl                            (   const   TLE&                        aTle                                        )
                                :   tle_(aTle),
                                    sgp4_(::Tle(tle_.getSatelliteName(), tle_.getFirstLine(), tle_.getSecondLine())),
                                    temeFrameOfEpochSPtr_(Frame::TEMEOfEpoch(tle_.getEpoch()))
{

}

State                           SGP4::Impl::calculateStateAt                (   const   Instant&                    anInstant                                   ) const
{

    using library::math::obj::Vector3d ;

    using library::physics::time::Duration ;

    const Real durationFromEpoch_min = Duration::Between(tle_.getEpoch(), anInstant).inMinutes() ;

    const ::Eci xv_TEME = sgp4_.FindPosition(durationFromEpoch_min) ;

    const ::Vector x_TEME_km = xv_TEME.Position() ;
    const ::Vector v_TEME_kmps = xv_TEME.Velocity() ;

    const Vector3d x_TEME_m = Vector3d(x_TEME_km.x, x_TEME_km.y, x_TEME_km.z) * 1e3 ;
    const Vector3d v_TEME_mps = Vector3d(v_TEME_kmps.x, v_TEME_kmps.y, v_TEME_kmps.z) * 1e3 ;

    const Position position_TEME = { x_TEME_m, Position::Unit::Meter, temeFrameOfEpochSPtr_ } ;
    const Velocity velocity_TEME = { v_TEME_mps, Velocity::Unit::MeterPerSecond, temeFrameOfEpochSPtr_ } ;

    const State state_TEME = { anInstant, position_TEME, velocity_TEME } ;

    static const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

    const State state_GCRF = state_TEME.inFrame(gcrfSPtr) ;

    return state_GCRF ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                SGP4::SGP4                                  (   const   TLE&                        aTle                                        )
                                :   Model(),
                                    tle_(aTle),
                                    implUPtr_(std::make_unique<SGP4::Impl>(tle_))
{

}

                                SGP4::SGP4                                  (   const   SGP4&                       aSGP4Model                                  )
                                :   Model(aSGP4Model),
                                    tle_(aSGP4Model.tle_),
                                    implUPtr_(std::make_unique<SGP4::Impl>(tle_))
{

}

                                SGP4::~SGP4                                 ( )
{

}

SGP4&                           SGP4::operator =                            (   const   SGP4&                       aSGP4Model                                  )
{

    if (this != &aSGP4Model)
    {

        Model::operator = (aSGP4Model) ;

        tle_ = aSGP4Model.tle_ ;

        implUPtr_ = std::make_unique<SGP4::Impl>(tle_) ;

    }

    return *this ;

}

SGP4*                           SGP4::clone                                 ( ) const
{
    return new SGP4(*this) ;
}

bool                            SGP4::operator ==                           (   const   SGP4&                       aSGP4Model                                  ) const
{

    if ((!this->isDefined()) || (!aSGP4Model.isDefined()))
    {
        return false ;
    }

    return tle_ == aSGP4Model.tle_ ;

}

bool                            SGP4::operator !=                           (   const   SGP4&                       aSGP4Model                                  ) const
{
    return !((*this) == aSGP4Model) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   SGP4&                       aSGP4Model                                  )
{

    aSGP4Model.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            SGP4::isDefined                             ( ) const
{
    return tle_.isDefined() && (implUPtr_ != nullptr) ;
}

TLE                             SGP4::getTle                                ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("SGP4") ;
    }

    return tle_ ;

}

Instant                         SGP4::getEpoch                              ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("SGP4") ;
    }

    return tle_.getEpoch() ;

}

Integer                         SGP4::getRevolutionNumberAtEpoch            ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("SGP4") ;
    }

    return tle_.getRevolutionNumberAtEpoch() ;

}

State                           SGP4::calculateStateAt                      (   const   Instant&                    anInstant                                   ) const
{

    if (!anInstant.isDefined())
    {
        throw library::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("SGP4") ;
    }

    return implUPtr_->calculateStateAt(anInstant) ;

}

Integer                         SGP4::calculateRevolutionNumberAt           (   const   Instant&                    anInstant                                   ) const
{

    if (!anInstant.isDefined())
    {
        throw library::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("SGP4") ;
    }

    if (anInstant == tle_.getEpoch())
    {
        return this->getRevolutionNumberAtEpoch() ;
    }

    // aaaa

    return Integer::Undefined() ;

}

void                            SGP4::print                                 (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? library::core::utils::Print::Header(anOutputStream, "SGP4") : void () ;

    library::core::utils::Print::Line(anOutputStream) << "Epoch:"               << (this->getEpoch().isDefined() ? this->getEpoch().toString() : "Undefined") ;

    library::core::utils::Print::Separator(anOutputStream, "Two-Line Elements") ;

    // tle_.print(anOutputStream, false) ;

    displayDecorator ? library::core::utils::Print::Footer(anOutputStream) : void () ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool                            SGP4::operator ==                           (   const   trajectory::Model&          aModel                                      ) const
{

    const SGP4* aSGP4ModelPtr = dynamic_cast<const SGP4*>(&aModel) ;

    return (aSGP4ModelPtr != nullptr) && this->operator == (*aSGP4ModelPtr) ;

}

bool                            SGP4::operator !=                           (   const   trajectory::Model&          aModel                                      ) const
{
    return !((*this) == aModel) ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
