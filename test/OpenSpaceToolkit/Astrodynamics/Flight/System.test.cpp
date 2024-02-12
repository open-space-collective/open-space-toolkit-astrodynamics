/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System.hpp>

#include <Global.test.hpp>

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System, Constructor)
{
    using ostk::core::type::Integer;
    using ostk::core::type::Real;
    using ostk::core::type::String;

    using ostk::mathematics::geometry::d3::object::Composite;
    using ostk::mathematics::geometry::d3::object::Cuboid;
    using ostk::mathematics::geometry::d3::object::Point;
    using ostk::mathematics::object::Matrix3d;
    using ostk::mathematics::object::Vector3d;

    using ostk::physics::unit::Mass;

    using ostk::astro::flight::System;

    // Normal constructor
    {
        // Define satellite mass
        const Mass systemMass = {100.0, Mass::Unit::Kilogram};

        // Define satellite geometry
        const Point center = {0.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

        const Cuboid systemCuboid = {center, axes, extent};
        const Composite systemGeometry(systemCuboid);

        // Construct System object
        EXPECT_NO_THROW(System system(systemMass, systemGeometry));
    }

    // Copy constructor
    {
        // Define satellite mass
        const Mass systemMass = {100.0, Mass::Unit::Kilogram};

        // Define satellite geometry
        const Point center = {0.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

        const Cuboid systemCuboid = {center, axes, extent};
        const Composite systemGeometry(systemCuboid);

        // Construct System object
        const System system = {systemMass, systemGeometry};

        EXPECT_NO_THROW(System systemCopy(system));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System, EqualToOperator)
{
    using ostk::core::type::Integer;
    using ostk::core::type::Real;
    using ostk::core::type::String;

    using ostk::mathematics::geometry::d3::object::Composite;
    using ostk::mathematics::geometry::d3::object::Cuboid;
    using ostk::mathematics::geometry::d3::object::Point;
    using ostk::mathematics::object::Matrix3d;
    using ostk::mathematics::object::Vector3d;

    using ostk::physics::unit::Mass;

    using ostk::astro::flight::System;

    // Normal constructor
    {
        // Define system mass
        const Mass systemMass = {100.0, Mass::Unit::Kilogram};

        // Define system geometry
        const Point center = {0.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

        const Cuboid systemCuboid = {center, axes, extent};
        const Composite systemGeometry(systemCuboid);

        const System system = {systemMass, systemGeometry};

        EXPECT_TRUE(system == system);

        // Test for different mass
        const Mass systemMass_0 = {90.0, Mass::Unit::Kilogram};
        const System system_1 = {systemMass_0, systemGeometry};

        EXPECT_FALSE(system == system_1);

        // Test for different geometry
        const Point center_0 = {1.0, 0.0, 0.0};
        const Cuboid systemCuboid_0 = {center_0, axes, extent};
        const Composite systemGeometry_0(systemCuboid_0);
        const System system_2 = {systemMass, systemGeometry_0};

        EXPECT_FALSE(system == system_2);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System, NotEqualToOperator)
{
    using ostk::core::type::Integer;
    using ostk::core::type::Real;
    using ostk::core::type::String;

    using ostk::mathematics::geometry::d3::object::Composite;
    using ostk::mathematics::geometry::d3::object::Cuboid;
    using ostk::mathematics::geometry::d3::object::Point;
    using ostk::mathematics::object::Matrix3d;
    using ostk::mathematics::object::Vector3d;

    using ostk::physics::unit::Mass;

    using ostk::astro::flight::System;

    // Normal constructor
    {
        // Define system mass
        const Mass systemMass = {100.0, Mass::Unit::Kilogram};

        // Define system geometry
        const Point center = {0.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

        const Cuboid systemCuboid = {center, axes, extent};
        const Composite systemGeometry(systemCuboid);

        const System system = {systemMass, systemGeometry};

        EXPECT_FALSE(system != system);

        // Test for different mass
        const Mass systemMass_0 = {90.0, Mass::Unit::Kilogram};
        const System system_1 = {systemMass_0, systemGeometry};

        EXPECT_TRUE(system != system_1);

        // Test for different geometry
        const Point center_0 = {1.0, 0.0, 0.0};
        const Cuboid systemCuboid_0 = {center_0, axes, extent};
        const Composite systemGeometry_0(systemCuboid_0);
        const System system_2 = {systemMass, systemGeometry_0};

        EXPECT_TRUE(system != system_2);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System, IsDefined)
{
    using ostk::core::type::Integer;
    using ostk::core::type::Real;
    using ostk::core::type::String;

    using ostk::mathematics::geometry::d3::object::Composite;
    using ostk::mathematics::geometry::d3::object::Cuboid;
    using ostk::mathematics::geometry::d3::object::Point;
    using ostk::mathematics::object::Matrix3d;
    using ostk::mathematics::object::Vector3d;

    using ostk::physics::unit::Mass;

    using ostk::astro::flight::System;

    {
        // Define satellite mass
        const Mass systemMass = {100.0, Mass::Unit::Kilogram};

        // Define satellite geometry
        const Point center = {0.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

        const Cuboid systemCuboid = {center, axes, extent};
        const Composite systemGeometry(systemCuboid);

        EXPECT_TRUE(System(systemMass, systemGeometry).isDefined());

        EXPECT_FALSE(System(Mass::Undefined(), systemGeometry).isDefined());

        EXPECT_FALSE(System(systemMass, Composite::Undefined()).isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System, StreamOperator)
{
    using ostk::core::type::Integer;
    using ostk::core::type::Real;
    using ostk::core::type::String;

    using ostk::mathematics::geometry::d3::object::Composite;
    using ostk::mathematics::geometry::d3::object::Cuboid;
    using ostk::mathematics::geometry::d3::object::Point;
    using ostk::mathematics::object::Matrix3d;
    using ostk::mathematics::object::Vector3d;

    using ostk::physics::unit::Mass;

    using ostk::astro::flight::System;

    {
        // Define satellite mass
        const Mass systemMass = {100.0, Mass::Unit::Kilogram};

        // Define satellite geometry
        const Point center = {0.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

        const Cuboid systemCuboid = {center, axes, extent};
        const Composite systemGeometry(systemCuboid);

        // Construct System object
        const System system = {systemMass, systemGeometry};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << system << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System, Print)
{
    using ostk::core::type::Integer;
    using ostk::core::type::Real;
    using ostk::core::type::String;

    using ostk::mathematics::geometry::d3::object::Composite;
    using ostk::mathematics::geometry::d3::object::Cuboid;
    using ostk::mathematics::geometry::d3::object::Point;
    using ostk::mathematics::object::Matrix3d;
    using ostk::mathematics::object::Vector3d;

    using ostk::physics::unit::Mass;

    using ostk::astro::flight::System;

    {
        // Define satellite mass
        const Mass systemMass = {100.0, Mass::Unit::Kilogram};

        // Define satellite geometry
        const Point center = {0.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

        const Cuboid systemCuboid = {center, axes, extent};
        const Composite systemGeometry(systemCuboid);

        // Construct System object
        const System system = {systemMass, systemGeometry};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(system.print(std::cout, true));
        EXPECT_NO_THROW(system.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System, getMass)
{
    using ostk::core::type::Integer;
    using ostk::core::type::Real;
    using ostk::core::type::String;

    using ostk::mathematics::geometry::d3::object::Composite;
    using ostk::mathematics::geometry::d3::object::Cuboid;
    using ostk::mathematics::geometry::d3::object::Point;
    using ostk::mathematics::object::Matrix3d;
    using ostk::mathematics::object::Vector3d;

    using ostk::physics::unit::Mass;

    using ostk::astro::flight::System;

    {
        // Define satellite mass
        const Mass systemMass = {100.0, Mass::Unit::Kilogram};

        // Construct System object
        const System system = {systemMass, Composite::Undefined()};

        EXPECT_EQ(system.getMass(), systemMass);
    }

    {  // Define satellite geometry
        const Point center = {0.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

        const Cuboid systemCuboid = {center, axes, extent};
        const Composite systemGeometry(systemCuboid);

        // Construct System object
        const System system = {Mass::Undefined(), systemGeometry};

        EXPECT_ANY_THROW(system.getMass());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System, getGeometry)
{
    using ostk::core::type::Integer;
    using ostk::core::type::Real;
    using ostk::core::type::String;

    using ostk::mathematics::geometry::d3::object::Composite;
    using ostk::mathematics::geometry::d3::object::Cuboid;
    using ostk::mathematics::geometry::d3::object::Point;
    using ostk::mathematics::object::Matrix3d;
    using ostk::mathematics::object::Vector3d;

    using ostk::physics::unit::Mass;

    using ostk::astro::flight::System;

    {
        // Define satellite geometry
        const Point center = {0.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

        const Cuboid systemCuboid = {center, axes, extent};
        const Composite systemGeometry(systemCuboid);

        // Construct System object
        const System system = {Mass::Undefined(), systemGeometry};

        EXPECT_EQ(system.getGeometry(), systemGeometry);
    }

    {
        // Define satellite mass
        const Mass systemMass = {100.0, Mass::Unit::Kilogram};

        // Construct System object
        const System system = {systemMass, Composite::Undefined()};

        EXPECT_ANY_THROW(system.getGeometry());
    }
}
