/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Messages/SpaceX/OPM.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::filesystem::File;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::units::Angle;
using ostk::physics::units::Length;

using ostk::astro::trajectory::orbit::messages::spacex::OPM;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Messages_SpaceX_OPM : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        this->opm_ = OPM {
            OPM::Header {
                Instant::DateTime(DateTime(2022, 1, 2, 3, 4, 5), Scale::UTC),
                Instant::DateTime(DateTime(2022, 1, 2, 3, 4, 6), Scale::UTC),
            },
            {OPM::Deployment {
                 "A",
                 123,
                 Duration::Seconds(123.456),
                 Instant::DateTime(DateTime(2022, 1, 2, 3, 4, 6), Scale::UTC),
                 Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF()),
                 Velocity::MetersPerSecond({0.0, 0.0, 0.0}, Frame::ITRF()),
                 Length::Kilometers(100.0),
                 Length::Kilometers(200.0),
                 Angle::Degrees(1.0),
                 Angle::Degrees(2.0),
                 Angle::Degrees(3.0),
                 Angle::Degrees(4.0),
                 123.456
             },
             OPM::Deployment {
                 "B",
                 456,
                 Duration::Seconds(456.123),
                 Instant::DateTime(DateTime(2022, 1, 2, 3, 4, 7), Scale::UTC),
                 Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF()),
                 Velocity::MetersPerSecond({0.0, 0.0, 0.0}, Frame::ITRF()),
                 Length::Kilometers(300.0),
                 Length::Kilometers(400.0),
                 Angle::Degrees(5.0),
                 Angle::Degrees(6.0),
                 Angle::Degrees(7.0),
                 Angle::Degrees(8.0),
                 456.123
             }}
        };
    }

    OPM opm_ = OPM::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Messages_SpaceX_OPM, Constructor)
{
    const OPM opm = {
        OPM::Header {
            Instant::DateTime(DateTime(2022, 1, 2, 3, 4, 5), Scale::UTC),
            Instant::DateTime(DateTime(2022, 1, 2, 3, 4, 6), Scale::UTC),
        },
        {OPM::Deployment {
            "A",
            123,
            Duration::Seconds(123.456),
            Instant::DateTime(DateTime(2022, 1, 2, 3, 4, 6), Scale::UTC),
            Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF()),
            Velocity::MetersPerSecond({0.0, 0.0, 0.0}, Frame::ITRF()),
            Length::Kilometers(100.0),
            Length::Kilometers(200.0),
            Angle::Degrees(1.0),
            Angle::Degrees(2.0),
            Angle::Degrees(3.0),
            Angle::Degrees(4.0),
            123.456
        }}
    };
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Messages_SpaceX_OPM, IsDefined)
{
    {
        EXPECT_TRUE(this->opm_.isDefined());
    }

    {
        EXPECT_FALSE(OPM::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Messages_SpaceX_OPM, GetHeader)
{
    {
        const OPM::Header header = this->opm_.getHeader();

        EXPECT_EQ(Instant::DateTime(DateTime(2022, 1, 2, 3, 4, 5), Scale::UTC), header.generationDate);
        EXPECT_EQ(Instant::DateTime(DateTime(2022, 1, 2, 3, 4, 6), Scale::UTC), header.launchDate);
    }

    {
        EXPECT_ANY_THROW(OPM::Undefined().getHeader());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Messages_SpaceX_OPM, GetDeployments)
{
    {
        const Array<OPM::Deployment> deployments = this->opm_.getDeployments();

        EXPECT_EQ(2, deployments.getSize());

        EXPECT_EQ("A", deployments[0].name);
        EXPECT_EQ(123, deployments[0].sequenceNumber);
        EXPECT_EQ(Duration::Seconds(123.456), deployments[0].missionTime);
        EXPECT_EQ(Instant::DateTime(DateTime(2022, 1, 2, 3, 4, 6), Scale::UTC), deployments[0].date);
        EXPECT_EQ(Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF()), deployments[0].position);
        EXPECT_EQ(Velocity::MetersPerSecond({0.0, 0.0, 0.0}, Frame::ITRF()), deployments[0].velocity);
        EXPECT_EQ(Length::Kilometers(100.0), deployments[0].meanPerigeeAltitude);
        EXPECT_EQ(Length::Kilometers(200.0), deployments[0].meanApogeeAltitude);
        EXPECT_EQ(Angle::Degrees(1.0), deployments[0].meanInclination);
        EXPECT_EQ(Angle::Degrees(2.0), deployments[0].meanArgumentOfPerigee);
        EXPECT_EQ(Angle::Degrees(3.0), deployments[0].meanLongitudeAscendingNode);
        EXPECT_EQ(Angle::Degrees(4.0), deployments[0].meanMeanAnomaly);
        EXPECT_EQ(123.456, deployments[0].ballisticCoefficient);
    }

    {
        EXPECT_ANY_THROW(OPM::Undefined().getDeployments());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Messages_SpaceX_OPM, GetDeploymentAt)
{
    {
        const OPM::Deployment deployment = this->opm_.getDeploymentAt(0);

        EXPECT_EQ("A", deployment.name);
        EXPECT_EQ(123, deployment.sequenceNumber);
        EXPECT_EQ(Duration::Seconds(123.456), deployment.missionTime);
        EXPECT_EQ(Instant::DateTime(DateTime(2022, 1, 2, 3, 4, 6), Scale::UTC), deployment.date);
        EXPECT_EQ(Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF()), deployment.position);
        EXPECT_EQ(Velocity::MetersPerSecond({0.0, 0.0, 0.0}, Frame::ITRF()), deployment.velocity);
        EXPECT_EQ(Length::Kilometers(100.0), deployment.meanPerigeeAltitude);
        EXPECT_EQ(Length::Kilometers(200.0), deployment.meanApogeeAltitude);
        EXPECT_EQ(Angle::Degrees(1.0), deployment.meanInclination);
        EXPECT_EQ(Angle::Degrees(2.0), deployment.meanArgumentOfPerigee);
        EXPECT_EQ(Angle::Degrees(3.0), deployment.meanLongitudeAscendingNode);
        EXPECT_EQ(Angle::Degrees(4.0), deployment.meanMeanAnomaly);
        EXPECT_EQ(123.456, deployment.ballisticCoefficient);
    }

    {
        EXPECT_ANY_THROW(this->opm_.getDeploymentAt(2));
    }

    {
        EXPECT_ANY_THROW(OPM::Undefined().getDeploymentAt(0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Messages_SpaceX_OPM, GetDeploymentWithName)
{
    {
        const OPM::Deployment deployment = this->opm_.getDeploymentWithName("B");

        EXPECT_EQ("B", deployment.name);
        EXPECT_EQ(456, deployment.sequenceNumber);
        EXPECT_EQ(Duration::Seconds(456.123), deployment.missionTime);
        EXPECT_EQ(Instant::DateTime(DateTime(2022, 1, 2, 3, 4, 7), Scale::UTC), deployment.date);
        EXPECT_EQ(Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF()), deployment.position);
        EXPECT_EQ(Velocity::MetersPerSecond({0.0, 0.0, 0.0}, Frame::ITRF()), deployment.velocity);
        EXPECT_EQ(Length::Kilometers(300.0), deployment.meanPerigeeAltitude);
        EXPECT_EQ(Length::Kilometers(400.0), deployment.meanApogeeAltitude);
        EXPECT_EQ(Angle::Degrees(5.0), deployment.meanInclination);
        EXPECT_EQ(Angle::Degrees(6.0), deployment.meanArgumentOfPerigee);
        EXPECT_EQ(Angle::Degrees(7.0), deployment.meanLongitudeAscendingNode);
        EXPECT_EQ(Angle::Degrees(8.0), deployment.meanMeanAnomaly);
        EXPECT_EQ(456.123, deployment.ballisticCoefficient);
    }

    {
        EXPECT_ANY_THROW(this->opm_.getDeploymentWithName("C"));
    }

    {
        EXPECT_ANY_THROW(OPM::Undefined().getDeploymentWithName("B"));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Messages_SpaceX_OPM, Undefined)
{
    {
        EXPECT_NO_THROW(OPM::Undefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Messages_SpaceX_OPM, Dictionary)
{
    using ostk::core::container::Dictionary;
    using ostk::core::container::Object;

    {
        Dictionary opmDictionary = Dictionary::Empty();

        opmDictionary["header"] = Object::Dictionary(Dictionary::Empty());

        opmDictionary["header"]["generation_date"] = Object::String("2020-01-01T12:34:56.789Z");
        opmDictionary["header"]["launch_date"] = Object::String("2020-01-02T12:34:56.789Z");

        opmDictionary["deployments"] = Object::Array(Array<Object>::Empty());

        const OPM opm = OPM::Dictionary(opmDictionary);

        EXPECT_EQ(Instant::DateTime(DateTime(2020, 1, 1, 12, 34, 56, 789), Scale::UTC), opm.getHeader().generationDate);
        EXPECT_EQ(Instant::DateTime(DateTime(2020, 1, 2, 12, 34, 56, 789), Scale::UTC), opm.getHeader().launchDate);

        EXPECT_EQ(0, opm.getDeployments().getSize());
    }

    {
        EXPECT_ANY_THROW(OPM::Dictionary(Dictionary::Empty()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Messages_SpaceX_OPM, Parse)
{
    {
        EXPECT_ANY_THROW(OPM::Parse(String::Empty()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Messages_SpaceX_OPM, Load)
{
    using ostk::core::filesystem::File;
    using ostk::core::filesystem::Path;

    {
        const OPM opm = OPM::Load(File::Path(
            Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Messages/SpaceX/OPM/opm_1.yaml")
        ));

        EXPECT_EQ(Instant::DateTime(DateTime(2020, 1, 1, 12, 34, 56, 789), Scale::UTC), opm.getHeader().generationDate);
        EXPECT_EQ(Instant::DateTime(DateTime(2020, 1, 2, 12, 34, 56, 789), Scale::UTC), opm.getHeader().launchDate);

        EXPECT_EQ(2, opm.getDeployments().getSize());

        EXPECT_EQ("satellite_a", opm.getDeployments()[0].name);
        EXPECT_EQ(1, opm.getDeployments()[0].sequenceNumber);
        EXPECT_EQ(Duration::Hours(1.0), opm.getDeployments()[0].missionTime);
        EXPECT_EQ(Instant::DateTime(DateTime(2020, 1, 2, 13, 34, 56, 789), Scale::UTC), opm.getDeployments()[0].date);
        EXPECT_EQ(
            Position::Meters({693289.644, 6876578.628, -133035.288}, Frame::ITRF()), opm.getDeployments()[0].position
        );
        EXPECT_EQ(
            Velocity::MetersPerSecond({1305.783, 39.783, 7525.920}, Frame::ITRF()), opm.getDeployments()[0].velocity
        );
        EXPECT_EQ(Length::Kilometers(526.768), opm.getDeployments()[0].meanPerigeeAltitude);
        EXPECT_EQ(Length::Kilometers(568.430), opm.getDeployments()[0].meanApogeeAltitude);
        EXPECT_EQ(Angle::Degrees(97.123), opm.getDeployments()[0].meanInclination);
        EXPECT_EQ(Angle::Degrees(-179.513), opm.getDeployments()[0].meanArgumentOfPerigee);
        EXPECT_EQ(Angle::Degrees(85.057), opm.getDeployments()[0].meanLongitudeAscendingNode);
        EXPECT_EQ(Angle::Degrees(179.263), opm.getDeployments()[0].meanMeanAnomaly);
        EXPECT_EQ(47.55, opm.getDeployments()[0].ballisticCoefficient);

        EXPECT_EQ("satellite_b", opm.getDeployments()[1].name);
        EXPECT_EQ(2, opm.getDeployments()[1].sequenceNumber);
        EXPECT_EQ(Duration::Hours(2.0), opm.getDeployments()[1].missionTime);
        EXPECT_EQ(Instant::DateTime(DateTime(2020, 1, 2, 14, 34, 56, 789), Scale::UTC), opm.getDeployments()[1].date);
        EXPECT_EQ(
            Position::Meters({699863.059, 6875647.517, -123777.595}, Frame::ITRF()), opm.getDeployments()[1].position
        );
        EXPECT_EQ(
            Velocity::MetersPerSecond({1504.658, 6.705, 7538.669}, Frame::ITRF()), opm.getDeployments()[1].velocity
        );
        EXPECT_EQ(Length::Kilometers(536.779), opm.getDeployments()[1].meanPerigeeAltitude);
        EXPECT_EQ(Length::Kilometers(529.851), opm.getDeployments()[1].meanApogeeAltitude);
        EXPECT_EQ(Angle::Degrees(97.124), opm.getDeployments()[1].meanInclination);
        EXPECT_EQ(Angle::Degrees(136.875), opm.getDeployments()[1].meanArgumentOfPerigee);
        EXPECT_EQ(Angle::Degrees(85.032), opm.getDeployments()[1].meanLongitudeAscendingNode);
        EXPECT_EQ(Angle::Degrees(-127.164), opm.getDeployments()[1].meanMeanAnomaly);
        EXPECT_EQ(44.26, opm.getDeployments()[1].ballisticCoefficient);
    }

    {
        EXPECT_ANY_THROW(OPM::Load(File::Undefined()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Messages_SpaceX_OPM, DeploymentToState)
{
    using ostk::astro::trajectory::State;

    {
        const OPM::Deployment deployment = this->opm_.getDeploymentWithName("B");

        const State deploymentState = deployment.toState();

        EXPECT_EQ(deployment.date, deploymentState.getInstant());
        EXPECT_EQ(deployment.position, deploymentState.getPosition());
        EXPECT_EQ(deployment.velocity, deploymentState.getVelocity());
    }
}
