// /// Apache License 2.0

// #include <OpenSpaceToolkit/Core/Types/Integer.hpp>
// #include <OpenSpaceToolkit/Core/Types/Real.hpp>
// #include <OpenSpaceToolkit/Core/Types/String.hpp>

// #include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

// #include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

// #include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>

// #include <Global.test.hpp>


// TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, Constructor)
// {
//     using ostk::core::types::Integer;
//     using ostk::core::types::Real;
//     using ostk::core::types::String;

//     using ostk::math::geom::d3::objects::Composite;
//     using ostk::math::geom::d3::objects::Cuboid;
//     using ostk::math::geom::d3::objects::Point;
//     using ostk::math::obj::Matrix3d;
//     using ostk::math::obj::Vector3d;

//     using ostk::physics::units::Mass;

//     using ostk::astro::flight::system::PropulsionSystem;

//     // Normal constructor
//     {
//         // Define satellite mass
//         const Mass satelliteMass = {100.0, Mass::Unit::Kilogram};

//         // Define satellite intertia tensor
//         const Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

//         // Define satellite geometry
//         const Point center = {0.0, 0.0, 0.0};
//         const std::array<Vector3d, 3> axes = {
//             Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}};
//         const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

//         const Cuboid satelliteCuboid = {center, axes, extent};
//         const Composite satelliteGeometry(satelliteCuboid);

//         // Define satellite cross sectional surface area
//         const Real crossSectionalSurfaceArea = 0.8;

//         // Define satellite coefficient of drag
//         const Real dragCoefficient = 1.2;

//         // Construct PropulsionSystem object
//         EXPECT_NO_THROW(PropulsionSystem satellitesystem(
//             satelliteMass, satelliteGeometry, satelliteInertiaTensor, crossSectionalSurfaceArea, dragCoefficient
//         ));
//     }
// }

// TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, CopyConstructor)
// {
//     using ostk::core::types::Integer;
//     using ostk::core::types::Real;
//     using ostk::core::types::String;

//     using ostk::math::geom::d3::objects::Composite;
//     using ostk::math::geom::d3::objects::Cuboid;
//     using ostk::math::geom::d3::objects::Point;
//     using ostk::math::obj::Matrix3d;
//     using ostk::math::obj::Vector3d;

//     using ostk::physics::units::Mass;

//     using ostk::astro::flight::system::PropulsionSystem;

//     // Copy constructor
//     {
//         // Define satellite mass
//         const Mass satelliteMass = {100.0, Mass::Unit::Kilogram};

//         // Define satellite intertia tensor
//         const Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

//         // Define satellite geometry
//         const Point center = {0.0, 0.0, 0.0};
//         const std::array<Vector3d, 3> axes = {
//             Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}};
//         const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

//         const Cuboid satelliteCuboid = {center, axes, extent};
//         const Composite satelliteGeometry(satelliteCuboid);

//         // Define satellite cross sectional surface area
//         const Real crossSectionalSurfaceArea = 0.8;

//         // Define satellite coefficient of drag
//         const Real dragCoefficient = 1.2;

//         // Construct PropulsionSystem object
//         const PropulsionSystem satellitesystem = {
//             satelliteMass, satelliteGeometry, satelliteInertiaTensor, crossSectionalSurfaceArea, dragCoefficient};

//         EXPECT_NO_THROW(PropulsionSystem satellitesystemCopy(satellitesystem));
//     }
// }

// TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, EqualToOperator)
// {
//     using ostk::core::types::Integer;
//     using ostk::core::types::Real;
//     using ostk::core::types::String;

//     using ostk::math::geom::d3::objects::Composite;
//     using ostk::math::geom::d3::objects::Cuboid;
//     using ostk::math::geom::d3::objects::Point;
//     using ostk::math::obj::Matrix3d;
//     using ostk::math::obj::Vector3d;

//     using ostk::physics::units::Mass;

