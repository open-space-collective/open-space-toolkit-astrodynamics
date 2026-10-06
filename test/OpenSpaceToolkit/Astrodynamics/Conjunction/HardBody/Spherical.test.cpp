/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody/Spherical.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astrodynamics::conjunction::HardBody;
using ostk::astrodynamics::conjunction::hardbody::Spherical;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::StateBuilder;

class OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Spherical : public ::testing::Test
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

    const Spherical spherical_ = {2.0};

    const State state_ = buildState({7.0e6, 0.0, 0.0}, {0.0, 7.5e3, 0.0});
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Spherical, Constructor)
{
    {
        EXPECT_NO_THROW(Spherical(1.0));
    }

    {
        EXPECT_NO_THROW(Spherical(0.0));
    }

    {
        const HardBody& hardBody = spherical_;

        EXPECT_NO_THROW(hardBody.calculateCrossSectionAt(state_, Frame::GCRF()));
    }

    {
        EXPECT_THROW(
            try { Spherical(Real::Undefined()); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Radius} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            try { Spherical(-1.0); } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ("Radius must be greater than or equal to 0.", e.getMessage());
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Spherical, GetRadius)
{
    {
        EXPECT_EQ(2.0, spherical_.getRadius());
    }

    {
        EXPECT_EQ(0.0, Spherical(0.0).getRadius());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Spherical, CalculateCrossSectionAt)
{
    {
        const HardBody::CrossSection crossSection = spherical_.calculateCrossSectionAt(state_, Frame::GCRF());

        EXPECT_DOUBLE_EQ(2.0, crossSection.getEnclosingRadius());
    }

    // Cross section does not depend on the state or frame
    {
        EXPECT_DOUBLE_EQ(2.0, spherical_.calculateCrossSectionAt(state_, Frame::ITRF()).getEnclosingRadius());

        const State anotherState = buildState({0.0, -4.2e7, 1.0e3}, {3.0e3, 0.0, 0.0});

        EXPECT_DOUBLE_EQ(2.0, spherical_.calculateCrossSectionAt(anotherState, Frame::GCRF()).getEnclosingRadius());
    }

    // Zero radius: point cross section
    {
        EXPECT_DOUBLE_EQ(0.0, Spherical(0.0).calculateCrossSectionAt(state_, Frame::GCRF()).getEnclosingRadius());
    }

    // Combining two spherical cross sections gives a circle with the sum of the radii
    {
        const Spherical anotherSpherical = {3.0};

        const HardBody::CrossSection combinedCrossSection = HardBody::CrossSection::combine(
            spherical_.calculateCrossSectionAt(state_, Frame::GCRF()),
            anotherSpherical.calculateCrossSectionAt(state_, Frame::GCRF())
        );

        EXPECT_DOUBLE_EQ(5.0, combinedCrossSection.getEnclosingRadius());
    }
}
