/// Apache License 2.0

#include <cmath>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationVector.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/Angle.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/Static.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody/Custom.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::geometry::Angle;
using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::geometry::d3::transformation::rotation::RotationVector;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::frame::provider::Static;
using ostk::physics::coordinate::Transform;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astrodynamics::conjunction::HardBody;
using ostk::astrodynamics::conjunction::hardbody::Custom;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::StateBuilder;

class OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        // Frame rotated by 90 degrees about the GCRF X axis: its X-Y plane is the GCRF X-Z plane
        if (Frame::Exists(rotatedFrameName_))
        {
            Frame::Destruct(rotatedFrameName_);
        }

        rotatedFrameSPtr_ = Frame::Construct(
            rotatedFrameName_,
            true,
            Frame::GCRF(),
            std::make_shared<Static>(Static(Transform::Passive(
                Instant::J2000(),
                Vector3d::Zero(),
                Vector3d::Zero(),
                Quaternion::RotationVector(RotationVector({1.0, 0.0, 0.0}, Angle::Degrees(90.0))),
                Vector3d::Zero()
            )))
        );
    }

    void TearDown() override
    {
        Frame::Destruct(rotatedFrameName_);
    }

    // Helper function to build a state
    State buildState(const Vector3d& aPosition, const Vector3d& aVelocity) const
    {
        const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
        };

        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), coordinateSubsets);

        VectorXd coordinates(6);
        coordinates << aPosition, aVelocity;

        return stateBuilder.build(Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC), coordinates);
    }

    // Helper function to build a vertex generator returning fixed offsets
    static Custom::VertexGenerator buildVertexGenerator(const Array<Vector3d>& anOffsetArray)
    {
        return [anOffsetArray](const State&) -> Array<Custom::Vertex>
        {
            Array<Custom::Vertex> vertices = Array<Custom::Vertex>::Empty();

            for (const Vector3d& offset : anOffsetArray)
            {
                vertices.add(Custom::Vertex::Vector(offset));
            }

            return vertices;
        };
    }

    const String rotatedFrameName_ = "HardBodyCustomTestRotatedFrame";
    Shared<const Frame> rotatedFrameSPtr_ = nullptr;

    // Cube of side 2.0
    const Array<Vector3d> cubeOffsets_ = {
        {-1.0, -1.0, -1.0},
        {-1.0, -1.0, 1.0},
        {-1.0, 1.0, -1.0},
        {-1.0, 1.0, 1.0},
        {1.0, -1.0, -1.0},
        {1.0, -1.0, 1.0},
        {1.0, 1.0, -1.0},
        {1.0, 1.0, 1.0},
    };

    // Plate of size 2.0 x 6.0 in the X-Z plane
    const Array<Vector3d> plateOffsets_ = {
        {-1.0, 0.0, -3.0},
        {-1.0, 0.0, 3.0},
        {1.0, 0.0, -3.0},
        {1.0, 0.0, 3.0},
    };

    // Plate of size 2.0 x 6.0 in the X-Y plane
    const Array<Vector3d> flatPlateOffsets_ = {
        {-1.0, -3.0, 0.0},
        {-1.0, 3.0, 0.0},
        {1.0, -3.0, 0.0},
        {1.0, 3.0, 0.0},
    };

    const Shared<const LocalOrbitalFrameFactory> vncFactorySPtr_ = LocalOrbitalFrameFactory::VNC(Frame::GCRF());

    const State state_ = buildState({7.0e6, 0.0, 0.0}, {0.0, 7.5e3, 0.0});
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom, GetVertexGenerator)
{
    {
        const Custom custom = Custom::FromInertial(Frame::GCRF(), buildVertexGenerator(cubeOffsets_));

        const Custom::VertexGenerator vertexGenerator = custom.getVertexGenerator();

        EXPECT_TRUE(static_cast<bool>(vertexGenerator));
        EXPECT_EQ(cubeOffsets_.getSize(), vertexGenerator(state_).getSize());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom, FromInertial)
{
    {
        EXPECT_NO_THROW(Custom::FromInertial(Frame::GCRF(), buildVertexGenerator(cubeOffsets_)));
    }

    {
        const Custom custom = Custom::FromInertial(Frame::GCRF(), buildVertexGenerator(cubeOffsets_));
        const HardBody& hardBody = custom;

        EXPECT_NO_THROW(hardBody.calculateCrossSectionAt(state_, Frame::GCRF()));
    }

    {
        EXPECT_THROW(
            try {
                Custom::FromInertial(Frame::GCRF(), Custom::VertexGenerator());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Vertex generator} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            try {
                Custom::FromInertial(nullptr, buildVertexGenerator(cubeOffsets_));
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            try {
                Custom::FromInertial(Frame::ITRF(), buildVertexGenerator(cubeOffsets_));
            } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_EQ("Frame = ITRF is wrong.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom, FromLocalOrbitalFrame)
{
    {
        EXPECT_NO_THROW(Custom::FromLocalOrbitalFrame(vncFactorySPtr_, buildVertexGenerator(cubeOffsets_)));
    }

    {
        EXPECT_THROW(
            try {
                Custom::FromLocalOrbitalFrame(vncFactorySPtr_, Custom::VertexGenerator());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Vertex generator} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            try {
                Custom::FromLocalOrbitalFrame(
                    LocalOrbitalFrameFactory::Undefined(), buildVertexGenerator(cubeOffsets_)
                );
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Local orbital frame factory} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            Custom::FromLocalOrbitalFrame(nullptr, buildVertexGenerator(cubeOffsets_)),
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom, CalculateCrossSectionAt)
{
    // Cube: square shadow of side 2.0
    {
        const Custom custom = Custom::FromInertial(Frame::GCRF(), buildVertexGenerator(cubeOffsets_));

        EXPECT_DOUBLE_EQ(std::sqrt(2.0), custom.calculateCrossSectionAt(state_, Frame::GCRF()).getEnclosingRadius());
    }

    // Cross section is centered on the object, whatever the state
    {
        const Custom custom = Custom::FromInertial(Frame::GCRF(), buildVertexGenerator(cubeOffsets_));

        const State anotherState = buildState({0.0, -4.2e7, 1.0e3}, {3.0e3, 0.0, 0.0});

        EXPECT_DOUBLE_EQ(
            std::sqrt(2.0), custom.calculateCrossSectionAt(anotherState, Frame::GCRF()).getEnclosingRadius()
        );
    }

    // Plate in the GCRF X-Z plane, projected along the GCRF Z axis: segment of length 2.0
    {
        const Custom custom = Custom::FromInertial(Frame::GCRF(), buildVertexGenerator(plateOffsets_));

        EXPECT_DOUBLE_EQ(1.0, custom.calculateCrossSectionAt(state_, Frame::GCRF()).getEnclosingRadius());
    }

    // Same plate, rotated into the rotated frame (projected along the GCRF Y axis): rectangle of size 2.0 x 6.0
    {
        const Custom custom = Custom::FromInertial(Frame::GCRF(), buildVertexGenerator(plateOffsets_));

        EXPECT_NEAR(
            std::sqrt(10.0), custom.calculateCrossSectionAt(state_, rotatedFrameSPtr_).getEnclosingRadius(), 1e-12
        );
    }

    // Same plate, expressed in the rotated frame and projected in the rotated frame: segment of length 2.0
    {
        const Custom custom = Custom::FromInertial(rotatedFrameSPtr_, buildVertexGenerator(plateOffsets_));

        EXPECT_NEAR(1.0, custom.calculateCrossSectionAt(state_, rotatedFrameSPtr_).getEnclosingRadius(), 1e-12);
    }

    // Plate in the VNC V-N plane. At this state, V is the GCRF Y axis, N the GCRF Z axis and C the GCRF X axis.
    {
        const Custom custom = Custom::FromLocalOrbitalFrame(vncFactorySPtr_, buildVertexGenerator(flatPlateOffsets_));

        // Projected along the GCRF Z axis (N): segment of length 2.0 along V
        EXPECT_NEAR(1.0, custom.calculateCrossSectionAt(state_, Frame::GCRF()).getEnclosingRadius(), 1e-12);

        // Projected along the Z axis of the rotated frame (GCRF Y axis, i.e. V): segment of length 6.0 along N
        EXPECT_NEAR(3.0, custom.calculateCrossSectionAt(state_, rotatedFrameSPtr_).getEnclosingRadius(), 1e-12);

        // Projected along the VNC Z axis (C): the whole plate
        EXPECT_NEAR(
            std::sqrt(10.0),
            custom.calculateCrossSectionAt(state_, vncFactorySPtr_->generateFrame(state_)).getEnclosingRadius(),
            1e-12
        );
    }

    // VNC follows the state: here V is the GCRF Z axis and N the GCRF X axis
    {
        const Custom custom = Custom::FromLocalOrbitalFrame(vncFactorySPtr_, buildVertexGenerator(flatPlateOffsets_));

        const State anotherState = buildState({0.0, 7.0e6, 0.0}, {0.0, 0.0, 7.5e3});

        // Projected along the GCRF Z axis (V): segment of length 6.0 along N
        EXPECT_NEAR(3.0, custom.calculateCrossSectionAt(anotherState, Frame::GCRF()).getEnclosingRadius(), 1e-12);
    }

    // Vertices along the projection axis only: point shadow
    {
        const Custom custom =
            Custom::FromInertial(Frame::GCRF(), buildVertexGenerator({{0.0, 0.0, -5.0}, {0.0, 0.0, 5.0}}));

        EXPECT_DOUBLE_EQ(0.0, custom.calculateCrossSectionAt(state_, Frame::GCRF()).getEnclosingRadius());
    }

    // Non-convex L shape: the shadow is its convex hull
    {
        const Custom custom = Custom::FromInertial(
            Frame::GCRF(),
            buildVertexGenerator({
                {0.0, 0.0, 0.0},
                {2.0, 0.0, 0.0},
                {2.0, 1.0, 0.0},
                {1.0, 1.0, 0.0},
                {1.0, 2.0, 0.0},
                {0.0, 2.0, 0.0},
            })
        );

        EXPECT_DOUBLE_EQ(std::sqrt(5.0), custom.calculateCrossSectionAt(state_, Frame::GCRF()).getEnclosingRadius());
    }

    // Combining with another hard body cross section
    {
        const Custom custom = Custom::FromInertial(Frame::GCRF(), buildVertexGenerator(cubeOffsets_));

        const HardBody::CrossSection combinedCrossSection = HardBody::CrossSection::combine(
            custom.calculateCrossSectionAt(state_, Frame::GCRF()), custom.calculateCrossSectionAt(state_, Frame::GCRF())
        );

        EXPECT_DOUBLE_EQ(2.0 * std::sqrt(2.0), combinedCrossSection.getEnclosingRadius());
    }

    {
        const Custom custom = Custom::FromInertial(
            Frame::GCRF(),
            [](const State&) -> Array<Custom::Vertex>
            {
                return Array<Custom::Vertex>::Empty();
            }
        );

        EXPECT_THROW(
            try {
                custom.calculateCrossSectionAt(state_, Frame::GCRF());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Vertices} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}