//     using ostk::astro::flight::system::PropulsionSystem;

//     {
//         // Test for all same parameters
//         // Define satellite mass
//         const Mass satelliteMass(90.0, Mass::Unit::Kilogram);

//         // Define satellite intertia tensor
//         Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

//         // Define satellite geometry
//         const Point center = {0.0, 0.0, 0.0};
//         const std::array<Vector3d, 3> axes = {
//             Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}};
//         const std::array<Real, 3> extent = {1.0, 2.0, 3.0};
//         const Cuboid satelliteCuboid = {center, axes, extent};
//         const Composite satelliteGeometry(satelliteCuboid);

//         // Test for default drag coefficient value
//         const PropulsionSystem satelliteSystem = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};
//         const PropulsionSystem satelliteSystem_0 = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};

//         EXPECT_TRUE(satelliteSystem == satelliteSystem_0);

//         // Test for different mass
//         const Mass satelliteMass_1(100.0, Mass::Unit::Kilogram);
//         const PropulsionSystem satelliteSystem_1 = {
//             satelliteMass_1, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};

//         EXPECT_FALSE(satelliteSystem == satelliteSystem_1);

//         // Test for different geometry
//         const Point center_0 = {1.0, 0.0, 0.0};
//         const Composite satelliteGeometry_0(Cuboid(center_0, axes, extent));
//         const PropulsionSystem satelliteSystem_3 = {
//             satelliteMass, satelliteGeometry_0, satelliteInertiaTensor, 0.8, 2.2};

//         EXPECT_FALSE(satelliteSystem == satelliteSystem_3);

//         // Test for different inertia tensor
//         Matrix3d satelliteInertiaTensor_0 = satelliteInertiaTensor;
//         satelliteInertiaTensor_0(0, 0) = 2.0;
//         const PropulsionSystem satelliteSystem_2 = {
//             satelliteMass, satelliteGeometry, satelliteInertiaTensor_0, 0.8, 2.2};

//         EXPECT_FALSE(satelliteSystem == satelliteSystem_2);

//         // Test for different area value
//         const PropulsionSystem satelliteSystem_4 = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.9, 2.2};
//         EXPECT_FALSE(satelliteSystem == satelliteSystem_4);

//         // Test for different drag coefficient value
//         const PropulsionSystem satelliteSystem_5 = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.3};
//         EXPECT_FALSE(satelliteSystem == satelliteSystem_5);
//     }
// }

// TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, NotEqualToOperator)
// {
//     using ostk::core::types::Integer;
//     using ostk::core::types::Real;
//     using ostk::core::types::String;

//     using ostk::math::geom::d3::objects::Composite;
//     using ostk::math::geom::d3::objects::Cuboid;
//     using ostk::math::geom::d3::objects::Point;
//     using ostk::math::obj::Matrix3d;
//     using ostk::math::obj::Vector3d;

//     using ostk::physics::units::Mass;

//     using ostk::astro::flight::system::PropulsionSystem;

//     {
//         // Test for all same parameters
//         // Define satellite mass
//         const Mass satelliteMass(90.0, Mass::Unit::Kilogram);

//         // Define satellite intertia tensor // TODO fix this initialization once I implement the Matrix3d class in OSTk
//         // math
//         Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

//         // Define satellite geometry
//         const Point center = {0.0, 0.0, 0.0};
//         const std::array<Vector3d, 3> axes = {
//             Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}};
//         const std::array<Real, 3> extent = {1.0, 2.0, 3.0};
//         const Cuboid satelliteCuboid = {center, axes, extent};
//         const Composite satelliteGeometry(satelliteCuboid);

//         // Test for default drag coefficient value
//         const PropulsionSystem satelliteSystem = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};
//         const PropulsionSystem satelliteSystem_0 = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};

//         EXPECT_FALSE(satelliteSystem != satelliteSystem_0);

