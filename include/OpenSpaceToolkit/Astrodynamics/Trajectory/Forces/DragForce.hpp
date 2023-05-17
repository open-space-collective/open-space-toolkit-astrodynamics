// /// Apache License 2.0

// #ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_DragForce__
// #define __OpenSpaceToolkit_Astrodynamics_Trajectory_DragForce__

// #include <OpenSpaceToolkit/Core/Types/Shared.hpp>

// #include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
// // #include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
// // #include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
// // #include <OpenSpaceToolkit/Physics/Units/Length.hpp>

// // #include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
// // #include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
// #include <OpenSpaceToolkit/Astrodynamics/Trajectory/Force.hpp>

// namespace ostk
// {
// namespace astro
// {
// namespace trajectory
// {
// namespace force
// {

// using ostk::core::types::Shared;
// using ostk::physics::env::objects::Celestial;
// using ostk::astro::trajectory::Force;

// class DragForce : public Force
// {
//    public:

//     DragForce(
//         const CelestialObject& aCelestialObject,
//     );

//     virtual DragForce* clone() const override;

//     bool operator==(const DragForce& aDragForce) const;

//     bool operator!=(const DragForce& aDragForce) const;

//     friend std::ostream& operator<<(std::ostream& anOutputStream, const DragForce& aDragForce);

//     virtual bool isDefined() const override;

//     Vector3d getContribution() const;

//     virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

//     static String StringFromPerturbationType(const Kepler::PerturbationType& aPerturbationType);

//    protected:
//     virtual bool operator==(const trajectory::Model& aModel) const override;

//     virtual bool operator!=(const trajectory::Model& aModel) const override;

//    private:
//     Shared<CelestialObject> celestialObjectPtr_;

// };


// }  // namespace force
// }  // namespace trajectory
// }  // namespace astro
// }  // namespace ostk

// #endif
