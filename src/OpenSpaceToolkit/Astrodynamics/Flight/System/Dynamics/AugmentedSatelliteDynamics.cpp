// /// Apache License 2.0

// #include <OpenSpaceToolkit/Core/Error.hpp>
// #include <OpenSpaceToolkit/Core/Utilities.hpp>

// #include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/AugmentedSatelliteDynamics.hpp>

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

// using ostk::core::types::Shared;
// using ostk::core::types::Size;

// using ostk::physics::units::Derived;
// using ostk::physics::units::Length;
// using ostk::physics::units::Time;
// using ostk::physics::units::Mass;

// using ostk::astro::trajectory::Force;
// using ostk::astro::trajectory::force::GravityForce;

// static const Derived::Unit GravitationalParameterSIUnit =
//     Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

// AugmentedSatelliteDynamics::AugmentedSatelliteDynamics(const Environment& anEnvironment, const SatelliteSystem& aSatelliteSystem)
//     : Dynamics(),
//       environment_(anEnvironment),
//       gcrfSPtr_(Frame::GCRF()),
//       satelliteSystem_(aSatelliteSystem),
//       forceArray_(Array<Shared<Force>>::Empty()),  // Define here based on element from environment here basically (by default)
//       instant_(Instant::Undefined())
// {

//     // Initialize forceArray_ for gravitational forces
//     Size numberCelesialObjects = environment_.accessObjects().getSize();
//     forceArray_.reserve(numberCelesialObjects);

//     std::cout << "Constructor" << std::endl;

//     for (const auto& objectName : environment_.getObjectNames())
//     {
//         std::cout << objectName << std::endl;
//         GravityForce gravityForce = GravityForce(anEnvironment.accessCelestialObjectWithName(objectName));
//         Shared<GravityForce> gravityForceSPtr = std::make_shared<GravityForce>(gravityForce);  // Flaky check what's best here, Child class is the way to go
//         forceArray_.add(gravityForceSPtr);
//     }

// }

// AugmentedSatelliteDynamics::AugmentedSatelliteDynamics(const AugmentedSatelliteDynamics& aAugmentedSatelliteDynamics)
//     : Dynamics(aAugmentedSatelliteDynamics),
//       environment_(aAugmentedSatelliteDynamics.environment_),
//       gcrfSPtr_(aAugmentedSatelliteDynamics.gcrfSPtr_),
//       satelliteSystem_(aAugmentedSatelliteDynamics.satelliteSystem_),
//       instant_(Instant::Undefined())
// {
// }

// AugmentedSatelliteDynamics::~AugmentedSatelliteDynamics() {}

// AugmentedSatelliteDynamics* AugmentedSatelliteDynamics::clone() const
// {
//     return new AugmentedSatelliteDynamics(*this);
// }

// bool AugmentedSatelliteDynamics::operator==(const AugmentedSatelliteDynamics& aAugmentedSatelliteDynamics) const
// {
//     if ((!this->isDefined()) || (!aAugmentedSatelliteDynamics.isDefined()))
//     {
//         return false;
//     }

//     return (environment_.getInstant() == aAugmentedSatelliteDynamics.environment_.getInstant()) &&
//            (environment_.getObjectNames() == aAugmentedSatelliteDynamics.environment_.getObjectNames()) &&
//            (satelliteSystem_ == aAugmentedSatelliteDynamics.satelliteSystem_);
// }

// bool AugmentedSatelliteDynamics::operator!=(const AugmentedSatelliteDynamics& aAugmentedSatelliteDynamics) const
// {
//     return !((*this) == aAugmentedSatelliteDynamics);
// }

// std::ostream& operator<<(std::ostream& anOutputStream, const AugmentedSatelliteDynamics& aAugmentedSatelliteDynamics)
// {
//     aAugmentedSatelliteDynamics.print(anOutputStream);

//     return anOutputStream;
// }

// bool AugmentedSatelliteDynamics::isDefined() const
// {
//     return environment_.isDefined() && satelliteSystem_.isDefined();
// }

// void AugmentedSatelliteDynamics::print(std::ostream& anOutputStream, bool displayDecorator) const
// {
//     displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Satellite Dynamics") : void();

//     ostk::core::utils::Print::Line(anOutputStream) << "Environment:" << environment_;

//     ostk::core::utils::Print::Separator(anOutputStream, "Satellite System");
//     satelliteSystem_.print(anOutputStream, false);

//     displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
// }