//         // Test for different mass
//         const Mass satelliteMass_1(100.0, Mass::Unit::Kilogram);
//         const PropulsionSystem satelliteSystem_1 = {
//             satelliteMass_1, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};

//         EXPECT_TRUE(satelliteSystem != satelliteSystem_1);

//         // Test for different geometry
//         const Point center_0 = {1.0, 0.0, 0.0};
//         const Composite satelliteGeometry_0(Cuboid(center_0, axes, extent));
//         const PropulsionSystem satelliteSystem_3 = {
//             satelliteMass, satelliteGeometry_0, satelliteInertiaTensor, 0.8, 2.2};

//         EXPECT_TRUE(satelliteSystem != satelliteSystem_3);

//         // Test for different inertia tensor
//         Matrix3d satelliteInertiaTensor_0 = satelliteInertiaTensor;
//         satelliteInertiaTensor_0(0, 0) = 2.0;
//         const PropulsionSystem satelliteSystem_2 = {
//             satelliteMass, satelliteGeometry, satelliteInertiaTensor_0, 0.8, 2.2};

//         EXPECT_TRUE(satelliteSystem != satelliteSystem_2);

//         // Test for different area value
//         const PropulsionSystem satelliteSystem_4 = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.9, 2.2};
//         EXPECT_TRUE(satelliteSystem != satelliteSystem_4);

//         // Test for different drag coefficient value
//         const PropulsionSystem satelliteSystem_5 = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.3};
//         EXPECT_TRUE(satelliteSystem != satelliteSystem_5);
//     }
// }

// TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, IsDefined)
// {
//     using ostk::core::types::Integer;
//     using ostk::core::types::Real;
//     using ostk::core::types::String;

//     using ostk::math::geom::d3::objects::Composite;
//     using ostk::math::geom::d3::objects::Cuboid;
//     using ostk::math::geom::d3::objects::Point;
//     using ostk::math::obj::Matrix3d;
//     using ostk::math::obj::Vector3d;

//     using ostk::physics::units::Mass;

//     using ostk::astro::flight::system::PropulsionSystem;

//     {
//         // Define satellite mass
//         const Mass satelliteMass(90.0, Mass::Unit::Kilogram);

//         // Define satellite intertia tensor
//         const Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

//         // Define satellite geometry
//         const Point center = {0.0, 0.0, 0.0};
//         const std::array<Vector3d, 3> axes = {
//             Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}};
//         const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

//         const Cuboid satelliteCuboid = {center, axes, extent};
//         const Composite satelliteGeometry(satelliteCuboid);

//         // Construct PropulsionSystem object
//         const PropulsionSystem satelliteSystem = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};

//         EXPECT_TRUE(satelliteSystem.isDefined());
//     }
// }

// TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, StreamOperator)
// {
//     using ostk::core::types::Integer;
//     using ostk::core::types::Real;
//     using ostk::core::types::String;

//     using ostk::math::geom::d3::objects::Composite;
//     using ostk::math::geom::d3::objects::Cuboid;
//     using ostk::math::geom::d3::objects::Point;
//     using ostk::math::obj::Matrix3d;
//     using ostk::math::obj::Vector3d;

//     using ostk::physics::units::Mass;

//     using ostk::astro::flight::system::PropulsionSystem;

//     {
//         // Define satellite mass
//         const Mass satelliteMass(90.0, Mass::Unit::Kilogram);

//         // Define satellite intertia tensor
//         const Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

//         // Define satellite geometry
//         const Point center = {0.0, 0.0, 0.0};
//         const std::array<Vector3d, 3> axes = {
//             Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}};
//         const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

//         const Cuboid satelliteCuboid = {center, axes, extent};
//         const Composite satelliteGeometry(satelliteCuboid);

//         // Construct PropulsionSystem object
//         const PropulsionSystem satelliteSystem = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};

//         testing::internal::CaptureStdout();

//         EXPECT_NO_THROW(std::cout << satelliteSystem << std::endl);

