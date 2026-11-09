/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystemBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::type::String;

using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;

using ostk::physics::unit::Mass;

using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::flight::system::SatelliteSystemBuilder;

class OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder : public ::testing::Test
{
   protected:
    void SetUp() override {}

    Mass dryMass_ = {100.0, Mass::Unit::Kilogram};
    Matrix3d inertiaTensor_ = Matrix3d::Identity();
    double crossSectionalSurfaceArea_ = 0.8;
    double dragCoefficient_ = 1.2;
    Composite geometry_ = Composite(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    PropulsionSystem propulsionSystem_ = PropulsionSystem::Default();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder, Constructor)
{
    {
        EXPECT_NO_THROW(SatelliteSystemBuilder());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder, CopyConstructor)
{
    {
        SatelliteSystemBuilder builder = SatelliteSystemBuilder();
        EXPECT_NO_THROW(SatelliteSystemBuilder satelliteSystemBuilderCopy(builder));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder, StreamOperator)
{
    {
        SatelliteSystemBuilder builder = SatelliteSystemBuilder();

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << builder << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder, Build)
{
    {
        SatelliteSystemBuilder builder =
            SatelliteSystemBuilder().withDryMass(dryMass_).withGeometry(geometry_).withInertiaTensor(inertiaTensor_);
        EXPECT_NO_THROW(builder.build());
    }

    {
        // A cross-sectional surface area, drag coefficient and propulsion system that were not set are absent
        const SatelliteSystem system = SatelliteSystemBuilder()
                                           .withDryMass(dryMass_)
                                           .withGeometry(geometry_)
                                           .withInertiaTensor(inertiaTensor_)
                                           .build();

        EXPECT_THROW(system.getCrossSectionalSurfaceArea(), ostk::core::error::runtime::Undefined);
        EXPECT_THROW(system.getDragCoefficient(), ostk::core::error::runtime::Undefined);
        EXPECT_THROW(system.getPropulsionSystem(), ostk::core::error::runtime::Undefined);
    }

    {
        // The dry mass, geometry and inertia tensor are required
        EXPECT_THROW(SatelliteSystemBuilder().build(), ostk::core::error::runtime::Undefined);

        EXPECT_THROW(
            SatelliteSystemBuilder().withGeometry(geometry_).withInertiaTensor(inertiaTensor_).build(),
            ostk::core::error::runtime::Undefined
        );
        EXPECT_THROW(
            SatelliteSystemBuilder().withDryMass(dryMass_).withInertiaTensor(inertiaTensor_).build(),
            ostk::core::error::runtime::Undefined
        );
        EXPECT_THROW(
            SatelliteSystemBuilder().withDryMass(dryMass_).withGeometry(geometry_).build(),
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder, WithDryMass)
{
    {
        SatelliteSystemBuilder builder = SatelliteSystemBuilder::Default().withDryMass(dryMass_);
        const SatelliteSystem system = builder.build();

        EXPECT_EQ(system.getMass(), dryMass_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder, WithGeometry)
{
    {
        SatelliteSystemBuilder builder = SatelliteSystemBuilder::Default().withGeometry(geometry_);
        const SatelliteSystem system = builder.build();

        EXPECT_EQ(system.getGeometry(), geometry_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder, WithInertiaTensor)
{
    {
        SatelliteSystemBuilder builder = SatelliteSystemBuilder::Default().withInertiaTensor(inertiaTensor_);
        const SatelliteSystem system = builder.build();

        EXPECT_EQ(system.getInertiaTensor(), inertiaTensor_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder, WithCrossSectionalSurfaceArea)
{
    {
        SatelliteSystemBuilder builder =
            SatelliteSystemBuilder::Default().withCrossSectionalSurfaceArea(crossSectionalSurfaceArea_);
        const SatelliteSystem system = builder.build();

        EXPECT_EQ(system.getCrossSectionalSurfaceArea(), crossSectionalSurfaceArea_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder, WithDragCoefficient)
{
    {
        SatelliteSystemBuilder builder = SatelliteSystemBuilder::Default().withDragCoefficient(dragCoefficient_);
        const SatelliteSystem system = builder.build();

        EXPECT_EQ(system.getDragCoefficient(), dragCoefficient_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder, WithPropulsionSystem)
{
    {
        SatelliteSystemBuilder builder = SatelliteSystemBuilder::Default().withPropulsionSystem(propulsionSystem_);
        const SatelliteSystem system = builder.build();

        EXPECT_EQ(system.getPropulsionSystem(), propulsionSystem_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder, Print)
{
    {
        SatelliteSystemBuilder builder = SatelliteSystemBuilder();

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(builder.print(std::cout, true));
        EXPECT_NO_THROW(builder.print(std::cout, false));

        const std::string output = testing::internal::GetCapturedStdout();

        EXPECT_FALSE(output.empty());
        EXPECT_NE(output.find("Undefined"), std::string::npos);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder, Default)
{
    {
        SatelliteSystemBuilder builder = SatelliteSystemBuilder::Default();
        SatelliteSystem system = builder.build();

        EXPECT_EQ(system, SatelliteSystem::Default());
    }
}