// Instant AugmentedSatelliteDynamics::getInstant() const
// {
//     return instant_;
// }

// void AugmentedSatelliteDynamics::setInstant(const Instant& anInstant)
// {
//     this->instant_ = anInstant;
// }

// Array<Shared<const Force>> AugmentedSatelliteDynamics::accessForces () const
// {

//     Array<Shared<const Force>> forces = Array<Shared<const Force>>::Empty() ;

//     forces.reserve(forceArray_.getSize()) ;

//     for (const auto& forceSPtr : forceArray_)
//     {
//         forces.add(forceSPtr) ;
//     }

//     return forces ;

// }

// Dynamics::DynamicalEquationWrapper AugmentedSatelliteDynamics::getDynamicalEquations()
// {
//     if (!this->isDefined())
//     {
//         throw ostk::core::error::runtime::Undefined("AugmentedSatelliteDynamics");
//     }

//     if (!this->instant_.isDefined())
//     {
//         throw ostk::core::error::runtime::Undefined("Instant");
//     }

//     return std::bind(
//         &AugmentedSatelliteDynamics::DynamicalEquations,
//         this,
//         std::placeholders::_1,
//         std::placeholders::_2,
//         std::placeholders::_3
//     );
// }

// void AugmentedSatelliteDynamics::DynamicalEquations(const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const double t)
// {
//     const Position currentPosition = Position::Meters({x[0], x[1], x[2]}, gcrfSPtr_);
//     // const Mass currentMass = this->satelliteSystem_.getMass();
//     // const Real currentMassKilograms = currentMass.inKilograms();  // useful for initial value

//     // Check for radii below 70km altitude
//     if (currentPosition.getCoordinates().norm() < Earth::Models::EGM2008::EquatorialRadius.inMeters() + 70000.0)
//     {
//         throw ostk::core::error::RuntimeError("Satellite altitude too low, has re-entered.");
//     }

//     // Update environment time
//     const Instant currentInstant = instant_ + Duration::Seconds(t);
//     environment_.setInstant(currentInstant);

//     // Initialize total acceleration value
//     Vector3d totalAcceleration_SI = {0.0, 0.0, 0.0};

//     std::cout << "LA" << std::endl ;
//     std::cout << this->forceArray_.getSize() << std::endl ;

//     // Add force models acceleration contributions
//     for (const auto& forceModelSPtr : this->forceArray_)
//     {

//         std::cout << "HERE" << std::endl;

//         totalAcceleration_SI += forceModelSPtr->getContribution(
//             currentPosition,
//             currentInstant
//         );
//     }

//     // Initialize gravitational acceleration vector
//     // Vector3d totalGravitationalAcceleration_SI = {0.0, 0.0, 0.0};

//     // Access all objects in the environment and loop through them
//     // for (const auto& objectName : environment_.getObjectNames())
//     // {
//     //     if (objectName != "Earth")
//     //     {
//     //         // Obtain 3rd body effect on center of Earth (origin in GCRF) aka 3rd body correction
//     //         const Vector gravitationalAcceleration3rdBodyCorrection =
//     //             environment_.accessCelestialObjectWithName(objectName)
//     //                 ->getGravitationalFieldAt(Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr_));

//     //         // Subtract 3rd body correct from total gravitational acceleration
//     //         totalGravitationalAcceleration_SI -=
//     //             gravitationalAcceleration3rdBodyCorrection.inFrame(gcrfSPtr_, currentInstant).getValue();
//     //     }

//     //     // Obtain gravitational acceleration from current object
//     //     const Vector gravitationalAcceleration =
//     //         environment_.accessCelestialObjectWithName(objectName)->getGravitationalFieldAt(currentPosition);

//     //     // Add object's gravity to total gravitational acceleration
//     //     totalGravitationalAcceleration_SI += gravitationalAcceleration.inFrame(gcrfSPtr_, currentInstant).getValue();
//     // }

//     // Integrate position and velocity states
//     dxdt[0] = x[3];
//     dxdt[1] = x[4];
//     dxdt[2] = x[5];
//     dxdt[3] = totalAcceleration_SI[0];
//     dxdt[4] = totalAcceleration_SI[1];
//     dxdt[5] = totalAcceleration_SI[2];
//     dxdt[6] = -2 * x[6];  // Adding mass decrease law here
// }

// }  // namespace dynamics
// }  // namespace system
// }  // namespace flight
// }  // namespace astro
// }  // namespace ostk