//         EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
//     }
// }

// TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, Print)
// {
//     using ostk::core::types::Integer;
//     using ostk::core::types::Real;
//     using ostk::core::types::String;

//     using ostk::math::geom::d3::objects::Composite;
//     using ostk::math::geom::d3::objects::Cuboid;
//     using ostk::math::geom::d3::objects::Point;
//     using ostk::math::obj::Matrix3d;
//     using ostk::math::obj::Vector3d;

//     using ostk::physics::units::Mass;

//     using ostk::astro::flight::system::PropulsionSystem;

//     {
//         // Define satellite mass
//         const Mass satelliteMass(90.0, Mass::Unit::Kilogram);

//         // Define satellite intertia tensor
//         const Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

//         // Define satellite geometry
//         const Point center = {0.0, 0.0, 0.0};
//         const std::array<Vector3d, 3> axes = {
//             Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}};
//         const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

//         const Cuboid satelliteCuboid = {center, axes, extent};
//         const Composite satelliteGeometry(satelliteCuboid);

//         // Construct PropulsionSystem object
//         const PropulsionSystem satelliteSystem = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};

//         testing::internal::CaptureStdout();

//         EXPECT_NO_THROW(satelliteSystem.print(std::cout, true));
//         EXPECT_NO_THROW(satelliteSystem.print(std::cout, false));
//         EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
//     }
// }

// TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, getMass)
// {
//     using ostk::core::types::Integer;
//     using ostk::core::types::Real;
//     using ostk::core::types::String;

//     using ostk::math::geom::d3::objects::Composite;
//     using ostk::math::geom::d3::objects::Cuboid;
//     using ostk::math::geom::d3::objects::Point;
//     using ostk::math::obj::Matrix3d;
//     using ostk::math::obj::Vector3d;

//     using ostk::physics::units::Mass;

//     using ostk::astro::flight::system::PropulsionSystem;

//     {
//         // Define satellite mass
//         const Mass satelliteMass(90.0, Mass::Unit::Kilogram);

//         // Define satellite intertia tensor
//         const Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

//         // Define satellite geometry
//         const Point center = {0.0, 0.0, 0.0};
//         const std::array<Vector3d, 3> axes = {
//             Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}};
//         const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

//         const Cuboid satelliteCuboid = {center, axes, extent};
//         const Composite satelliteGeometry(satelliteCuboid);

//         // Construct PropulsionSystem object
//         const PropulsionSystem satelliteSystem = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};

//         EXPECT_EQ(satelliteSystem.getMass(), satelliteMass);
//     }
// }

// TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, getGeometry)
// {
//     using ostk::core::types::Integer;
//     using ostk::core::types::Real;
//     using ostk::core::types::String;

//     using ostk::math::geom::d3::objects::Composite;
//     using ostk::math::geom::d3::objects::Cuboid;
//     using ostk::math::geom::d3::objects::Point;
//     using ostk::math::obj::Matrix3d;
//     using ostk::math::obj::Vector3d;

//     using ostk::physics::units::Mass;

//     using ostk::astro::flight::system::PropulsionSystem;

//     {
//         // Define satellite mass
//         const Mass satelliteMass(90.0, Mass::Unit::Kilogram);

//         // Define satellite intertia tensor
//         const Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

//         // Define satellite geometry
//         const Point center = {0.0, 0.0, 0.0};
//         const std::array<Vector3d, 3> axes = {
//             Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}};
//         const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

//         const Cuboid satelliteCuboid = {center, axes, extent};
//         const Composite satelliteGeometry(satelliteCuboid);

//         // Construct PropulsionSystem object
//         const PropulsionSystem satelliteSystem = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};

//         EXPECT_EQ(satelliteSystem.getGeometry(), satelliteGeometry);
//     }
// }

// TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, getInertiaTensor)
// {
//     using ostk::core::types::Integer;
//     using ostk::core::types::Real;
//     using ostk::core::types::String;

