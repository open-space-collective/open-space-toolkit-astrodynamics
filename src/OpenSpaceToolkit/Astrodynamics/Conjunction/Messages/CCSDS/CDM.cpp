////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Conjunction/Messages/CCSDS/CDM.cpp
/// @author         Remy Derollez <remy@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/Messages/CCSDS/CDM.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Core/Containers/Object.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace conjunction
{
namespace messages
{
namespace ccsds
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::math::obj::Vector3d ;
using ostk::math::obj::MatrixXd ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vector3d                        vector3dFromObject                          (   const   ctnr::Object&               anObject                                    )
{
    return
    {
        anObject.accessArray()[0].accessReal(),
        anObject.accessArray()[1].accessReal(),
        anObject.accessArray()[2].accessReal(),
    } ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                CDM::CDM                                    (   const   CDM::Header&                aHeader,
                                                                                const   CDM::RelativeMetadata&      aRelativeMetadata,
                                                                                const   Array<CDM::Metadata>&       aMetadataArray,
                                                                                const   Array<CDM::Data>            aDataArray                                  )
                                :   header_(aHeader),
                                    relativeMetadata_(aRelativeMetadata),
                                    objectsMetadata_(aMetadataArray),
                                    objectsData_(aDataArray)
{

}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   CDM&                        anCDM                                       )
{

    anCDM.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            CDM::isDefined                              ( ) const
{
    return true ;  // TBI
}

CDM::Header                     CDM::getHeader                              ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->header_ ;

}

CDM::Header                     CDM::getHeader                              ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->header_ ;

}

Array<CDM::Deployment>          CDM::getDeployments                         ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->deployments_ ;

}

CDM::Deployment                 CDM::getDeploymentAt                        (   const   Size&                       anIndex                                     ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->deployments_.at(anIndex) ;

}

CDM::Deployment                 CDM::getDeploymentWithName                  (   const   String&                     aName                                       ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    for (const auto& deployment : this->deployments_)
    {
        if (deployment.name == aName)
        {
            return deployment ;
        }
    }

    throw ostk::core::error::RuntimeError("Deployment with name [{}] not found.", aName) ;

}

void                            CDM::print                                  (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    using ostk::core::types::String ;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Orbit Parameter Message") : void () ;

    ostk::core::utils::Print::Line(anOutputStream) << "Generation date:" << (this->header_.generationDate.isDefined() ? this->header_.generationDate.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Launch date:" << (this->header_.launchDate.isDefined() ? this->header_.launchDate.toString() : "Undefined") ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

CDM                             CDM::Undefined                              ( )
{
    return { { Instant::Undefined(), Instant::Undefined() }, Array<Deployment>::Empty() } ;
}

CDM                             CDM::Dictionary                             (   const   ctnr::Dictionary&           aDictionary                                 )
{

    using ostk::physics::time::DateTime ;
    using ostk::physics::time::Scale ;
    using ostk::physics::coord::Frame ;

    Array<CDM::Deployment> deployments = Array<CDM::Deployment>::Empty() ;

    for (const auto& deploymentObject : aDictionary["deployments"].accessArray())
    {

        const CDM::Deployment deployment =
        {
            deploymentObject["name"].accessString(),
            deploymentObject["sequence_number"].accessInteger(),
            Duration::Seconds(deploymentObject["mission_time_s"].accessReal()),
            Instant::DateTime(DateTime::Parse(deploymentObject["date"].getString().replace("Z", ""), DateTime::Format::ISO8601), Scale::UTC),
            Position::Meters(vector3dFromObject(deploymentObject["r_ecef_m"]), Frame::ITRF()),
            Velocity::MetersPerSecond(vector3dFromObject(deploymentObject["v_ecef_m_per_s"]), Frame::ITRF()),
            Length::Kilometers(deploymentObject["mean_perigee_altitude_km"].accessReal()),
            Length::Kilometers(deploymentObject["mean_apogee_altitude_km"].accessReal()),
            Angle::Degrees(deploymentObject["mean_inclination_deg"].accessReal()),
            Angle::Degrees(deploymentObject["mean_argument_of_perigee_deg"].accessReal()),
            Angle::Degrees(deploymentObject["mean_longitude_ascending_node_deg"].accessReal()),
            Angle::Degrees(deploymentObject["mean_mean_anomaly_deg"].accessReal()),
            deploymentObject["ballistic_coef_kg_per_m2"].accessReal()
        } ;

        deployments.add(deployment) ;

    }

    return CDM
    {
        CDM::Header
        {
            Instant::DateTime(DateTime::Parse(aDictionary["header"]["generation_date"].getString().replace("Z", ""), DateTime::Format::ISO8601), Scale::UTC),
            Instant::DateTime(DateTime::Parse(aDictionary["header"]["launch_date"].getString().replace("Z", ""), DateTime::Format::ISO8601), Scale::UTC),
        },
        deployments
    } ;

}

CDM                             CDM::Parse                                  (   const   String&                     aString                                     )
{

    using ostk::core::ctnr::Object ;

    return CDM::Dictionary(Object::Parse(aString, Object::Format::YAML).accessDictionary()) ;

}

CDM                             CDM::Load                                   (   const   File&                       aFile                                       )
{

    using ostk::core::ctnr::Object ;

    return CDM::Dictionary(Object::Load(aFile, Object::Format::YAML).accessDictionary()) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

State                           CDM::Deployment::toState                    ( ) const
{
    return { date, position, velocity } ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
