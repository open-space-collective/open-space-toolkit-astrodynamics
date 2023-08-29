/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>

#include <Global.test.hpp>

using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::geom::d3::objects::Composite;
using ostk::math::geom::d3::objects::Cuboid;
using ostk::math::geom::d3::objects::Point;
using ostk::math::obj::Matrix3d;
using ostk::math::obj::Vector3d;

using ostk::physics::data::Scalar;
using ostk::physics::units::Length;
using ostk::physics::units::Mass;
using ostk::physics::units::Time;
using ostk::physics::units::ElectricCurrent;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::Unit;

using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::PropulsionSystem;

class OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        // Define satellite mass
        mass_ = {100.0, Mass::Unit::Kilogram};

        // Define satellite intertia tensor
        satelliteInertiaTensor_ = Matrix3d::Identity();

        // Define satellite geometry
        const Point center = {0.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

        const Cuboid satelliteCuboid = {center, axes, extent};
        const Composite satelliteGeometry_(satelliteCuboid);

        // Define satellite cross sectional surface area
        crossSectionalSurfaceArea_ = 0.8;

        // Define satellite coefficient of drag
        dragCoefficient_ = 1.2;

        // Define propulsion system
        const Scalar thrust_ = Scalar(0.01, PropulsionSystem::thrustSIUnit);
        const Scalar specificImpulse_ = Scalar(100.0, PropulsionSystem::specificImpulseSIUnit);

        propulsionSystem_ = {
            thrust_,
            specificImpulse_,
        };

        satelliteSystem_ = {
            mass_,
            satelliteGeometry_,
            satelliteInertiaTensor_,
            crossSectionalSurfaceArea_,
            dragCoefficient_,
            propulsionSystem_,
        };
    }

    Mass mass_ = Mass::Undefined();
    Matrix3d satelliteInertiaTensor_ = Matrix3d::Identity();
    Real crossSectionalSurfaceArea_ = Real::Undefined();
    Real dragCoefficient_ = Real::Undefined();
    PropulsionSystem propulsionSystem_ = PropulsionSystem::Undefined();
    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, Constructor)
{
    // Normal constructor
    {
        // Define satellite mass
        const Mass satelliteMass = {100.0, Mass::Unit::Kilogram};

        // Define satellite intertia tensor
        const Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

        // Define satellite geometry
        const Point center = {0.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

        const Cuboid satelliteCuboid = {center, axes, extent};
        const Composite satelliteGeometry(satelliteCuboid);

        // Define satellite cross sectional surface area
        const Real crossSectionalSurfaceArea = 0.8;

        // Define satellite coefficient of drag
        const Real dragCoefficient = 1.2;

        // Construct SatelliteSystem object
        EXPECT_NO_THROW(SatelliteSystem satelliteSystem(
            satelliteMass, satelliteGeometry, satelliteInertiaTensor, crossSectionalSurfaceArea, dragCoefficient
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, CopyConstructor)
{
    {
        EXPECT_NO_THROW(SatelliteSystem satelliteSystemCopy(satelliteSystem_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, EqualToOperator)
{
    {
        // Test for all same parameters
        // Define satellite mass
        const Mass satelliteMass(90.0, Mass::Unit::Kilogram);

        // Define satellite intertia tensor
        Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

        // Define satellite geometry
        const Point center = {0.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};
        const Cuboid satelliteCuboid = {center, axes, extent};
        const Composite satelliteGeometry(satelliteCuboid);

        // Test for default drag coefficient value
        const SatelliteSystem satelliteSystem = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};
        const SatelliteSystem satelliteSystem_0 = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};

        EXPECT_TRUE(satelliteSystem == satelliteSystem_0);

        // Test for different mass
        const Mass satelliteMass_1(100.0, Mass::Unit::Kilogram);
        const SatelliteSystem satelliteSystem_1 = {
            satelliteMass_1, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2
        };

        EXPECT_FALSE(satelliteSystem == satelliteSystem_1);

        // Test for different geometry
        const Point center_0 = {1.0, 0.0, 0.0};
        const Composite satelliteGeometry_0(Cuboid(center_0, axes, extent));
        const SatelliteSystem satelliteSystem_3 = {
            satelliteMass, satelliteGeometry_0, satelliteInertiaTensor, 0.8, 2.2
        };

        EXPECT_FALSE(satelliteSystem == satelliteSystem_3);

        // Test for different inertia tensor
        Matrix3d satelliteInertiaTensor_0 = satelliteInertiaTensor;
        satelliteInertiaTensor_0(0, 0) = 2.0;
        const SatelliteSystem satelliteSystem_2 = {
            satelliteMass, satelliteGeometry, satelliteInertiaTensor_0, 0.8, 2.2
        };

        EXPECT_FALSE(satelliteSystem == satelliteSystem_2);

        // Test for different area value
        const SatelliteSystem satelliteSystem_4 = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.9, 2.2};
        EXPECT_FALSE(satelliteSystem == satelliteSystem_4);

        // Test for different drag coefficient value
        const SatelliteSystem satelliteSystem_5 = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.3};
        EXPECT_FALSE(satelliteSystem == satelliteSystem_5);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, NotEqualToOperator)
{
    {
        // Test for all same parameters
        // Define satellite mass
        const Mass satelliteMass(90.0, Mass::Unit::Kilogram);

        // Define satellite intertia tensor // TODO fix this initialization once I implement the Matrix3d class in OSTk
        // math
        Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

        // Define satellite geometry
        const Point center = {0.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};
        const Cuboid satelliteCuboid = {center, axes, extent};
        const Composite satelliteGeometry(satelliteCuboid);

        // Test for default drag coefficient value
        const SatelliteSystem satelliteSystem = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};
        const SatelliteSystem satelliteSystem_0 = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};

        EXPECT_FALSE(satelliteSystem != satelliteSystem_0);

        // Test for different mass
        const Mass satelliteMass_1(100.0, Mass::Unit::Kilogram);
        const SatelliteSystem satelliteSystem_1 = {
            satelliteMass_1, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2
        };

        EXPECT_TRUE(satelliteSystem != satelliteSystem_1);

        // Test for different geometry
        const Point center_0 = {1.0, 0.0, 0.0};
        const Composite satelliteGeometry_0(Cuboid(center_0, axes, extent));
        const SatelliteSystem satelliteSystem_3 = {
            satelliteMass, satelliteGeometry_0, satelliteInertiaTensor, 0.8, 2.2
        };

        EXPECT_TRUE(satelliteSystem != satelliteSystem_3);

        // Test for different inertia tensor
        Matrix3d satelliteInertiaTensor_0 = satelliteInertiaTensor;
        satelliteInertiaTensor_0(0, 0) = 2.0;
        const SatelliteSystem satelliteSystem_2 = {
            satelliteMass, satelliteGeometry, satelliteInertiaTensor_0, 0.8, 2.2
        };

        EXPECT_TRUE(satelliteSystem != satelliteSystem_2);

        // Test for different area value
        const SatelliteSystem satelliteSystem_4 = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.9, 2.2};
        EXPECT_TRUE(satelliteSystem != satelliteSystem_4);

        // Test for different drag coefficient value
        const SatelliteSystem satelliteSystem_5 = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.3};
        EXPECT_TRUE(satelliteSystem != satelliteSystem_5);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, IsDefined)
{
    {
        EXPECT_TRUE(satelliteSystem_.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << satelliteSystem_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(satelliteSystem_.print(std::cout, true));
        EXPECT_NO_THROW(satelliteSystem_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, GetMass)
{
    {
        EXPECT_EQ(satelliteSystem_.getMass(), mass_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, GetGeometry)
{
    {
        // Define satellite mass
        const Mass satelliteMass(90.0, Mass::Unit::Kilogram);

        // Define satellite intertia tensor
        const Matrix3d satelliteInertiaTensor = Matrix3d::Identity();

        // Define satellite geometry
        const Point center = {0.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};

        const Cuboid satelliteCuboid = {center, axes, extent};
        const Composite satelliteGeometry(satelliteCuboid);

        // Construct SatelliteSystem object
        const SatelliteSystem satelliteSystem = {satelliteMass, satelliteGeometry, satelliteInertiaTensor, 0.8, 2.2};

        EXPECT_EQ(satelliteSystem.getGeometry(), satelliteGeometry);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, GetInertiaTensor)
{
    {
        EXPECT_EQ(satelliteSystem_.getInertiaTensor(), satelliteInertiaTensor_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, GetCrossSectionalSurfaceArea)
{
    {
        EXPECT_EQ(satelliteSystem_.getCrossSectionalSurfaceArea(), crossSectionalSurfaceArea_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, GetDragCoefficient)
{
    {
        EXPECT_EQ(satelliteSystem_.getDragCoefficient(), dragCoefficient_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, GetPropulsionSystem)
{
    {
        EXPECT_EQ(satelliteSystem_.getPropulsionSystem(), propulsionSystem_);
    }
    
    {
        EXPECT_ANY_THROW(SatelliteSystem::Undefined().getPropulsionSystem());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, Undefined)
{
    {
        EXPECT_NO_THROW(SatelliteSystem::Undefined());
    }
}
