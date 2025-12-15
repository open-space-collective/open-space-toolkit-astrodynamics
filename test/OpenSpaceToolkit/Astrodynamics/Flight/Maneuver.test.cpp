/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::Dynamics;
using TabulatedDynamics = ostk::astrodynamics::dynamics::Tabulated;
using ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameDirection;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianAcceleration;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_Flight_Maneuver : public ::testing::Test
{
   protected:
    State stateGenerator(
        const Instant& anInstant,
        const Vector3d& anAcceleration = {0.0, 0.0, 0.0},
        const Real& aMassFlowRate = -0.5e-5,
        const Vector3d& aPosition = {0.0, 0.0, 0.0},
        const Vector3d& aVelocity = {0.0, 0.0, 0.0},
        const Shared<const Frame>& aFrame = Frame::GCRF()
    )
    {
        VectorXd coordinates(10);
        coordinates << aPosition(0), aPosition(1), aPosition(2), aVelocity(0), aVelocity(1), aVelocity(2),
            anAcceleration(0), anAcceleration(1), anAcceleration(2), aMassFlowRate;

        return State(anInstant, coordinates, aFrame, Maneuver::RequiredCoordinateSubsets);
    }

    const Shared<const Frame> defaultFrameSPtr_ = Frame::GCRF();
    const Shared<const Frame> secondFrameSPtr_ = Frame::ITRF();

    const Real defaultConstantMassFlowRate_ = -0.5e-5;

    const Array<State> defaultStates_ = {
        stateGenerator(Instant::J2000(), {1e-1, 0.0, 0.0}, -1.0e-5),
        stateGenerator(Instant::J2000() + Duration::Seconds(1.0), {0.0, 1e-1, 0.0}, -1.1e-5),
        stateGenerator(Instant::J2000() + Duration::Seconds(5.0), {0.0, 0.0, 1e-1}, -0.9e-5),
        stateGenerator(Instant::J2000() + Duration::Seconds(7.0), {1e-1, 1e-1, 1e-1}, -1.0e-5),
    };

    const Maneuver defaultManeuver_ = {defaultStates_};
    const Maneuver singleStateManeuver_ = {{defaultStates_.accessFirst()}};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, Constructor)
{
    {
        EXPECT_NO_THROW(Maneuver {defaultStates_});
    }

    {
        const Array<State> statesWithoutAcceleration = {
            stateGenerator(Instant::J2000()),
            stateGenerator(Instant::J2000() + Duration::Seconds(1.0)),
            stateGenerator(Instant::J2000() + Duration::Seconds(5.0)),
            State(
                Instant::J2000() + Duration::Seconds(7.0),
                VectorXd::Zero(6),
                Frame::GCRF(),
                {CartesianPosition::Default(), CartesianVelocity::Default()}
            ),
        };

        EXPECT_THROW(
            {
                try
                {
                    Maneuver {statesWithoutAcceleration};
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("THRUST_ACCELERATION not found in states.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    Maneuver {{}};
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("No states provided.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    // Unsorted instant array
    {
        const Array<State> unorderedStates = {
            stateGenerator(Instant::J2000() + Duration::Seconds(1.0)),
            stateGenerator(Instant::J2000()),
            stateGenerator(Instant::J2000() + Duration::Seconds(7.0)),
            stateGenerator(Instant::J2000() + Duration::Seconds(5.0)),
        };

        EXPECT_THROW(
            {
                try
                {
                    Maneuver {unorderedStates};
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_EQ("{Unsorted or Duplicate State Array} is wrong.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );
    }

    // Only two states, but are duplicates
    {
        const Array<State> duplicateStates_ = {
            stateGenerator(Instant::J2000()),
            stateGenerator(Instant::J2000()),
        };

        EXPECT_THROW(
            {
                try
                {
                    Maneuver {duplicateStates_};
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_EQ("{Unsorted or Duplicate State Array} is wrong.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );
    }

    // State array with duplicates
    {
        const Array<State> duplicateStates_ = {
            stateGenerator(Instant::J2000()),
            stateGenerator(Instant::J2000() + Duration::Seconds(1.0)),
            stateGenerator(Instant::J2000() + Duration::Seconds(1.0)),
            stateGenerator(Instant::J2000() + Duration::Seconds(5.0)),
        };

        EXPECT_THROW(
            {
                try
                {
                    Maneuver {duplicateStates_};
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_EQ("{Unsorted or Duplicate State Array} is wrong.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );
    }

    // Maneuver with some intervals larger than the maximum recommended interpolation interval
    {
        const Array<State> spacedOutStates = {
            stateGenerator(Instant::J2000()),
            stateGenerator(Instant::J2000() + Maneuver::MaximumRecommendedInterpolationInterval * 2),
            stateGenerator(Instant::J2000() + Maneuver::MaximumRecommendedInterpolationInterval * 2.5),
            stateGenerator(Instant::J2000() + Maneuver::MaximumRecommendedInterpolationInterval * 3),
        };

        testing::internal::CaptureStdout();
        Maneuver {spacedOutStates};
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }

    // Maneuver with duration of less than minimum recommended duration
    {
        const Array<State> shortStates = {
            stateGenerator(Instant::J2000()),
            stateGenerator(Instant::J2000() + Maneuver::MinimumRecommendedDuration / 4),
            stateGenerator(Instant::J2000() + Maneuver::MinimumRecommendedDuration / 3),
            stateGenerator(Instant::J2000() + Maneuver::MinimumRecommendedDuration / 2),
        };

        testing::internal::CaptureStdout();
        Maneuver {shortStates};
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }

    // Mass flow rate profile with zero or positive mass flow rates
    {
        Array<State> statesWithIncorrectMassFlowRateProfile = {
            stateGenerator(Instant::J2000(), {0.0, 0.0, 1.0}, 1.0e-5),
            stateGenerator(Instant::J2000() + Duration::Seconds(1.0), {0.0, 0.0, 1.0}, -1.0e-5),
            stateGenerator(Instant::J2000() + Duration::Seconds(2.0), {0.0, 0.0, 1.0}, -1.0e-5),
            stateGenerator(Instant::J2000() + Duration::Seconds(3.0), {0.0, 0.0, 1.0}, -1.0e-5)
        };

        EXPECT_THROW(
            {
                try
                {
                    Maneuver {statesWithIncorrectMassFlowRateProfile};
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Mass flow rate profile must have strictly negative values.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, EqualToOperator)
{
    {
        EXPECT_TRUE(defaultManeuver_ == defaultManeuver_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, NotEqualToOperator)
{
    {
        EXPECT_FALSE(defaultManeuver_ != defaultManeuver_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, StreamOperator)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(std::cout << defaultManeuver_ << std::endl);

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(defaultManeuver_.print(std::cout, true));
    EXPECT_NO_THROW(defaultManeuver_.print(std::cout, false));

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, IsDefined)
{
    {
        EXPECT_TRUE(defaultManeuver_.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, Getters)
{
    {
        EXPECT_EQ(defaultManeuver_.getStates(), defaultStates_);
    }

    {
        EXPECT_EQ(
            defaultManeuver_.getInterval(),
            Interval::Closed(
                defaultStates_[0].accessInstant(), defaultStates_[defaultStates_.getSize() - 1].accessInstant()
            )
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, CalculateScalarQuantities)
{
    // Profile chosen to produce the same dV as equally spaced 1 m/s profiles

    const Array<State> evenlySpacedStates = {
        stateGenerator(Instant::J2000(), {0.5, 0.0, 0.0}, -0.5e-1),
        stateGenerator(Instant::J2000() + Duration::Minutes(1.0), {1.25, 0.0, 0.0}, -1.25e-1),
        stateGenerator(Instant::J2000() + Duration::Minutes(2.0), {1.25, 0.0, 0.0}, -1.25e-1),
        stateGenerator(Instant::J2000() + Duration::Minutes(3.0), {0.5, 0.0, 0.0}, -0.5e-1),
    };

    const Array<State> unevenlySpacedStates = {
        stateGenerator(Instant::J2000(), {0.5, 0.0, 0.0}, -0.5e-1),
        stateGenerator(
            Instant::J2000() + Duration::Minutes(0.5), {1.25, 0.0, 0.0}, -1.25e-1
        ),  // Larger gap in the middle
        stateGenerator(Instant::J2000() + Duration::Minutes(2.5), {1.25, 0.0, 0.0}, -1.25e-1),
        stateGenerator(Instant::J2000() + Duration::Minutes(3.0), {0.5, 0.0, 0.0}, -0.5e-1),
    };

    // Constant and non-constant mass flow rates chosen to produce the same dM as equally spaced 0.1 kg/s profiles
    const Real constantMassFlowRate = -1.0e-1;

    const Maneuver evenlySpacedManeuverWithConstantMassFlowRate =
        Maneuver::ConstantMassFlowRateProfile(evenlySpacedStates, constantMassFlowRate);

    const Maneuver unevenlySpacedManeuverWithConstantMassFlowRate =
        Maneuver::ConstantMassFlowRateProfile(unevenlySpacedStates, constantMassFlowRate);

    const Maneuver evenlySpacedManeuverWithNonConstantMassFlowRate = {evenlySpacedStates};

    const Maneuver unevenlySpacedManeuverWithNonConstantMassFlowRate = {unevenlySpacedStates};

    {
        // dV equivalent to 1.0 m/s * 180 s = 180 m/s
        EXPECT_DOUBLE_EQ(180.0, evenlySpacedManeuverWithConstantMassFlowRate.calculateDeltaV());
        // acceleration is higher for a greater portion of the burn, so total dV is higher
        EXPECT_DOUBLE_EQ(202.5, unevenlySpacedManeuverWithConstantMassFlowRate.calculateDeltaV());
        // dV equivalent to 1.0 m/s * 180 s = 180 m/s
        EXPECT_DOUBLE_EQ(180.0, evenlySpacedManeuverWithNonConstantMassFlowRate.calculateDeltaV());
        // acceleration is higher for a greater portion of the burn, so total dV is higher
        EXPECT_DOUBLE_EQ(202.5, unevenlySpacedManeuverWithNonConstantMassFlowRate.calculateDeltaV());
    }

    {
        // dM equivalent to 0.1 kg/s * 180 s = 18 kg
        EXPECT_DOUBLE_EQ(18.0, evenlySpacedManeuverWithConstantMassFlowRate.calculateDeltaMass().inKilograms());
        // dM equivalent to 0.1 kg/s * 180 s = 18 kg
        EXPECT_DOUBLE_EQ(18.0, unevenlySpacedManeuverWithConstantMassFlowRate.calculateDeltaMass().inKilograms());
        // dM equivalent to 0.1 kg/s * 180 s = 18 kg
        EXPECT_DOUBLE_EQ(18.0, evenlySpacedManeuverWithNonConstantMassFlowRate.calculateDeltaMass().inKilograms());
        // mass flow rate is higher for a greater portion of the burn, so total dM is higher
        EXPECT_DOUBLE_EQ(20.25, unevenlySpacedManeuverWithNonConstantMassFlowRate.calculateDeltaMass().inKilograms());
    }

    {
        EXPECT_DOUBLE_EQ(
            88.0,  // The average thrust is less than 100N because the spacecraft mass is decreasing during the burn
            evenlySpacedManeuverWithConstantMassFlowRate.calculateAverageThrust(Mass(100.0, Mass::Unit::Kilogram))
        );

        EXPECT_DOUBLE_EQ(
            96.9375,  // Higher than evenlySpacedManeuverWithConstantMassFlowRate because acceleration is higher for a
                      // greater portion of the burn
            unevenlySpacedManeuverWithConstantMassFlowRate.calculateAverageThrust(Mass(100.0, Mass ::Unit::Kilogram))
        );

        EXPECT_DOUBLE_EQ(
            87.90625,  // Very slightly lower than evenlySpacedManeuverWithConstantMassFlowRate's because non-constantly
                       // decreasing spacecraft mass
            evenlySpacedManeuverWithNonConstantMassFlowRate.calculateAverageThrust(Mass(100.0, Mass ::Unit::Kilogram))
        );

        EXPECT_DOUBLE_EQ(
            94.4765625,  // Very slightly lower than unevenlySpacedManeuverWithConstantMassFlowRate's because
                         // non-constantly decreasing spacecraft mass
            unevenlySpacedManeuverWithNonConstantMassFlowRate.calculateAverageThrust(Mass(100.0, Mass ::Unit::Kilogram))
        );
    }

    {
        EXPECT_DOUBLE_EQ(
            89.735026742057684,  // The specific impulse is less than 100s because the spacecraft mass is decreasing
                                 // during the burn
            evenlySpacedManeuverWithConstantMassFlowRate.calculateAverageSpecificImpulse(
                Mass(100.0, Mass::Unit::Kilogram)
            )
        );

        EXPECT_DOUBLE_EQ(
            98.848740395547921,  // Higher than evenlySpacedManeuverWithConstantMassFlowRate because acceleration is
                                 // higher for a greater portion of the burn
            unevenlySpacedManeuverWithConstantMassFlowRate.calculateAverageSpecificImpulse(
                Mass(100.0, Mass::Unit::Kilogram)
            )
        );

        EXPECT_DOUBLE_EQ(
            89.639428347090998,  // Very slightly lower than evenlySpacedManeuverWithConstantMassFlowRate's because
                                 // non-constantly decreasing
                                 // spacecraft mass
            evenlySpacedManeuverWithNonConstantMassFlowRate.calculateAverageSpecificImpulse(
                Mass(100.0, Mass::Unit::Kilogram)
            )
        );

        EXPECT_DOUBLE_EQ(
            85.634917802375611,  // Very slightly lower than unevenlySpacedManeuverWithConstantMassFlowRate's because
                                 // non-constantly decreasing
                                 // spacecraft mass
            unevenlySpacedManeuverWithNonConstantMassFlowRate.calculateAverageSpecificImpulse(
                Mass(100.0, Mass::Unit::Kilogram)
            )
        );
    }

    // Failure cases for single state maneuver
    {
        {
            EXPECT_THROW(
                {
                    try
                    {
                        singleStateManeuver_.calculateDeltaV();
                    }
                    catch (const ostk::core::error::RuntimeError& e)
                    {
                        EXPECT_EQ("At least two states are required to calculate the delta V.", e.getMessage());
                        throw;
                    }
                },
                ostk::core::error::RuntimeError
            );
        }

        {
            EXPECT_THROW(
                {
                    try
                    {
                        singleStateManeuver_.calculateDeltaMass();
                    }
                    catch (const ostk::core::error::RuntimeError& e)
                    {
                        EXPECT_EQ("At least two states are required to calculate the delta mass.", e.getMessage());
                        throw;
                    }
                },
                ostk::core::error::RuntimeError
            );
        }

        {
            EXPECT_THROW(
                {
                    try
                    {
                        singleStateManeuver_.calculateAverageThrust(Mass(100.0, Mass::Unit::Kilogram));
                    }
                    catch (const ostk::core::error::RuntimeError& e)
                    {
                        EXPECT_EQ("At least two states are required to calculate the average thrust.", e.getMessage());
                        throw;
                    }
                },
                ostk::core::error::RuntimeError
            );
        }

        {
            EXPECT_THROW(
                {
                    try
                    {
                        singleStateManeuver_.calculateAverageSpecificImpulse(Mass(100.0, Mass::Unit::Kilogram));
                    }
                    catch (const ostk::core::error::RuntimeError& e)
                    {
                        EXPECT_EQ(
                            "At least two states are required to calculate the average specific impulse.",
                            e.getMessage()
                        );
                        throw;
                    }
                },
                ostk::core::error::RuntimeError
            );
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, ToTabulatedDynamics)
{
    {
        const Shared<TabulatedDynamics> tabulatedDynamicsSPtr =
            defaultManeuver_.toTabulatedDynamics(defaultFrameSPtr_, Interpolator::Type::BarycentricRational);

        const Array<Instant> instants = defaultStates_.map<Instant>(
            [](const auto& state)
            {
                return state.accessInstant();
            }
        );

        EXPECT_EQ(tabulatedDynamicsSPtr->accessInstants(), instants);

        const MatrixXd contributionProfile = tabulatedDynamicsSPtr->accessContributionProfile();

        EXPECT_EQ(contributionProfile.rows(), 4);
        EXPECT_EQ(contributionProfile.cols(), defaultStates_.getSize());

        const Array<Vector3d> accelerationProfile = defaultStates_.map<Vector3d>(
            [this](const State& aState) -> Vector3d
            {
                return Vector3d::Map(aState.inFrame(defaultFrameSPtr_)
                                         .extractCoordinate(CartesianAcceleration::ThrustAcceleration())
                                         .data());
            }
        );
        const Array<Real> massFlowRateProfile = defaultStates_.map<Real>(
            [](const State& aState) -> Real
            {
                return aState.extractCoordinate(CoordinateSubset::MassFlowRate())[0];
            }
        );

        for (Size i = 0; i < defaultStates_.getSize(); i++)
        {
            EXPECT_NEAR(contributionProfile(i, 0), accelerationProfile[i](0), 1.0e-15);
            EXPECT_NEAR(contributionProfile(i, 1), accelerationProfile[i](1), 1.0e-15);
            EXPECT_NEAR(contributionProfile(i, 2), accelerationProfile[i](2), 1.0e-15);
            EXPECT_NEAR(contributionProfile(i, 3), massFlowRateProfile[i], 1.0e-15);
        }

        const Array<Shared<const CoordinateSubset>> writeCoordinateSubsets = {
            CartesianVelocity::Default(), CoordinateSubset::Mass()
        };
        EXPECT_EQ(tabulatedDynamicsSPtr->getWriteCoordinateSubsets(), writeCoordinateSubsets);
        EXPECT_EQ(tabulatedDynamicsSPtr->accessFrame(), defaultFrameSPtr_);
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    singleStateManeuver_.toTabulatedDynamics(
                        defaultFrameSPtr_, Interpolator::Type::BarycentricRational
                    );
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(
                        "At least two states are required to convert a maneuver to tabulated dynamics.", e.getMessage()
                    );
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, calculateMeanThrustDirectionAndMaximumAngularOffset)
{
    const Shared<const Frame> inertialFrameSPtr = Frame::GCRF();
    const Shared<const LocalOrbitalFrameFactory> localOrbitalFrameFactorySPtr =
        LocalOrbitalFrameFactory::TNW(inertialFrameSPtr);

    {
        const Maneuver::MeanDirectionAndMaximumAngularOffset meanDirectionAndMaximumAngularOffset =
            defaultManeuver_.calculateMeanThrustDirectionAndMaximumAngularOffset(localOrbitalFrameFactorySPtr);

        const LocalOrbitalFrameDirection meanDirection = meanDirectionAndMaximumAngularOffset.first;
        EXPECT_TRUE(meanDirection.isDefined());
        EXPECT_EQ(meanDirection.getLocalOrbitalFrameFactory(), localOrbitalFrameFactorySPtr);

        const Angle maximumAngularOffset = meanDirectionAndMaximumAngularOffset.second;
        EXPECT_TRUE(maximumAngularOffset.isDefined());
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    defaultManeuver_.calculateMeanThrustDirectionAndMaximumAngularOffset(nullptr);
                }
                catch (const ostk::core::error::runtime::Undefined& e)
                {
                    EXPECT_EQ("{Local Orbital Frame Factory} is undefined.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    defaultManeuver_.calculateMeanThrustDirectionAndMaximumAngularOffset(
                        LocalOrbitalFrameFactory::Undefined()
                    );
                }
                catch (const ostk::core::error::runtime::Undefined& e)
                {
                    EXPECT_EQ("{Local Orbital Frame Factory} is undefined.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        const Array<State> statesWithZeroThrustAcceleration = {
            stateGenerator(Instant::J2000() + Duration::Seconds(1.0), {0.0, 0.0, 0.0}, -1.0e-5),
            stateGenerator(Instant::J2000() + Duration::Seconds(5.0), {0.0, 0.0, 0.0}, -1.0e-5),
        };
        const Maneuver maneuverWithZeroThrustAcceleration = {statesWithZeroThrustAcceleration};

        EXPECT_THROW(
            maneuverWithZeroThrustAcceleration.calculateMeanThrustDirectionAndMaximumAngularOffset(
                localOrbitalFrameFactorySPtr
            ),
            ostk::core::error::RuntimeError
        );
    }

    {
        // Acceleration direction is constant in LoF frame ({2, -1, 3}, in TNW), with different magnitudes at each
        // state: 0.8 and 1.5.
        const Array<State> originalStates = {
            stateGenerator(
                Instant::J2000() + Duration::Seconds(0.0),
                {0.8 * 1.0, 0.8 * 2.0, 0.8 * 3.0},  // 0.8 * {2, -1, 3} in TNW
                -1.0e-5,
                {7.0e6, 0.0, 0.0},
                {0.0, 8.0e3, 0.0},
                Frame::GCRF()
            ),
            stateGenerator(
                Instant::J2000() + Duration::Seconds(1.0),
                {1.5 * -2.0, 1.5 * 1.0, 1.5 * 3.0},  // 1.5 * {2, -1, 3} in TNW
                -1.0e-5,
                {0.0, 7.0e6, 0.0},
                {-8.0e3, 0.0, 0.0},
                Frame::GCRF()
            ),
        };

        const Maneuver originalManeuver = {originalStates};
        const Maneuver::MeanDirectionAndMaximumAngularOffset meanDirectionAndMaximumAngularOffset =
            originalManeuver.calculateMeanThrustDirectionAndMaximumAngularOffset(localOrbitalFrameFactorySPtr);

        const LocalOrbitalFrameDirection meanDirection = meanDirectionAndMaximumAngularOffset.first;
        EXPECT_TRUE(meanDirection.isDefined());
        EXPECT_EQ(meanDirection.getLocalOrbitalFrameFactory(), localOrbitalFrameFactorySPtr);
        EXPECT_TRUE(meanDirection.getValue().isApprox(Vector3d(2.0, -1.0, 3.0).normalized(), 1e-12));

        const Angle maximumAngularOffset = meanDirectionAndMaximumAngularOffset.second;
        EXPECT_TRUE(maximumAngularOffset.isDefined());
        EXPECT_DOUBLE_EQ(maximumAngularOffset.inDegrees(0.0, 360.0), 0.0);
    }

    {
        // Acceleration direction is not constant in TNW frame, but magnitudes are.
        const Array<State> originalStates = {
            stateGenerator(
                Instant::J2000() + Duration::Seconds(0.0),
                Vector3d {1.0, 2.0, 3.0},  // {2.0, -1.0, 3.0} in TNW
                -1.0e-5,
                {7.0e6, 0.0, 0.0},
                {0.0, 8.0e3, 0.0},
                Frame::GCRF()
            ),
            stateGenerator(
                Instant::J2000() + Duration::Seconds(1.0),
                Vector3d {-3.0, -1.0, -2.0},  // {3.0, 1.0, -2.0} in TNW
                -1.0e-5,
                {0.0, 7.0e6, 0.0},
                {-8.0e3, 0.0, 0.0},
                Frame::GCRF()
            ),
        };

        const Maneuver originalManeuver = {originalStates};

        const Maneuver::MeanDirectionAndMaximumAngularOffset meanDirectionAndMaximumAngularOffset =
            originalManeuver.calculateMeanThrustDirectionAndMaximumAngularOffset(localOrbitalFrameFactorySPtr);

        const LocalOrbitalFrameDirection meanDirection = meanDirectionAndMaximumAngularOffset.first;
        EXPECT_TRUE(meanDirection.isDefined());
        EXPECT_EQ(meanDirection.getLocalOrbitalFrameFactory(), localOrbitalFrameFactorySPtr);
        EXPECT_TRUE(meanDirection.getValue().isApprox(Vector3d(2.5, 0.0, 0.5).normalized(), 1e-12));

        const Angle maximumAngularOffset = meanDirectionAndMaximumAngularOffset.second;
        EXPECT_TRUE(maximumAngularOffset.isDefined());
        EXPECT_DOUBLE_EQ(
            maximumAngularOffset.inDegrees(0.0, 360.0),
            std::max(
                Angle::Between(meanDirection.getValue(), Vector3d(2.0, -1.0, 3.0)).inDegrees(0.0, 360.0),
                Angle::Between(meanDirection.getValue(), Vector3d(3.0, 1.0, -2.0)).inDegrees(0.0, 360.0)
            )
        );
    }

    {
        // Neither direction nor magnitudes are constant.
        const Array<State> originalStates = {
            stateGenerator(
                Instant::J2000() + Duration::Seconds(0.0),
                Vector3d {2.0, 4.0, 6.0},  // {4.0, -2.0, 6.0} in TNW
                -1.0e-5,
                {7.0e6, 0.0, 0.0},
                {0.0, 8.0e3, 0.0},
                Frame::GCRF()
            ),
            stateGenerator(
                Instant::J2000() + Duration::Seconds(1.0),
                Vector3d {-3.0, -1.0, -2.0},  // {3.0, 1.0, -2.0} in TNW
                -1.0e-5,
                {0.0, 7.0e6, 0.0},
                {-8.0e3, 0.0, 0.0},
                Frame::GCRF()
            ),
        };

        const Maneuver originalManeuver = {originalStates};

        const Maneuver::MeanDirectionAndMaximumAngularOffset meanDirectionAndMaximumAngularOffset =
            originalManeuver.calculateMeanThrustDirectionAndMaximumAngularOffset(localOrbitalFrameFactorySPtr);

        const LocalOrbitalFrameDirection meanDirection = meanDirectionAndMaximumAngularOffset.first;
        EXPECT_TRUE(meanDirection.isDefined());
        EXPECT_EQ(meanDirection.getLocalOrbitalFrameFactory(), localOrbitalFrameFactorySPtr);
        EXPECT_TRUE(meanDirection.getValue().isApprox(Vector3d(3.5, -0.5, 2.0).normalized(), 1e-12));

        const Angle maximumAngularOffset = meanDirectionAndMaximumAngularOffset.second;
        EXPECT_TRUE(maximumAngularOffset.isDefined());
        EXPECT_DOUBLE_EQ(
            maximumAngularOffset.inDegrees(0.0, 360.0),
            std::max(
                Angle::Between(meanDirection.getValue(), Vector3d(4.0, -2.0, 6.0)).inDegrees(0.0, 360.0),
                Angle::Between(meanDirection.getValue(), Vector3d(3.0, 1.0, -2.0)).inDegrees(0.0, 360.0)
            )
        );
    }

    {
        // Single state maneuver
        const Array<State> originalStates = {
            stateGenerator(
                Instant::J2000() + Duration::Seconds(0.0),
                {0.8 * 1.0, 0.8 * 2.0, 0.8 * 3.0},  // 0.8 * {2, -1, 3} in TNW
                -1.0e-5,
                {7.0e6, 0.0, 0.0},
                {0.0, 8.0e3, 0.0},
                Frame::GCRF()
            ),
        };

        const Maneuver originalManeuver = {originalStates};
        const Maneuver::MeanDirectionAndMaximumAngularOffset meanDirectionAndMaximumAngularOffset =
            originalManeuver.calculateMeanThrustDirectionAndMaximumAngularOffset(localOrbitalFrameFactorySPtr);

        const LocalOrbitalFrameDirection meanDirection = meanDirectionAndMaximumAngularOffset.first;
        EXPECT_TRUE(meanDirection.isDefined());
        EXPECT_EQ(meanDirection.getLocalOrbitalFrameFactory(), localOrbitalFrameFactorySPtr);
        EXPECT_TRUE(meanDirection.getValue().isApprox(Vector3d(2.0, -1.0, 3.0).normalized(), 1e-12));

        const Angle maximumAngularOffset = meanDirectionAndMaximumAngularOffset.second;
        EXPECT_TRUE(maximumAngularOffset.isDefined());
        EXPECT_DOUBLE_EQ(maximumAngularOffset.inDegrees(0.0, 360.0), 0.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, toConstantLocalOrbitalFrameDirectionManeuver)
{
    const Shared<const Frame> inertialFrameSPtr = Frame::GCRF();
    const Shared<const LocalOrbitalFrameFactory> localOrbitalFrameFactorySPtr =
        LocalOrbitalFrameFactory::TNW(inertialFrameSPtr);

    // Ignore maximum angular offset
    {
        const Maneuver newManeuver =
            defaultManeuver_.toConstantLocalOrbitalFrameDirectionManeuver(localOrbitalFrameFactorySPtr);

        EXPECT_TRUE(newManeuver.isDefined());
        EXPECT_EQ(newManeuver.getStates().getSize(), defaultStates_.getSize());
        EXPECT_EQ(newManeuver.getInterval(), defaultManeuver_.getInterval());

        for (Size i = 0; i < newManeuver.getStates().getSize(); i++)
        {
            const State originalState = defaultManeuver_.getStates()[i];
            const State newState = newManeuver.getStates()[i];

            EXPECT_EQ(originalState.getInstant(), newState.getInstant());
            EXPECT_EQ(originalState.getFrame(), newState.getFrame());
            EXPECT_EQ(originalState.getPosition(), newState.getPosition());
            EXPECT_EQ(originalState.getVelocity(), newState.getVelocity());
        }
    }

    // Consider maximum allowed angular offset, but it's not violated
    {
        const Maneuver::MeanDirectionAndMaximumAngularOffset meanDirectionAndMaximumAngularOffset =
            defaultManeuver_.calculateMeanThrustDirectionAndMaximumAngularOffset(localOrbitalFrameFactorySPtr);

        const Maneuver newManeuver = defaultManeuver_.toConstantLocalOrbitalFrameDirectionManeuver(
            localOrbitalFrameFactorySPtr,
            Angle::Degrees(1.1 * meanDirectionAndMaximumAngularOffset.second.inDegrees(0.0, 360.0))
        );

        EXPECT_TRUE(newManeuver.isDefined());
        EXPECT_EQ(newManeuver.getStates().getSize(), defaultStates_.getSize());
        EXPECT_EQ(newManeuver.getInterval(), defaultManeuver_.getInterval());
    }

    // Consider maximum allowed angular offset, and it's violated
    {
        const Maneuver::MeanDirectionAndMaximumAngularOffset meanDirectionAndMaximumAngularOffset =
            defaultManeuver_.calculateMeanThrustDirectionAndMaximumAngularOffset(localOrbitalFrameFactorySPtr);

        EXPECT_THROW(
            try {
                defaultManeuver_.toConstantLocalOrbitalFrameDirectionManeuver(
                    localOrbitalFrameFactorySPtr,
                    Angle::Degrees(0.9 * meanDirectionAndMaximumAngularOffset.second.inDegrees(0.0, 360.0))
                );
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_NE(e.getMessage().find("Maximum angular offset"), std::string::npos);
                EXPECT_NE(e.getMessage().find(" is greater than the maximum allowed ("), std::string::npos);
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    // Local Orbital Frame Factory is undefined
    {
        EXPECT_THROW(
            defaultManeuver_.toConstantLocalOrbitalFrameDirectionManeuver(LocalOrbitalFrameFactory::Undefined()),
            ostk::core::error::runtime::Undefined
        );
    }

    // Thrust acceleration direction is too small
    {
        const Array<State> statesWithZeroThrustAcceleration = {
            stateGenerator(Instant::J2000() + Duration::Seconds(1.0), {0.0, 0.0, 0.0}, -1.0e-5),
            stateGenerator(Instant::J2000() + Duration::Seconds(5.0), {0.0, 0.0, 0.0}, -1.0e-5),
        };

        const Maneuver maneuverWithZeroThrustAcceleration = {statesWithZeroThrustAcceleration};
        EXPECT_THROW(
            maneuverWithZeroThrustAcceleration.toConstantLocalOrbitalFrameDirectionManeuver(localOrbitalFrameFactorySPtr
            ),
            ostk::core::error::RuntimeError
        );
    }

    // Acceleration direction is constant in LoF frame ({2, -1, 3}, in TNW), with different magnitudes at each
    // state: 0.8 and 1.5.
    {
        const Array<State> originalStates = {
            stateGenerator(
                Instant::J2000() + Duration::Seconds(0.0),
                {0.8 * 1.0, 0.8 * 2.0, 0.8 * 3.0},  // 0.8 * {2, -1, 3} in TNW
                -1.0e-5,
                {7.0e6, 0.0, 0.0},
                {0.0, 8.0e3, 0.0},
                Frame::GCRF()
            ),
            stateGenerator(
                Instant::J2000() + Duration::Seconds(1.0),
                {1.5 * -2.0, 1.5 * 1.0, 1.5 * 3.0},  // 1.5 * {2, -1, 3} in TNW
                -1.0e-5,
                {0.0, 7.0e6, 0.0},
                {-8.0e3, 0.0, 0.0},
                Frame::GCRF()
            ),
        };

        const Maneuver originalManeuver = {originalStates};
        const Maneuver newManeuver =
            originalManeuver.toConstantLocalOrbitalFrameDirectionManeuver(localOrbitalFrameFactorySPtr);
        const Array<State> newStates = newManeuver.getStates();

        const Vector3d expectedThrustAccelerationInLof = {2.0, -1.0, 3.0};

        const Vector3d state0ThrustAccelerationInLof =
            newStates[0]
                .accessFrame()
                ->getTransformTo(
                    localOrbitalFrameFactorySPtr->generateFrame(newStates[0]), newStates[0].accessInstant()
                )
                .applyToVector(newStates[0].extractCoordinate(CartesianAcceleration::ThrustAcceleration()));
        const Vector3d state1ThrustAccelerationInLof =
            newStates[1]
                .accessFrame()
                ->getTransformTo(
                    localOrbitalFrameFactorySPtr->generateFrame(newStates[1]), newStates[1].accessInstant()
                )
                .applyToVector(newStates[1].extractCoordinate(CartesianAcceleration::ThrustAcceleration()));

        EXPECT_TRUE(state0ThrustAccelerationInLof.isApprox(0.8 * expectedThrustAccelerationInLof, 1e-12));
        EXPECT_TRUE(state1ThrustAccelerationInLof.isApprox(1.5 * expectedThrustAccelerationInLof, 1e-12));
    }

    // Acceleration direction is not constant in TNW frame, but magnitudes are.
    {
        const Array<State> originalStates = {
            stateGenerator(
                Instant::J2000() + Duration::Seconds(0.0),
                Vector3d {1.0, 2.0, 3.0},  // {2.0, -1.0, 3.0} in TNW
                -1.0e-5,
                {7.0e6, 0.0, 0.0},
                {0.0, 8.0e3, 0.0},
                Frame::GCRF()
            ),
            stateGenerator(
                Instant::J2000() + Duration::Seconds(1.0),
                Vector3d {-3.0, -1.0, -2.0},  // {3.0, 1.0, -2.0} in TNW
                -1.0e-5,
                {0.0, 7.0e6, 0.0},
                {-8.0e3, 0.0, 0.0},
                Frame::GCRF()
            ),
        };

        const Maneuver originalManeuver = {originalStates};
        const Maneuver newManeuver =
            originalManeuver.toConstantLocalOrbitalFrameDirectionManeuver(localOrbitalFrameFactorySPtr);
        const Array<State> newStates = newManeuver.getStates();

        const Vector3d expectedThrustAccelerationInLof =
            Vector3d {2.5, 0.0, 0.5}.normalized() * Vector3d {1.0, 2.0, 3.0}.norm();

        const Vector3d state0ThrustAccelerationInLof =
            newStates[0]
                .accessFrame()
                ->getTransformTo(
                    localOrbitalFrameFactorySPtr->generateFrame(newStates[0]), newStates[0].accessInstant()
                )
                .applyToVector(newStates[0].extractCoordinate(CartesianAcceleration::ThrustAcceleration()));
        const Vector3d state1ThrustAccelerationInLof =
            newStates[1]
                .accessFrame()
                ->getTransformTo(
                    localOrbitalFrameFactorySPtr->generateFrame(newStates[1]), newStates[1].accessInstant()
                )
                .applyToVector(newStates[1].extractCoordinate(CartesianAcceleration::ThrustAcceleration()));

        EXPECT_TRUE(state0ThrustAccelerationInLof.isApprox(expectedThrustAccelerationInLof, 1e-12));
        EXPECT_TRUE(state1ThrustAccelerationInLof.isApprox(expectedThrustAccelerationInLof, 1e-12));
    }

    // Single state maneuver
    {
        const Array<State> originalStates = {
            stateGenerator(
                Instant::J2000() + Duration::Seconds(0.0),
                {0.8 * 1.0, 0.8 * 2.0, 0.8 * 3.0},  // 0.8 * {2, -1, 3} in TNW
                -1.0e-5,
                {7.0e6, 0.0, 0.0},
                {0.0, 8.0e3, 0.0},
                Frame::GCRF()
            ),
        };

        const Maneuver originalManeuver = {originalStates};
        const Maneuver newManeuver =
            originalManeuver.toConstantLocalOrbitalFrameDirectionManeuver(localOrbitalFrameFactorySPtr);
        const Array<State> newStates = newManeuver.getStates();

        const Vector3d expectedThrustAccelerationInLof = {2.0, -1.0, 3.0};

        const Vector3d state0ThrustAccelerationInLof =
            newStates[0]
                .accessFrame()
                ->getTransformTo(
                    localOrbitalFrameFactorySPtr->generateFrame(newStates[0]), newStates[0].accessInstant()
                )
                .applyToVector(newStates[0].extractCoordinate(CartesianAcceleration::ThrustAcceleration()));

        std::cout << "Expected thrust acceleration in LOF: " << expectedThrustAccelerationInLof.transpose()
                  << std::endl;
        std::cout << "Actual thrust acceleration in LOF:   " << state0ThrustAccelerationInLof.transpose() << std::endl;
        EXPECT_TRUE(state0ThrustAccelerationInLof.isApprox(0.8 * expectedThrustAccelerationInLof, 1e-12));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, ConstantMassFlowRateProfile)
{
    {
        const Maneuver maneuver = Maneuver::ConstantMassFlowRateProfile(defaultStates_, defaultConstantMassFlowRate_);

        EXPECT_EQ(maneuver.getStates().getSize(), defaultStates_.getSize());

        for (const auto& state : maneuver.getStates())
        {
            EXPECT_DOUBLE_EQ(
                state.extractCoordinate(CoordinateSubset::MassFlowRate())[0], defaultConstantMassFlowRate_
            );
        }
    }

    // Single state maneuver
    {
        const Maneuver maneuver =
            Maneuver::ConstantMassFlowRateProfile({defaultStates_.accessFirst()}, defaultConstantMassFlowRate_);

        EXPECT_EQ(maneuver.getStates().getSize(), 1);

        EXPECT_DOUBLE_EQ(
            maneuver.getStates()[0].extractCoordinate(CoordinateSubset::MassFlowRate())[0], defaultConstantMassFlowRate_
        );
    }
}
