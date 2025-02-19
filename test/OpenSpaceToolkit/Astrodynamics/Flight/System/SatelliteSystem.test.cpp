/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>

#include <Global.test.hpp>

using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;

using ostk::physics::unit::Mass;

using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;

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
        satelliteGeometry_ = Composite(satelliteCuboid);

        // Define satellite cross sectional surface area
        crossSectionalSurfaceArea_ = 0.8;

        // Define satellite coefficient of drag
        dragCoefficient_ = 1.2;

        // Define propulsion system
        propulsionSystem_ = {
            0.01,
            100.0,
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
    Composite satelliteGeometry_ = Composite::Undefined();
    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, Constructor)
{
    {
        EXPECT_NO_THROW(
            SatelliteSystem satelliteSystem(
                mass_,
                satelliteGeometry_,
                satelliteInertiaTensor_,
                crossSectionalSurfaceArea_,
                dragCoefficient_,
                propulsionSystem_
            )
        );
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
        EXPECT_TRUE(
            satelliteSystem_ == SatelliteSystem(
                                    mass_,
                                    satelliteGeometry_,
                                    satelliteInertiaTensor_,
                                    crossSectionalSurfaceArea_,
                                    dragCoefficient_,
                                    propulsionSystem_
                                )
        );
    }

    {
        EXPECT_FALSE(
            satelliteSystem_ == SatelliteSystem(
                                    {99.0, Mass::Unit::Kilogram},
                                    satelliteGeometry_,
                                    satelliteInertiaTensor_,
                                    crossSectionalSurfaceArea_,
                                    dragCoefficient_,
                                    propulsionSystem_
                                )
        );
    }

    {
        const Point center = {1.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};
        const Composite anotherSatelliteGeometry(Cuboid(center, axes, extent));
        EXPECT_FALSE(
            satelliteSystem_ == SatelliteSystem(
                                    mass_,
                                    anotherSatelliteGeometry,
                                    satelliteInertiaTensor_,
                                    crossSectionalSurfaceArea_,
                                    dragCoefficient_,
                                    propulsionSystem_
                                )
        );
    }

    {
        Matrix3d anotherSatelliteInertiaTensor = satelliteInertiaTensor_;
        anotherSatelliteInertiaTensor(0, 0) = satelliteInertiaTensor_(0, 0) + 1.0;
        EXPECT_FALSE(
            satelliteSystem_ == SatelliteSystem(
                                    mass_,
                                    satelliteGeometry_,
                                    anotherSatelliteInertiaTensor,
                                    crossSectionalSurfaceArea_,
                                    dragCoefficient_,
                                    propulsionSystem_
                                )
        );
    }

    {
        EXPECT_FALSE(
            satelliteSystem_ == SatelliteSystem(
                                    mass_,
                                    satelliteGeometry_,
                                    satelliteInertiaTensor_,
                                    crossSectionalSurfaceArea_ + 1.0,
                                    dragCoefficient_,
                                    propulsionSystem_
                                )
        );
    }

    {
        EXPECT_FALSE(
            satelliteSystem_ == SatelliteSystem(
                                    mass_,
                                    satelliteGeometry_,
                                    satelliteInertiaTensor_,
                                    crossSectionalSurfaceArea_,
                                    dragCoefficient_ + 1.0,
                                    propulsionSystem_
                                )
        );
    }

    {
        PropulsionSystem anotherPropulsionSystem = {0.099, 99.0};
        EXPECT_FALSE(
            satelliteSystem_ == SatelliteSystem(
                                    mass_,
                                    satelliteGeometry_,
                                    satelliteInertiaTensor_,
                                    crossSectionalSurfaceArea_,
                                    dragCoefficient_,
                                    anotherPropulsionSystem
                                )
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, NotEqualToOperator)
{
    {
        EXPECT_FALSE(
            satelliteSystem_ != SatelliteSystem(
                                    mass_,
                                    satelliteGeometry_,
                                    satelliteInertiaTensor_,
                                    crossSectionalSurfaceArea_,
                                    dragCoefficient_,
                                    propulsionSystem_
                                )
        );
    }

    {
        EXPECT_TRUE(
            satelliteSystem_ != SatelliteSystem(
                                    {99.0, Mass::Unit::Kilogram},
                                    satelliteGeometry_,
                                    satelliteInertiaTensor_,
                                    crossSectionalSurfaceArea_,
                                    dragCoefficient_,
                                    propulsionSystem_
                                )
        );
    }

    {
        const Point center = {1.0, 0.0, 0.0};
        const std::array<Vector3d, 3> axes = {
            Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}
        };
        const std::array<Real, 3> extent = {1.0, 2.0, 3.0};
        const Composite anotherSatelliteGeometry(Cuboid(center, axes, extent));
        EXPECT_TRUE(
            satelliteSystem_ != SatelliteSystem(
                                    mass_,
                                    anotherSatelliteGeometry,
                                    satelliteInertiaTensor_,
                                    crossSectionalSurfaceArea_,
                                    dragCoefficient_,
                                    propulsionSystem_
                                )
        );
    }

    {
        Matrix3d anotherSatelliteInertiaTensor = satelliteInertiaTensor_;
        anotherSatelliteInertiaTensor(0, 0) = satelliteInertiaTensor_(0, 0) + 1.0;
        EXPECT_TRUE(
            satelliteSystem_ != SatelliteSystem(
                                    mass_,
                                    satelliteGeometry_,
                                    anotherSatelliteInertiaTensor,
                                    crossSectionalSurfaceArea_,
                                    dragCoefficient_,
                                    propulsionSystem_
                                )
        );
    }

    {
        EXPECT_TRUE(
            satelliteSystem_ != SatelliteSystem(
                                    mass_,
                                    satelliteGeometry_,
                                    satelliteInertiaTensor_,
                                    crossSectionalSurfaceArea_ + 1.0,
                                    dragCoefficient_,
                                    propulsionSystem_
                                )
        );
    }

    {
        EXPECT_TRUE(
            satelliteSystem_ != SatelliteSystem(
                                    mass_,
                                    satelliteGeometry_,
                                    satelliteInertiaTensor_,
                                    crossSectionalSurfaceArea_,
                                    dragCoefficient_ + 1.0,
                                    propulsionSystem_
                                )
        );
    }

    {
        PropulsionSystem anotherPropulsionSystem = {0.099, 99.0};
        EXPECT_TRUE(
            satelliteSystem_ != SatelliteSystem(
                                    mass_,
                                    satelliteGeometry_,
                                    satelliteInertiaTensor_,
                                    crossSectionalSurfaceArea_,
                                    dragCoefficient_,
                                    anotherPropulsionSystem
                                )
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, IsDefined)
{
    {
        EXPECT_TRUE(satelliteSystem_.isDefined());
    }

    {
        EXPECT_FALSE(SatelliteSystem(
                         Mass::Undefined(),
                         satelliteGeometry_,
                         satelliteInertiaTensor_,
                         crossSectionalSurfaceArea_,
                         dragCoefficient_,
                         propulsionSystem_
        )
                         .isDefined());

        EXPECT_FALSE(SatelliteSystem(
                         mass_,
                         Composite::Undefined(),
                         satelliteInertiaTensor_,
                         crossSectionalSurfaceArea_,
                         dragCoefficient_,
                         propulsionSystem_
        )
                         .isDefined());

        EXPECT_FALSE(SatelliteSystem(
                         mass_,
                         satelliteGeometry_,
                         Matrix3d::Undefined(),
                         crossSectionalSurfaceArea_,
                         dragCoefficient_,
                         propulsionSystem_
        )
                         .isDefined());

        EXPECT_FALSE(SatelliteSystem(
                         mass_,
                         satelliteGeometry_,
                         satelliteInertiaTensor_,
                         Real::Undefined(),
                         dragCoefficient_,
                         propulsionSystem_
        )
                         .isDefined());

        EXPECT_FALSE(SatelliteSystem(
                         mass_,
                         satelliteGeometry_,
                         satelliteInertiaTensor_,
                         crossSectionalSurfaceArea_,
                         Real::Undefined(),
                         propulsionSystem_
        )
                         .isDefined());

        EXPECT_FALSE(SatelliteSystem(
                         mass_,
                         satelliteGeometry_,
                         satelliteInertiaTensor_,
                         crossSectionalSurfaceArea_,
                         dragCoefficient_,
                         PropulsionSystem::Undefined()
        )
                         .isDefined());
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
        const SatelliteSystem aSatelliteSystem = {
            Mass::Undefined(),
            Composite::Undefined(),
            satelliteInertiaTensor_,
            Real::Undefined(),
            Real::Undefined(),
            PropulsionSystem::Undefined()
        };
        EXPECT_EQ(satelliteSystem_.getInertiaTensor(), satelliteInertiaTensor_);
    }

    {
        const SatelliteSystem aSatelliteSystem = {
            mass_,
            satelliteGeometry_,
            Matrix3d::Undefined(),
            crossSectionalSurfaceArea_,
            dragCoefficient_,
            propulsionSystem_
        };
        EXPECT_ANY_THROW(aSatelliteSystem.getInertiaTensor());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, GetCrossSectionalSurfaceArea)
{
    {
        const SatelliteSystem aSatelliteSystem = {
            Mass::Undefined(),
            Composite::Undefined(),
            Matrix3d::Undefined(),
            crossSectionalSurfaceArea_,
            Real::Undefined(),
            PropulsionSystem::Undefined()
        };
        EXPECT_EQ(satelliteSystem_.getCrossSectionalSurfaceArea(), crossSectionalSurfaceArea_);
    }

    {
        const SatelliteSystem aSatelliteSystem = {
            mass_, satelliteGeometry_, satelliteInertiaTensor_, Real::Undefined(), dragCoefficient_, propulsionSystem_
        };
        EXPECT_ANY_THROW(aSatelliteSystem.getCrossSectionalSurfaceArea());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, GetDragCoefficient)
{
    {
        const SatelliteSystem aSatelliteSystem = {
            Mass::Undefined(),
            Composite::Undefined(),
            Matrix3d::Undefined(),
            Real::Undefined(),
            dragCoefficient_,
            PropulsionSystem::Undefined()
        };
        EXPECT_EQ(satelliteSystem_.getDragCoefficient(), dragCoefficient_);
    }

    {
        const SatelliteSystem aSatelliteSystem = {
            mass_,
            satelliteGeometry_,
            satelliteInertiaTensor_,
            crossSectionalSurfaceArea_,
            Real::Undefined(),
            propulsionSystem_
        };
        EXPECT_ANY_THROW(aSatelliteSystem.getDragCoefficient());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, GetPropulsionSystem)
{
    {
        const SatelliteSystem aSatelliteSystem = {
            Mass::Undefined(),
            Composite::Undefined(),
            Matrix3d::Undefined(),
            Real::Undefined(),
            Real::Undefined(),
            propulsionSystem_
        };
        EXPECT_EQ(satelliteSystem_.getPropulsionSystem(), propulsionSystem_);
    }

    {
        const SatelliteSystem aSatelliteSystem = {
            mass_,
            satelliteGeometry_,
            satelliteInertiaTensor_,
            crossSectionalSurfaceArea_,
            dragCoefficient_,
            PropulsionSystem::Undefined()
        };
        EXPECT_ANY_THROW(aSatelliteSystem.getPropulsionSystem());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, Undefined)
{
    {
        EXPECT_NO_THROW(SatelliteSystem::Undefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem, Default)
{
    {
        EXPECT_NO_THROW(SatelliteSystem::Default());
    }
}
