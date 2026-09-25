/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Index;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::Environment;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::Instant;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

class DynamicsMock : public Dynamics
{
   public:
    DynamicsMock(const String& aName)
        : Dynamics(aName) {};

    MOCK_METHOD(bool, isDefined, (), (const, override));

    MOCK_METHOD(Array<Shared<const CoordinateSubset>>, getReadCoordinateSubsets, (), (const, override));

    MOCK_METHOD(Array<Shared<const CoordinateSubset>>, getWriteCoordinateSubsets, (), (const, override));

    MOCK_METHOD(
        VectorXd,
        computeContribution,
        (const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr),
        (const, override)
    );
};

class OpenSpaceToolkit_Astrodynamics_Dynamics : public ::testing::Test
{
   protected:
    /// @brief Build a dynamics context, registering the dynamics' coordinate subsets into the provided broker
    ///        (mirrors what Propagator::addDynamics does)
    static Dynamics::Context BuildContext(
        const Shared<Dynamics>& aDynamicsSPtr, const Shared<CoordinateBroker>& aCoordinateBrokerSPtr
    )
    {
        Array<Pair<Index, Size>> readInfo = Array<Pair<Index, Size>>::Empty();
        for (const Shared<const CoordinateSubset>& subset : aDynamicsSPtr->getReadCoordinateSubsets())
        {
            const Pair<Index, Size> indexAndSize = {aCoordinateBrokerSPtr->addSubset(subset), subset->getSize()};
            readInfo.add(indexAndSize);
        }

        Array<Pair<Index, Size>> writeInfo = Array<Pair<Index, Size>>::Empty();
        for (const Shared<const CoordinateSubset>& subset : aDynamicsSPtr->getWriteCoordinateSubsets())
        {
            const Pair<Index, Size> indexAndSize = {aCoordinateBrokerSPtr->addSubset(subset), subset->getSize()};
            writeInfo.add(indexAndSize);
        }

        return {aDynamicsSPtr, readInfo, writeInfo};
    }

