/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Pair;
using ostk::core::types::Index;
using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::NumericalSolver;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;

class DynamicsMock : public Dynamics
{
   public:
    DynamicsMock(const String& aName)
        : Dynamics(aName) {};

    MOCK_METHOD(Dynamics*, clone, (), (const, override));

    MOCK_METHOD(bool, isDefined, (), (const, override));

    MOCK_METHOD(Array<Shared<const CoordinatesSubset>>, getReadCoordinatesSubsets, (), (const, override));

    MOCK_METHOD(Array<Shared<const CoordinatesSubset>>, getWriteCoordinatesSubsets, (), (const, override));

    MOCK_METHOD(
        VectorXd,
        computeContribution,
        (const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr),
        (const, override)
    );
};

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics, Getters)
{
    {
        EXPECT_EQ("A name", DynamicsMock("A name").getName());
    }
}