//     using ostk::math::geom::d3::objects::Composite;
//     using ostk::math::geom::d3::objects::Cuboid;
//     using ostk::math::geom::d3::objects::Point;
//     using ostk::math::obj::Matrix3d;
//     using ostk::math::obj::Vector3d;

//     using ostk::physics::units::Mass;

//     using ostk::astro::flight::system::PropulsionSystem;

//     {
//         // Define satellite mass
//         const Mass satelliteMass(90.0, Mass::Unit::Kilogram);

//         // Define satellite intertia tensor
//         const Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

//         // Define satellite geometry
//         const Point center = {0.0, 0.0, 0.0};
//         const std::array<Vector3d, 3> axes = {
//             Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}};
//         const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

//         const Cuboid satelliteCuboid = {center, axes, extent};
//         const Composite satelliteGeometry(satelliteCuboid);

//         // Construct PropulsionSystem object
//         const PropulsionSystem satelliteSystem = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};

//         EXPECT_EQ(satelliteSystem.getInertiaTensor(), satelliteInertiaTensor);
//     }
// }

// TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, getCrossSectionalSurfaceArea)
// {
//     using ostk::core::types::Integer;
//     using ostk::core::types::Real;
//     using ostk::core::types::String;

//     using ostk::math::geom::d3::objects::Composite;
//     using ostk::math::geom::d3::objects::Cuboid;
//     using ostk::math::geom::d3::objects::Point;
//     using ostk::math::obj::Matrix3d;
//     using ostk::math::obj::Vector3d;

//     using ostk::physics::units::Mass;

//     using ostk::astro::flight::system::PropulsionSystem;

//     {
//         // Define satellite mass
//         const Mass satelliteMass(90.0, Mass::Unit::Kilogram);

//         // Define satellite intertia tensor
//         const Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

//         // Define satellite geometry
//         const Point center = {0.0, 0.0, 0.0};
//         const std::array<Vector3d, 3> axes = {
//             Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}};
//         const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

//         const Cuboid satelliteCuboid = {center, axes, extent};
//         const Composite satelliteGeometry(satelliteCuboid);

//         // Define satellite coefficient of drag
//         const Real surfaceArea = 0.8;

//         // Construct PropulsionSystem object
//         const PropulsionSystem satelliteSystem = {
//             satelliteMass, satelliteGeometry, satelliteInertiaTensor, surfaceArea, 2.2};

//         EXPECT_EQ(satelliteSystem.getCrossSectionalSurfaceArea(), surfaceArea);
//     }
// }

// TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, getDragCoefficient)
// {
//     using ostk::core::types::Integer;
//     using ostk::core::types::Real;
//     using ostk::core::types::String;

//     using ostk::math::geom::d3::objects::Composite;
//     using ostk::math::geom::d3::objects::Cuboid;
//     using ostk::math::geom::d3::objects::Point;
//     using ostk::math::obj::Matrix3d;
//     using ostk::math::obj::Vector3d;

//     using ostk::physics::units::Mass;

//     using ostk::astro::flight::system::PropulsionSystem;

//     {
//         // Define satellite mass
//         const Mass satelliteMass(90.0, Mass::Unit::Kilogram);

//         // Define satellite intertia tensor
//         const Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

//         // Define satellite geometry
//         const Point center = {0.0, 0.0, 0.0};
//         const std::array<Vector3d, 3> axes = {
//             Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}};
//         const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

//         const Cuboid satelliteCuboid = {center, axes, extent};
//         const Composite satelliteGeometry(satelliteCuboid);

//         // Define satellite coefficient of drag
//         const Real dragCoefficient = 2.2;

//         // Construct PropulsionSystem object
//         const PropulsionSystem satelliteSystem = {
//             satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, dragCoefficient};

//         EXPECT_EQ(satelliteSystem.getDragCoefficient(), dragCoefficient);
//     }
// }
