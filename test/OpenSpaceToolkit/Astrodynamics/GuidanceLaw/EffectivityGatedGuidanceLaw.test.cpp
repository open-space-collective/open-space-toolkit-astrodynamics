/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityGatedGuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

#include <Global.test.hpp>

using ostk::core::container::Map;
using ostk::core::container::Tuple;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::GuidanceLaw;
using ostk::astrodynamics::guidancelaw::ConstantThrust;
using ostk::astrodynamics::guidancelaw::EffectivityGatedGuidanceLaw;
using ostk::astrodynamics::guidancelaw::EffectivityProvider;
using ostk::astrodynamics::guidancelaw::QLaw;
using QLawProvider = ostk::astrodynamics::guidancelaw::effectivityprovider::QLaw;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameDirection;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

namespace
{

/// @brief Stub provider with configurable gate; score unused here.
class StubProvider : public EffectivityProvider
{
   public:
    explicit StubProvider(bool aGate)
        : EffectivityProvider("Stub Provider"),
          gate_(aGate)
    {
    }

    bool isEffective(const ostk::astrodynamics::trajectory::State&, const Vector3d&, const Real&) const override
    {
        return gate_;
    }

   private:
    bool gate_;
};

}  // namespace

class OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedGuidanceLaw : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const COE lowCOE = {
            Length::Meters(7000.0e3),
            0.01,
            Angle::Degrees(0.05),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
        };

        const COE::CartesianState cartesianState = lowCOE.getCartesianState(gravitationalParameter_, Frame::GCRF());
        positionLow_ = cartesianState.first.getCoordinates();
        velocityLow_ = cartesianState.second.getCoordinates();
    }

    Shared<const ConstantThrust> makeAlongTrackThrust() const
    {
        const LocalOrbitalFrameDirection direction({1.0, 0.0, 0.0}, LocalOrbitalFrameFactory::VNC(Frame::GCRF()));
        return std::make_shared<const ConstantThrust>(direction);
    }

    Shared<const QLaw> makeQLaw(const Real& absoluteThreshold, const Real& relativeThreshold) const
    {
        const QLaw::Parameters parameters = QLaw::Parameters(
            {
                {COE::Element::SemiMajorAxis, {1.0, 100.0}},
                {COE::Element::Eccentricity, {1.0, 1e-4}},
                {COE::Element::Inclination, {1.0, 1e-4}},
                {COE::Element::Raan, {1.0, 1e-4}},
                {COE::Element::Aop, {1.0, 1e-4}},
            },
            3,
            4,
            2,
            0.01,
            100,
            1.0,
            Length::Kilometers(6578.0),
            absoluteThreshold,
            relativeThreshold
        );

        const COE highTargetCOE = {
            Length::Meters(42000.0e3),
            0.01,
            Angle::Degrees(0.05),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
        };

        return std::make_shared<const QLaw>(
            highTargetCOE,
            gravitationalParameter_,
            parameters,
            QLaw::COEDomain::Osculating,
            QLaw::GradientStrategy::Analytical
        );
    }

    const Derived gravitationalParameter_ = Derived(398600.49 * 1e9, Derived::Unit::MeterCubedPerSecondSquared());

    Vector3d positionLow_ = Vector3d::Zero();
    Vector3d velocityLow_ = Vector3d::Zero();

    const Real thrustAcceleration_ = 1.0 / 300.0;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedGuidanceLaw, Constructor)
{
    const auto baseLaw = makeAlongTrackThrust();
    const auto provider = std::make_shared<StubProvider>(true);

    EXPECT_NO_THROW(EffectivityGatedGuidanceLaw(baseLaw, provider));
    EXPECT_THROW(
        EffectivityGatedGuidanceLaw(Shared<const GuidanceLaw>(nullptr), provider), ostk::core::error::runtime::Undefined
    );
    EXPECT_THROW(
        EffectivityGatedGuidanceLaw(baseLaw, Shared<EffectivityProvider>(nullptr)),
        ostk::core::error::runtime::Undefined
    );
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedGuidanceLaw, Getters)
{
    const auto baseLaw = makeAlongTrackThrust();
    const auto provider = std::make_shared<StubProvider>(true);
    const EffectivityGatedGuidanceLaw law {baseLaw, provider};

    EXPECT_EQ(law.accessBaseLaw(), baseLaw);
    EXPECT_EQ(law.accessProvider(), provider);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedGuidanceLaw, Print)
{
    const EffectivityGatedGuidanceLaw law {makeAlongTrackThrust(), std::make_shared<StubProvider>(true)};

    testing::internal::CaptureStdout();
    EXPECT_NO_THROW(std::cout << law << std::endl);
    EXPECT_NO_THROW(law.print(std::cout, false));
    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedGuidanceLaw, GatePassesPropagatesBaseAcceleration)
{
    const auto baseLaw = makeAlongTrackThrust();
    const auto provider = std::make_shared<StubProvider>(true);
    const EffectivityGatedGuidanceLaw law {baseLaw, provider};

    const Vector3d gated = law.calculateThrustAccelerationAt(
        Instant::J2000(), positionLow_, velocityLow_, thrustAcceleration_, Frame::GCRF()
    );
    const Vector3d ungated = baseLaw->calculateThrustAccelerationAt(
        Instant::J2000(), positionLow_, velocityLow_, thrustAcceleration_, Frame::GCRF()
    );

    for (int i = 0; i < 3; ++i)
    {
        EXPECT_NEAR(gated(i), ungated(i), 1e-12);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedGuidanceLaw, GateRejectsZeros)
{
    const auto baseLaw = makeAlongTrackThrust();
    const auto provider = std::make_shared<StubProvider>(false);
    const EffectivityGatedGuidanceLaw law {baseLaw, provider};

    const Vector3d gated = law.calculateThrustAccelerationAt(
        Instant::J2000(), positionLow_, velocityLow_, thrustAcceleration_, Frame::GCRF()
    );

    EXPECT_EQ(gated, Vector3d::Zero());
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedGuidanceLaw,
    MatchesEffectivityGatedConstantThrustWithQLawProvider
)
{
    // Pair: ConstantThrust(+along-track) with a QLawProvider gated at relativeThreshold = 0.0.
    // Should match the legacy EffectivityGatedConstantThrust under the same configuration.
    const auto baseLaw = makeAlongTrackThrust();
    const auto qlaw = makeQLaw(Real::Undefined(), Real(0.0));
    const auto provider = std::make_shared<QLawProvider>(qlaw);
    const EffectivityGatedGuidanceLaw law {baseLaw, provider};

    const Vector3d gated = law.calculateThrustAccelerationAt(
        Instant::J2000(), positionLow_, velocityLow_, thrustAcceleration_, Frame::GCRF()
    );

    EXPECT_NEAR(gated.norm(), thrustAcceleration_, 1e-12);
}
