/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Object.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Messages/SpaceX/OPM.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace messages
{
namespace spacex
{

using ostk::mathematics::object::Vector3d;

Vector3d vector3dFromObject(const ctnr::Object& anObject)
{
    return {
        anObject.accessArray()[0].accessReal(),
        anObject.accessArray()[1].accessReal(),
        anObject.accessArray()[2].accessReal(),
    };
}

OPM::OPM(const OPM::Header& aHeader, const Array<OPM::Deployment>& aDeploymentArray)
    : header_(aHeader),
      deployments_(aDeploymentArray)
{
}

std::ostream& operator<<(std::ostream& anOutputStream, const OPM& anOPM)
{
    anOPM.print(anOutputStream);

    return anOutputStream;
}

bool OPM::isDefined() const
{
    return this->header_.generationDate.isDefined() && this->header_.launchDate.isDefined();
}

OPM::Header OPM::getHeader() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OPM");
    }

    return this->header_;
}

Array<OPM::Deployment> OPM::getDeployments() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OPM");
    }

    return this->deployments_;
}

OPM::Deployment OPM::getDeploymentAt(const Index& anIndex) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OPM");
    }

    return this->deployments_.at(anIndex);
}

OPM::Deployment OPM::getDeploymentWithName(const String& aName) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OPM");
    }

    for (const auto& deployment : this->deployments_)
    {
        if (deployment.name == aName)
        {
            return deployment;
        }
    }

    throw ostk::core::error::RuntimeError("Deployment with name [{}] not found.", aName);
}

void OPM::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::type::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Orbit Parameter Message") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Generation date:"
        << (this->header_.generationDate.isDefined() ? this->header_.generationDate.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Launch date:" << (this->header_.launchDate.isDefined() ? this->header_.launchDate.toString() : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

OPM OPM::Undefined()
{
    return {{Instant::Undefined(), Instant::Undefined()}, Array<Deployment>::Empty()};
}

OPM OPM::Dictionary(const ctnr::Dictionary& aDictionary)
{
    using ostk::physics::coordinate::Frame;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Scale;

    Array<OPM::Deployment> deployments = Array<OPM::Deployment>::Empty();

    for (const auto& deploymentObject : aDictionary["deployments"].accessArray())
    {
        const OPM::Deployment deployment = {
            deploymentObject["name"].accessString(),
            deploymentObject["sequence_number"].accessInteger(),
            Duration::Seconds(deploymentObject["mission_time_s"].accessReal()),
            Instant::DateTime(
                DateTime::Parse(deploymentObject["date"].getString().replace("Z", ""), DateTime::Format::ISO8601),
                Scale::UTC
            ),
            Position::Meters(vector3dFromObject(deploymentObject["r_ecef_m"]), Frame::ITRF()),
            Velocity::MetersPerSecond(vector3dFromObject(deploymentObject["v_ecef_m_per_s"]), Frame::ITRF()),
            Length::Kilometers(deploymentObject["mean_perigee_altitude_km"].accessReal()),
            Length::Kilometers(deploymentObject["mean_apogee_altitude_km"].accessReal()),
            Angle::Degrees(deploymentObject["mean_inclination_deg"].accessReal()),
            Angle::Degrees(deploymentObject["mean_argument_of_perigee_deg"].accessReal()),
            Angle::Degrees(deploymentObject["mean_longitude_ascending_node_deg"].accessReal()),
            Angle::Degrees(deploymentObject["mean_mean_anomaly_deg"].accessReal()),
            deploymentObject["ballistic_coef_kg_per_m2"].accessReal()
        };

        deployments.add(deployment);
    }

    return OPM {
        OPM::Header {
            Instant::DateTime(
                DateTime::Parse(
                    aDictionary["header"]["generation_date"].getString().replace("Z", ""), DateTime::Format::ISO8601
                ),
                Scale::UTC
            ),
            Instant::DateTime(
                DateTime::Parse(
                    aDictionary["header"]["launch_date"].getString().replace("Z", ""), DateTime::Format::ISO8601
                ),
                Scale::UTC
            ),
        },
        deployments
    };
}

OPM OPM::Parse(const String& aString)
{
    using ostk::core::container::Object;

    return OPM::Dictionary(Object::Parse(aString, Object::Format::YAML).accessDictionary());
}

OPM OPM::Load(const File& aFile)
{
    using ostk::core::container::Object;

    return OPM::Dictionary(Object::Load(aFile, Object::Format::YAML).accessDictionary());
}

State OPM::Deployment::toState() const
{
    return {date, position, velocity};
}

}  // namespace spacex
}  // namespace messages
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
