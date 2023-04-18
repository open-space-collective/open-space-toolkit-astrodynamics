////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Messages/SpaceX/OPM.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Tabulated.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Integer ;
using ostk::core::types::Size ;
using ostk::core::types::Real ;
using ostk::core::types::String ;
using ostk::core::ctnr::Table ;
using ostk::core::ctnr::Array ;
using ostk::core::fs::Path ;
using ostk::core::fs::File ;

using ostk::math::obj::VectorXd ;

using ostk::physics::time::Instant ;
using ostk::physics::time::DateTime ;
using ostk::physics::time::Duration ;
using ostk::physics::time::Scale ;
using ostk::physics::coord::Position ;
using ostk::physics::coord::Velocity ;
using ostk::physics::coord::Frame ;

using ostk::astro::trajectory::State ;
using ostk::astro::trajectory::orbit::Model ;
using ostk::astro::trajectory::orbit::models::Tabulated ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Tabulated : public ::testing::Test {

    protected:

        void                    SetUp                                       ( ) override
        {

            const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Tabulated/propagated_states.csv")), Table::Format::CSV, true) ;

            referenceStates_.reserve(referenceData.getRowCount()) ;
            states_.reserve(referenceData.getRowCount() / testStatesCount_) ;

            Size t = 0 ;
            for (const auto& row : referenceData)
            {

                const Instant instant = Instant::DateTime(DateTime::Parse(row[0].accessString()), Scale::UTC) ;

                const Position position = Position::Meters({row[1].accessReal(), row[2].accessReal(), row[3].accessReal()}, Frame::GCRF()) ;

                const Velocity velocity = Velocity::MetersPerSecond({row[4].accessReal(), row[5].accessReal(), row[6].accessReal()}, Frame::GCRF()) ;

                State state = State(instant, position, velocity) ;

                referenceStates_.add(state) ;

                if ((t % testStatesCount_) == 0)
                {
                    states_.add(state) ;
                }

                ++t ;

            }

        }

        const Size testStatesCount_ = 20 ;
        Array<State> states_ = Array<State>::Empty() ;
        Array<State> referenceStates_ = Array<State>::Empty() ;

} ;

TEST_F (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Tabulated, Constructor)
{
    const Tabulated tabulated(states_, 0, Tabulated::InterpolationType::Linear) ;
}

TEST_F (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Tabulated, calculateStateAt_Linear)
{

    const Tabulated tabulated(states_, 0, Tabulated::InterpolationType::Linear) ;

    for (Size i = 0 ; i < referenceStates_.getSize() - testStatesCount_ ; ++i)
    {

        const Instant referenceInstant = referenceStates_[i].accessInstant() ;

        if (i == 0 || referenceInstant >= states_.accessLast().accessInstant())
        {
            continue ;
        }

        State state = tabulated.calculateStateAt(referenceInstant) ;

        EXPECT_TRUE(state.getCoordinates().isNear(referenceStates_[i].getCoordinates(), 1e-2)) << String::Format("Residual: {}", (state - referenceStates_[i]).getCoordinates().toString()) ;

    }

}

TEST_F (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Tabulated, calculateStateAt_BarycentricRational)
{

    const Tabulated tabulated(states_, 0, Tabulated::InterpolationType::BarycentricRational) ;

    for (Size i = 0 ; i < referenceStates_.getSize() ; ++i)
    {

        const Instant referenceInstant = referenceStates_[i].accessInstant() ;

        if (i == 0 || referenceInstant >= states_.accessLast().accessInstant())
        {
            continue ;
        }

        State state = tabulated.calculateStateAt(referenceInstant) ;

        EXPECT_TRUE(state.getCoordinates().isNear(referenceStates_[i].getCoordinates(), 1e-1)) << String::Format("Residual: {}", (state - referenceStates_[i]).getCoordinates().toString()) ;

    }

}

TEST_F (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Tabulated, calculateStateAt_CubicSpline)
{

    const Tabulated tabulated(states_, 0, Tabulated::InterpolationType::CubicSpline) ;

    for (Size i = 0 ; i < referenceStates_.getSize() ; ++i)
    {

        const Instant referenceInstant = referenceStates_[i].accessInstant() ;

        if (i == 0 || referenceInstant >= states_.accessLast().accessInstant())
        {
            continue ;
        }

        State state = tabulated.calculateStateAt(referenceInstant) ;

        EXPECT_TRUE(state.getCoordinates().isNear(referenceStates_[i].getCoordinates(), 1e-2)) << String::Format("Residual: {}", (state - referenceStates_[i]).getCoordinates().toString()) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
