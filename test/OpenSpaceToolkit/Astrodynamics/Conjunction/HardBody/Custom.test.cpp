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

    // Cube of side 2.0
    const Array<Vector3d> cubeVertices_ = {
        {-1.0, -1.0, -1.0},
        {-1.0, -1.0, 1.0},
        {-1.0, 1.0, -1.0},
        {-1.0, 1.0, 1.0},
        {1.0, -1.0, -1.0},
        {1.0, -1.0, 1.0},
        {1.0, 1.0, -1.0},
        {1.0, 1.0, 1.0},
    };

    // Cuboid of dimensions 2 in X, 4 in Y and 1 in Z
    const Array<Vector3d> cuboidVertices_ = {
        {-1.0, -2.0, -0.5},
        {-1.0, -2.0, 0.5},
        {-1.0, 2.0, -0.5},
        {-1.0, 2.0, 0.5},
        {1.0, -2.0, -0.5},
        {1.0, -2.0, 0.5},
        {1.0, 2.0, -0.5},
        {1.0, 2.0, 0.5},
    };
    const LocalOrbitalFrameFactory vncFactory_ = LocalOrbitalFrameFactory::VNC(Frame::GCRF());

    const LocalOrbitalFrameFactory qswFactory_ = LocalOrbitalFrameFactory::QSW(Frame::GCRF());

    const State state_ = buildState({7.0e6, 0.0, 0.0}, {0.0, 7.5e3, 0.0});

    const State anotherState_ = buildState({0.0, -4.2e7, 1.0e3}, {3.0e3, 0.0, 0.0});
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom, Constructor)
{
    {
        EXPECT_NO_THROW(Custom(buildVertexGenerator(cubeVertices_), Frame::GCRF()));
    }

    {
        EXPECT_NO_THROW(Custom(buildVertexGenerator(cubeVertices_), vncFactory_));
    }

    {
        const Custom custom = {buildVertexGenerator(cubeVertices_), Frame::GCRF()};
        const HardBody& hardBody = custom;

        EXPECT_NO_THROW(hardBody.calculateCrossSectionAt(state_, Frame::GCRF()));
    }

    {
        EXPECT_THROW(
            try {
                Custom(Custom::VertexGenerator(), Frame::GCRF());
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
                Custom(Custom::VertexGenerator(), vncFactory_);
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
                Custom(buildVertexGenerator(cubeVertices_), Shared<const Frame>(nullptr));
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            Custom(buildVertexGenerator(cubeVertices_), LocalOrbitalFrameFactory::Undefined()),
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom, GetVertexGenerator)
{
    {
        const Custom custom = {buildVertexGenerator(cubeVertices_), Frame::GCRF()};

        const Custom::VertexGenerator vertexGenerator = custom.getVertexGenerator();

        EXPECT_TRUE(static_cast<bool>(vertexGenerator));
        EXPECT_EQ(cubeVertices_.getSize(), vertexGenerator(state_).getSize());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom, CalculateCrossSectionAtInGCRF)
{
    // Cube cross section perpendicular to Z axis in GCRF: square shadow of side 2.0
    {
        const Custom custom = {buildVertexGenerator(cubeVertices_), Frame::GCRF()};

        EXPECT_DOUBLE_EQ(std::sqrt(2.0), custom.calculateCrossSectionAt(state_, Frame::GCRF()).getEnclosingRadius());

        // Check it's state independent
        EXPECT_DOUBLE_EQ(
            std::sqrt(2.0), custom.calculateCrossSectionAt(anotherState_, Frame::GCRF()).getEnclosingRadius()
        );
    }

    // Cube cross section perpendicular to {1, 1, 1} in GCRF: hexagon
    // The 2 vertices on the diagonal project onto the center, the 6 others onto a regular hexagon of circumradius
    // sqrt(3 - 1/3), i.e. their distance to the diagonal
    {
        const Custom custom = {buildVertexGenerator(cubeVertices_), Frame::GCRF()};

        // Frame whose Z axis is {1, 1, 1} in GCRF: GCRF rotated about {-1, 1, 0} by the angle between Z and {1, 1, 1}
        const String diagonalFrameName = "HardBodyCustomTestDiagonalFrame";

        const Shared<const Frame> diagonalFrameSPtr = Frame::Construct(
            diagonalFrameName,
            true,
            Frame::GCRF(),
            std::make_shared<Static>(Static(Transform::Passive(
                Instant::J2000(),
                Vector3d::Zero(),
                Vector3d::Zero(),
                Quaternion::RotationVector(RotationVector(
                    Vector3d(-1.0, 1.0, 0.0).normalized(), Angle::Radians(std::acos(1.0 / std::sqrt(3.0)))
                )),
                Vector3d::Zero()
            )))
        );

        EXPECT_NEAR(
            std::sqrt(8.0 / 3.0), custom.calculateCrossSectionAt(state_, diagonalFrameSPtr).getEnclosingRadius(), 1e-12
        );

        // Check it's state independent
        EXPECT_NEAR(
            std::sqrt(8.0 / 3.0),
            custom.calculateCrossSectionAt(anotherState_, diagonalFrameSPtr).getEnclosingRadius(),
            1e-12
        );

        Frame::Destruct(diagonalFrameName);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom, CalculateCrossSectionAtInVNC)
{
    // Frame whose Z axis is a VNC axis at a given state: VNC rotated by a given rotation
    const String rotatedVNCFrameName = "HardBodyCustomTestRotatedVNCFrame";

    const auto constructRotatedVNCFrame =
        [this, &rotatedVNCFrameName](const State& aState, const RotationVector& aRotationVector) -> Shared<const Frame>
    {
        return Frame::Construct(
            rotatedVNCFrameName,
            false,
            vncFactory_.generateFrame(aState),
            std::make_shared<Static>(Static(Transform::Passive(
                Instant::J2000(),
                Vector3d::Zero(),
                Vector3d::Zero(),
                Quaternion::RotationVector(aRotationVector),
                Vector3d::Zero()
            )))
        );
    };

    // Cuboid cross section perpendicular to VNC X axis is a rectangle of 1 x 4
    {
        const Custom custom = {buildVertexGenerator(cuboidVertices_), vncFactory_};

        // VNC rotated about its Y axis by 90 degrees: Z axis along the VNC X axis
        const RotationVector rotationVector = {{0.0, 1.0, 0.0}, Angle::Degrees(90.0)};

        EXPECT_NEAR(
            std::sqrt(0.5 * 0.5 + 2.0 * 2.0),
            custom.calculateCrossSectionAt(state_, constructRotatedVNCFrame(state_, rotationVector))
                .getEnclosingRadius(),
            1e-12
        );

        Frame::Destruct(rotatedVNCFrameName);

        // Check it's state independent
        EXPECT_NEAR(
            std::sqrt(0.5 * 0.5 + 2.0 * 2.0),
            custom.calculateCrossSectionAt(anotherState_, constructRotatedVNCFrame(anotherState_, rotationVector))
                .getEnclosingRadius(),
            1e-12
        );

        Frame::Destruct(rotatedVNCFrameName);
    }

    // Cuboid cross section perpendicular to VNC Y axis is a rectangle of 2 x 1
    {
        const Custom custom = {buildVertexGenerator(cuboidVertices_), vncFactory_};

        // VNC rotated about its X axis by 90 degrees: Z axis along the VNC Y axis
        const RotationVector rotationVector = {{1.0, 0.0, 0.0}, Angle::Degrees(90.0)};

        EXPECT_NEAR(
            std::sqrt(1.0 * 1.0 + 0.5 * 0.5),
            custom.calculateCrossSectionAt(state_, constructRotatedVNCFrame(state_, rotationVector))
                .getEnclosingRadius(),
            1e-12
        );

        Frame::Destruct(rotatedVNCFrameName);

        // Check it's state independent
        EXPECT_NEAR(
            std::sqrt(1.0 * 1.0 + 0.5 * 0.5),
            custom.calculateCrossSectionAt(anotherState_, constructRotatedVNCFrame(anotherState_, rotationVector))
                .getEnclosingRadius(),
            1e-12
        );

        Frame::Destruct(rotatedVNCFrameName);
    }

    // Cuboid cross section perpendicular to VNC Z axis is a rectangle of 2 x 4
    {
        const Custom custom = {buildVertexGenerator(cuboidVertices_), vncFactory_};

        EXPECT_NEAR(
            std::sqrt(1.0 * 1.0 + 2.0 * 2.0),
            custom.calculateCrossSectionAt(state_, vncFactory_.generateFrame(state_)).getEnclosingRadius(),
            1e-12
        );

        // Check it's state independent
        EXPECT_NEAR(
            std::sqrt(1.0 * 1.0 + 2.0 * 2.0),
            custom.calculateCrossSectionAt(anotherState_, vncFactory_.generateFrame(anotherState_))
                .getEnclosingRadius(),
            1e-12
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom, CalculateCrossSectionDefinedInQSWButComputedInGCRF)
{
    const Custom custom = {buildVertexGenerator(cuboidVertices_), qswFactory_};

    // Frame whose Z axis is a GCRF axis: GCRF rotated by a given rotation
    // - About its Y axis by 90 degrees: Z axis along the GCRF X axis
    // - About its X axis by 90 degrees: Z axis along the GCRF Y axis
    const String rotatedFrameName = "HardBodyCustomTestRotatedFrame";

    const auto constructRotatedFrame = [&rotatedFrameName](const RotationVector& aRotationVector) -> Shared<const Frame>
    {
        return Frame::Construct(
            rotatedFrameName,
            true,
            Frame::GCRF(),
            std::make_shared<Static>(Static(Transform::Passive(
                Instant::J2000(),
                Vector3d::Zero(),
                Vector3d::Zero(),
                Quaternion::RotationVector(aRotationVector),
                Vector3d::Zero()
            )))
        );
    };

    // State where QSW is aligned with GCRF
    {
        const State state = buildState({7.0e6, 0.0, 0.0}, {0.0, 7.5e3, 0.0});

        // Cross section perpendicular to GCRF X axis is a rectangle of 1 x 4
        EXPECT_NEAR(
            std::sqrt(2.0 * 2.0 + 0.5 * 0.5),
            custom
                .calculateCrossSectionAt(
                    state, constructRotatedFrame(RotationVector({0.0, 1.0, 0.0}, Angle::Degrees(90.0)))
                )
                .getEnclosingRadius(),
            1e-12
        );

        Frame::Destruct(rotatedFrameName);

        // Cross section perpendicular to GCRF Y axis is a rectangle of 1 x 2
        EXPECT_NEAR(
            std::sqrt(1.0 * 1.0 + 0.5 * 0.5),
            custom
                .calculateCrossSectionAt(
                    state, constructRotatedFrame(RotationVector({1.0, 0.0, 0.0}, Angle::Degrees(90.0)))
                )
                .getEnclosingRadius(),
            1e-12
        );

        Frame::Destruct(rotatedFrameName);

        // Cross section perpendicular to GCRF Z axis is a rectangle of 2 x 4
        EXPECT_NEAR(
            std::sqrt(1.0 * 1.0 + 2.0 * 2.0),
            custom.calculateCrossSectionAt(state, Frame::GCRF()).getEnclosingRadius(),
            1e-12
        );
    }

    // State where QSW X is aligned with GCRF Y and QSW Y is aligned with GCRF -X
    {
        const State state = buildState({0.0, 7.0e6, 0.0}, {-7.5e3, 0.0, 0.0});

        // Cross section perpendicular to GCRF X axis is a rectangle of 1 x 2
        EXPECT_NEAR(
            std::sqrt(1.0 * 1.0 + 0.5 * 0.5),
            custom
                .calculateCrossSectionAt(
                    state, constructRotatedFrame(RotationVector({0.0, 1.0, 0.0}, Angle::Degrees(90.0)))
                )
                .getEnclosingRadius(),
            1e-12
        );

        Frame::Destruct(rotatedFrameName);

        // Cross section perpendicular to GCRF Y axis is a rectangle of 1 x 4
        EXPECT_NEAR(
            std::sqrt(2.0 * 2.0 + 0.5 * 0.5),
            custom
                .calculateCrossSectionAt(
                    state, constructRotatedFrame(RotationVector({1.0, 0.0, 0.0}, Angle::Degrees(90.0)))
                )
                .getEnclosingRadius(),
            1e-12
        );

        Frame::Destruct(rotatedFrameName);

        // Cross section perpendicular to GCRF Z axis is a rectangle of 2 x 4
        EXPECT_NEAR(
            std::sqrt(1.0 * 1.0 + 2.0 * 2.0),
            custom.calculateCrossSectionAt(state, Frame::GCRF()).getEnclosingRadius(),
            1e-12
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom, Combine)
{
    const Custom cube = {buildVertexGenerator(cubeVertices_), Frame::GCRF()};
    const Custom cuboid = {buildVertexGenerator(cuboidVertices_), Frame::GCRF()};

    // Frame whose Z axis is a GCRF axis: GCRF rotated by a given rotation
    const String rotatedFrameName = "HardBodyCustomTestRotatedFrame";

    const auto constructRotatedFrame = [&rotatedFrameName](const RotationVector& aRotationVector) -> Shared<const Frame>
    {
        return Frame::Construct(
            rotatedFrameName,
            true,
            Frame::GCRF(),
            std::make_shared<Static>(Static(Transform::Passive(
                Instant::J2000(),
                Vector3d::Zero(),
                Vector3d::Zero(),
                Quaternion::RotationVector(aRotationVector),
                Vector3d::Zero()
            )))
        );
    };

    // The Minkowski sum of the cube and the cuboid is a cuboid of half sides 1 + 1 = 2 in X, 1 + 2 = 3 in Y and
    // 1 + 0.5 = 1.5 in Z

    // Combined cross section perpendicular to X axis in GCRF has enclosing radius of sqrt(3^2 + 1.5^2)
    {
        // GCRF rotated about its Y axis by 90 degrees: Z axis along the GCRF X axis
        const Shared<const Frame> rotatedFrameSPtr =
            constructRotatedFrame(RotationVector({0.0, 1.0, 0.0}, Angle::Degrees(90.0)));

        const HardBody::CrossSection combinedCrossSection = HardBody::CrossSection::combine(
            cube.calculateCrossSectionAt(state_, rotatedFrameSPtr),
            cuboid.calculateCrossSectionAt(state_, rotatedFrameSPtr)
        );

        EXPECT_NEAR(std::sqrt(3.0 * 3.0 + 1.5 * 1.5), combinedCrossSection.getEnclosingRadius(), 1e-12);

        Frame::Destruct(rotatedFrameName);
    }

    // Combined cross section perpendicular to Y axis in GCRF has enclosing radius of sqrt(2^2 + 1.5^2)
    {
        // GCRF rotated about its X axis by 90 degrees: Z axis along the GCRF Y axis
        const Shared<const Frame> rotatedFrameSPtr =
            constructRotatedFrame(RotationVector({1.0, 0.0, 0.0}, Angle::Degrees(90.0)));

        const HardBody::CrossSection combinedCrossSection = HardBody::CrossSection::combine(
            cube.calculateCrossSectionAt(state_, rotatedFrameSPtr),
            cuboid.calculateCrossSectionAt(state_, rotatedFrameSPtr)
        );

        EXPECT_NEAR(std::sqrt(2.0 * 2.0 + 1.5 * 1.5), combinedCrossSection.getEnclosingRadius(), 1e-12);

        Frame::Destruct(rotatedFrameName);
    }

    // Combined cross section perpendicular to Z axis in GCRF has enclosing radius of sqrt(2^2 + 3^2)
    {
        const HardBody::CrossSection combinedCrossSection = HardBody::CrossSection::combine(
            cube.calculateCrossSectionAt(state_, Frame::GCRF()), cuboid.calculateCrossSectionAt(state_, Frame::GCRF())
        );

        EXPECT_NEAR(std::sqrt(2.0 * 2.0 + 3.0 * 3.0), combinedCrossSection.getEnclosingRadius(), 1e-12);
    }
}
