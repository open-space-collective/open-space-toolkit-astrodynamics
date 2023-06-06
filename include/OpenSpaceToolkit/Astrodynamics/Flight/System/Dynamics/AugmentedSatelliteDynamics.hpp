// /// Apache License 2.0

// #ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AugmentedSatelliteDynamics__
// #define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AugmentedSatelliteDynamics__

// #include <OpenSpaceToolkit/Core/Containers/Array.hpp>
// #include <OpenSpaceToolkit/Core/Types/Integer.hpp>
// #include <OpenSpaceToolkit/Core/Types/Real.hpp>
// #include <OpenSpaceToolkit/Core/Types/Shared.hpp>
// #include <OpenSpaceToolkit/Core/Types/String.hpp>

// #include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

// #include <OpenSpaceToolkit/Physics/Data/Vector.hpp>
// #include <OpenSpaceToolkit/Physics/Environment.hpp>
// #include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
// #include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
// #include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
// #include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
// #include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
// #include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
// #include <OpenSpaceToolkit/Physics/Units/Length.hpp>
// #include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

// #include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
// #include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
// #include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
// #include <OpenSpaceToolkit/Astrodynamics/Trajectory/Force.hpp>
// #include <OpenSpaceToolkit/Astrodynamics/Trajectory/Forces/GravityForce.hpp>

// namespace ostk
// {
// namespace astro
// {
// namespace flight
// {
// namespace system
// {
// namespace dynamics
// {

// using ostk::core::ctnr::Array;
// using ostk::core::types::Integer;
// using ostk::core::types::Real;
// using ostk::core::types::Shared;
// using ostk::core::types::String;

// using ostk::math::obj::Vector3d;

// using ostk::physics::Environment;
// using ostk::physics::coord::Frame;
// using ostk::physics::coord::Position;
// using ostk::physics::coord::Velocity;
// using ostk::physics::data::Vector;
// using ostk::physics::env::obj::celest::Earth;
// using ostk::physics::env::obj::celest::Moon;
// using ostk::physics::env::obj::celest::Sun;
// using ostk::physics::time::Duration;
// using ostk::physics::time::Instant;

// using ostk::astro::flight::system::Dynamics;
// using ostk::astro::flight::system::SatelliteSystem;
// using ostk::astro::trajectory::State;
// using ostk::astro::trajectory::Force;
// using ostk::astro::trajectory::force::GravityForce;

// /// @brief                      Defines a satellite in orbit subject to forces of varying fidelity
// ///                             Represents a system of differential equations that can be solved by calling the
// ///                             NumericalSolver class

// class AugmentedSatelliteDynamics : public Dynamics
// {
//    public:
//     /// @brief              Constructor
//     ///
//     /// @code
//     ///                     Environment environment = { ... } ;
//     ///                     SatelliteSystem satelliteSystem = { ... } ;
//     ///                     AugmentedSatelliteDynamics satelliteDynamics = { environment, satelliteSystem } ;
//     /// @endcode
//     ///
//     /// @param              [in] anEnvironment An environment
//     /// @param              [in] aSatelliteSystem A satellite system

//     AugmentedSatelliteDynamics(const Environment& anEnvironment, const SatelliteSystem& aSatelliteSystem);

//     /// @brief              Copy Constructor
//     ///
//     /// @param              [in] AugmentedSatelliteDynamics A satellite dynamics

//     AugmentedSatelliteDynamics(const AugmentedSatelliteDynamics& aAugmentedSatelliteDynamics);

//     /// @brief              Destructor

//     virtual ~AugmentedSatelliteDynamics() override;

//     /// @brief              Clone satellite dynamics
//     ///
//     /// @return             Pointer to cloned satellite dynamics

//     virtual AugmentedSatelliteDynamics* clone() const override;

//     /// @brief              Equal to operator
//     ///
//     /// @param              [in] aAugmentedSatelliteDynamics A satellite dynamics
//     /// @return             True if satellite dynamics are equal

//     bool operator==(const AugmentedSatelliteDynamics& aAugmentedSatelliteDynamics) const;

//     /// @brief              Not equal to operator
//     ///
//     /// @param              [in] aAugmentedSatelliteDynamics A satellite dynamics
//     /// @return             True if satellite dynamics are not equal

//     bool operator!=(const AugmentedSatelliteDynamics& aAugmentedSatelliteDynamics) const;

//     /// @brief              Output stream operator
//     ///
//     /// @param              [in] anOutputStream An output stream
//     /// @param              [in] aAugmentedSatelliteDynamics A satellite dynamics
//     /// @return             A reference to output stream

//     friend std::ostream& operator<<(std::ostream& anOutputStream, const AugmentedSatelliteDynamics& aAugmentedSatelliteDynamics);

//     /// @brief              Check if satellite dynamics is defined
//     ///
//     /// @return             True if satellite dynamics is defined

//     virtual bool isDefined() const override;

//     /// @brief              Print satellite dynamics
//     ///
//     /// @param              [in] anOutputStream An output stream
//     /// @param              [in] (optional) displayDecorators If true, display decorators

//     virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

//     /// @brief              Get satellite dynamics initial instant
//     ///
//     /// @code
//     ///                     satelliteDynamics.getInstant() ;
//     /// @endcode

//     Instant getInstant() const;

//     /// @brief              Set satellite dynamics initial epoch
//     ///
//     /// @code
//     ///                     Instant instant = { ... } ;
//     ///                     satelliteDynamics.setInstant(instant) ;
//     /// @endcode
//     /// @param              [in] anInstant An instant

//     virtual void setInstant(const Instant& anInstant) override;  // Weird, improve

//     /// @brief              Obtain dynamical equations function wrapper
//     ///
//     /// @code
//     ///                     Dynamics::DynamicalEquationWrapper dyneq = satelliteDynamics.getDynamicalEquations() ;
//     /// @endcode
//     /// @return             std::function<void(const std::vector<double>&, std::vector<double>&, const double)>

//     Array<Shared<const Force>> accessForces () const;

//     virtual Dynamics::DynamicalEquationWrapper getDynamicalEquations() override;

//    private:
//     Environment environment_;
//     Shared<const Frame> gcrfSPtr_;
//     SatelliteSystem satelliteSystem_;
//     Array<Shared<Force>> forceArray_;
//     Instant instant_;

//     // Only force model currently used that incorporates solely Earth's gravity
//     void DynamicalEquations(const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const double t);

//     // // Atmospheric perturbations only
//     // void                    Exponential_Dynamics                        (   const   AugmentedSatelliteDynamics::StateVector&
//     // x,
//     //                                                                                 AugmentedSatelliteDynamics::StateVector&
//     //                                                                                 dxdt,
//     //                                                                         const   double ) const ;
// };

// }  // namespace dynamics
// }  // namespace system
// }  // namespace flight
// }  // namespace astro
// }  // namespace ostk

// #endif