    const String defaultName_ = "Test";
    const DynamicsMock defaultDynamics_ = {defaultName_};

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();
    const Instant defaultInstant_ = Instant::J2000();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultDynamics_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultDynamics_.print(std::cout, true));
        EXPECT_NO_THROW(defaultDynamics_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics, Getters)
{
    {
        EXPECT_EQ(defaultName_, defaultDynamics_.getName());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics, ComputeContributions)
{
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(Earth::Spherical());

    const Shared<Dynamics> positionDerivativeSPtr = std::make_shared<PositionDerivative>();
    const Shared<Dynamics> centralBodyGravitySPtr = std::make_shared<CentralBodyGravity>(earthSPtr);

    const Shared<CoordinateBroker> coordinateBrokerSPtr = std::make_shared<CoordinateBroker>();

    const Array<Dynamics::Context> contexts = {
        BuildContext(positionDerivativeSPtr, coordinateBrokerSPtr),
        BuildContext(centralBodyGravitySPtr, coordinateBrokerSPtr),
    };

    const Index positionIndex = coordinateBrokerSPtr->getSubsetIndex(CartesianPosition::Default());
    const Index velocityIndex = coordinateBrokerSPtr->getSubsetIndex(CartesianVelocity::Default());
    const Size stateSize = coordinateBrokerSPtr->getNumberOfCoordinates();

    VectorXd position(3);
    position << 7000000.0, 0.0, 0.0;

    VectorXd velocity(3);
    velocity << 0.0, 7546.05, 0.0;

    NumericalSolver::StateVector stateVector = NumericalSolver::StateVector::Zero(stateSize);
    stateVector.segment(positionIndex, 3) = position;
    stateVector.segment(velocityIndex, 3) = velocity;

    const Array<VectorXd> contributions =
        Dynamics::ComputeContributions(contexts, defaultInstant_, stateVector, gcrfSPtr_);

    // Each contribution is exactly what the dynamics returns when called with the manually reduced state
    {
        ASSERT_EQ(2, contributions.getSize());

        const VectorXd expectedPositionDerivativeContribution =
            positionDerivativeSPtr->computeContribution(defaultInstant_, velocity, gcrfSPtr_);
        const VectorXd expectedCentralBodyGravityContribution =
            centralBodyGravitySPtr->computeContribution(defaultInstant_, position, gcrfSPtr_);

        ASSERT_EQ(expectedPositionDerivativeContribution.size(), contributions[0].size());
        ASSERT_EQ(expectedCentralBodyGravityContribution.size(), contributions[1].size());

        for (Index i = 0; i < 3; ++i)
        {
            EXPECT_EQ(expectedPositionDerivativeContribution(i), contributions[0](i));
            EXPECT_EQ(expectedCentralBodyGravityContribution(i), contributions[1](i));
        }
    }

    // Scatter-adding the contributions reproduces the summed system of equations
    {
        NumericalSolver::StateVector dxdt = NumericalSolver::StateVector::Zero(stateSize);

        Dynamics::GetSystemOfEquations(contexts, defaultInstant_, gcrfSPtr_)(stateVector, dxdt, 0.0);

        NumericalSolver::StateVector scatteredContributions = NumericalSolver::StateVector::Zero(stateSize);

        for (Index contextIndex = 0; contextIndex < contexts.getSize(); ++contextIndex)
        {
            Index offset = 0;

            for (const Pair<Index, Size>& writeInfo : contexts[contextIndex].writeIndexes)
            {
                scatteredContributions.segment(writeInfo.first, writeInfo.second) +=
                    contributions[contextIndex].segment(offset, writeInfo.second);
                offset += writeInfo.second;
            }
        }

        for (Index i = 0; i < stateSize; ++i)
        {
            EXPECT_EQ(dxdt(i), scatteredContributions(i));
        }
    }

    // Empty context array
    {
        EXPECT_TRUE(
            Dynamics::ComputeContributions(Array<Dynamics::Context>::Empty(), defaultInstant_, stateVector, gcrfSPtr_)
                .isEmpty()
        );
    }

    // Dynamics declaring no read coordinate subsets
    {
        const Shared<DynamicsMock> dynamicsMockSPtr = std::make_shared<DynamicsMock>("Read Nothing");
        const Shared<const CoordinateSubset> customSubsetSPtr = std::make_shared<CoordinateSubset>("Custom", 1);

        VectorXd mockContribution(1);
        mockContribution << 42.0;

        EXPECT_CALL(*dynamicsMockSPtr, getReadCoordinateSubsets())
            .WillRepeatedly(::testing::Return(Array<Shared<const CoordinateSubset>>::Empty()));
        EXPECT_CALL(*dynamicsMockSPtr, getWriteCoordinateSubsets())
            .WillRepeatedly(::testing::Return(Array<Shared<const CoordinateSubset>> {customSubsetSPtr}));
        EXPECT_CALL(*dynamicsMockSPtr, computeContribution(::testing::_, ::testing::_, ::testing::_))
            .WillRepeatedly(::testing::Return(mockContribution));

        const Shared<CoordinateBroker> mockCoordinateBrokerSPtr = std::make_shared<CoordinateBroker>();
        const Array<Dynamics::Context> mockContexts = {BuildContext(dynamicsMockSPtr, mockCoordinateBrokerSPtr)};

        const NumericalSolver::StateVector mockStateVector =
            NumericalSolver::StateVector::Zero(mockCoordinateBrokerSPtr->getNumberOfCoordinates());

        const Array<VectorXd> mockContributions =
            Dynamics::ComputeContributions(mockContexts, defaultInstant_, mockStateVector, gcrfSPtr_);

        ASSERT_EQ(1, mockContributions.getSize());
        ASSERT_EQ(1, mockContributions[0].size());
        EXPECT_EQ(42.0, mockContributions[0](0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics, FromEnvironment)
{
    {
        EXPECT_NO_THROW(Dynamics::FromEnvironment(Environment::Default()));
    }
}
