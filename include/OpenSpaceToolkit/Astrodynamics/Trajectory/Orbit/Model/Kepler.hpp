/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler__

#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{
namespace model
{

using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::physics::environment::object::Celestial;
using ostk::physics::time::Instant;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::State;

#define DEFAULT_IN_FIXED_FRAME false

/// @brief Keplerian orbit model.
///
/// @details Provides analytical orbit propagation using classical orbital elements with optional J2/J4 perturbations.
class Kepler : public ostk::astrodynamics::trajectory::orbit::Model
{
   public:
    enum class PerturbationType
    {

        None,  ///< No perturbations; pure two-body Keplerian propagation.
        J2,    ///< First-order J2 oblateness perturbation.
        J4     ///< Second-order J4 oblateness perturbation (includes J2).

    };

    /// @brief Construct a Kepler model from explicit gravitational parameters.
    ///
    /// @code{.cpp}
    ///     COE coe = COE::Circular({ Length::Kilometers(6878.0), Angle::Degrees(97.5) });
    ///     Instant epoch = Instant::J2000();
    ///     Derived mu = Derived(3.986004418e14, Derived::Unit::GravitationalParameter());
    ///     Length Re = Length::Kilometers(6378.137);
    ///     Real j2 = 1.08262668e-3;
    ///     Real j4 = -1.6109876e-6;
    ///     Kepler model(coe, epoch, mu, Re, j2, j4, Kepler::PerturbationType::J2);
    /// @endcode
    ///
    /// @param aClassicalOrbitalElementSet Classical orbital elements defining the orbit.
    /// @param anEpoch Epoch at which the orbital elements are defined.
    /// @param aGravitationalParameter Gravitational parameter of the central body.
    /// @param anEquatorialRadius Equatorial radius of the central body.
    /// @param aJ2 J2 zonal harmonic coefficient of the central body.
    /// @param aJ4 J4 zonal harmonic coefficient of the central body.
    /// @param aPerturbationType Perturbation type to apply during propagation.
    Kepler(
        const COE& aClassicalOrbitalElementSet,
        const Instant& anEpoch,
        const Derived& aGravitationalParameter,
        const Length& anEquatorialRadius,
        const Real& aJ2,
        const Real& aJ4,
        const Kepler::PerturbationType& aPerturbationType
    );

    /// @brief Construct a Kepler model from a celestial body object.
    ///
    /// @code{.cpp}
    ///     COE coe = COE::Circular({ Length::Kilometers(6878.0), Angle::Degrees(97.5) });
    ///     Instant epoch = Instant::J2000();
    ///     Shared<Celestial> earthSPtr = std::make_shared<Celestial>(Celestial::Earth());
    ///     Kepler model(coe, epoch, *earthSPtr, Kepler::PerturbationType::J2);
    /// @endcode
    ///
    /// @param aClassicalOrbitalElementSet Classical orbital elements defining the orbit.
    /// @param anEpoch Epoch at which the orbital elements are defined.
    /// @param aCelestialObject Celestial body providing gravitational parameters.
    /// @param aPerturbationType Perturbation type to apply during propagation.
    /// @param inFixedFrame If true, the provided orbital elements are given in the body-fixed frame.
    Kepler(
        const COE& aClassicalOrbitalElementSet,
        const Instant& anEpoch,
        const Celestial& aCelestialObject,
        const Kepler::PerturbationType& aPerturbationType,
        const bool inFixedFrame = DEFAULT_IN_FIXED_FRAME
    );

    /// @brief Clone the Kepler model.
    ///
    /// @return A pointer to a heap-allocated copy of this Kepler model.
    virtual Kepler* clone() const override;

    /// @brief Check equality between two Kepler models.
    ///
    /// @param aKeplerianModel Another Kepler model to compare against.
    /// @return True if both models are equal.
    bool operator==(const Kepler& aKeplerianModel) const;

    /// @brief Check inequality between two Kepler models.
    ///
    /// @param aKeplerianModel Another Kepler model to compare against.
    /// @return True if the models are not equal.
    bool operator!=(const Kepler& aKeplerianModel) const;

    /// @brief Stream insertion operator for Kepler model.
    ///
    /// @param anOutputStream Output stream to write to.
    /// @param aKeplerianModel Kepler model to stream.
    /// @return Reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Kepler& aKeplerianModel);

    /// @brief Check if the Kepler model is defined.
    ///
    /// @return True if the model is fully defined.
    virtual bool isDefined() const override;

    /// @brief Get the classical orbital elements.
    ///
    /// @return The classical orbital element set used by this model.
    COE getClassicalOrbitalElements() const;

    /// @brief Get the epoch of the orbital elements.
    ///
    /// @return The epoch at which the orbital elements are defined.
    virtual Instant getEpoch() const override;

    /// @brief Get the revolution number at the epoch.
    ///
    /// @return The revolution number at the epoch instant.
    virtual Integer getRevolutionNumberAtEpoch() const override;

    /// @brief Get the gravitational parameter of the central body.
    ///
    /// @return The gravitational parameter used in this model.
    Derived getGravitationalParameter() const;

    /// @brief Get the equatorial radius of the central body.
    ///
    /// @return The equatorial radius used in this model.
    Length getEquatorialRadius() const;

    /// @brief Get the J2 zonal harmonic coefficient.
    ///
    /// @return The J2 coefficient used in this model.
    Real getJ2() const;

    /// @brief Get the J4 zonal harmonic coefficient.
    ///
    /// @return The J4 coefficient used in this model.
    Real getJ4() const;

    /// @brief Get the perturbation type applied by this model.
    ///
    /// @return The perturbation type used during propagation.
    Kepler::PerturbationType getPerturbationType() const;

    /// @brief Calculate the spacecraft state at a given instant.
    ///
    /// @param anInstant The instant at which to evaluate the state.
    /// @return The propagated state at the given instant.
    virtual State calculateStateAt(const Instant& anInstant) const override;

    /// @brief Calculate the revolution number at a given instant.
    ///
    /// @param anInstant The instant at which to evaluate the revolution number.
    /// @return The revolution number at the given instant.
    virtual Integer calculateRevolutionNumberAt(const Instant& anInstant) const override;  // [TBR] ?

    /// @brief Print the Kepler model to an output stream.
    ///
    /// @param anOutputStream Output stream to write to.
    /// @param displayDecorator If true, print a decorator header and footer.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Convert a perturbation type to its string representation.
    ///
    /// @code{.cpp}
    ///     String name = Kepler::StringFromPerturbationType(Kepler::PerturbationType::J2);
    /// @endcode
    ///
    /// @param aPerturbationType The perturbation type to convert.
    /// @return A string representation of the perturbation type.
    static String StringFromPerturbationType(const Kepler::PerturbationType& aPerturbationType);

   protected:
    virtual bool operator==(const trajectory::Model& aModel) const override;

    virtual bool operator!=(const trajectory::Model& aModel) const override;

   private:
    COE coe_;
    Instant epoch_;
    Derived gravitationalParameter_;
    Length equatorialRadius_;
    Real j2_;
    Real j4_;
    Kepler::PerturbationType perturbationType_;

    static COE InertialCoeFromFixedCoe(
        const COE& aClassicalOrbitalElementSet, const Instant& anEpoch, const Celestial& aCelestialObject
    );

    static State CalculateNoneStateAt(
        const COE& aClassicalOrbitalElementSet,
        const Instant& anEpoch,
        const Derived& aGravitationalParameter,
        const Instant& anInstant
    );

    static Integer CalculateNoneRevolutionNumberAt(
        const COE& aClassicalOrbitalElementSet,
        const Instant& anEpoch,
        const Derived& aGravitationalParameter,
        const Instant& anInstant
    );

    static State CalculateJ2StateAt(
        const COE& aClassicalOrbitalElementSet,
        const Instant& anEpoch,
        const Derived& aGravitationalParameter,
        const Instant& anInstant,
        const Length& anEquatorialRadius,
        const Real& aJ2
    );

    static Integer CalculateJ2RevolutionNumberAt(
        const COE& aClassicalOrbitalElementSet,
        const Instant& anEpoch,
        const Derived& aGravitationalParameter,
        const Instant& anInstant,
        const Length& anEquatorialRadius,
        const Real& aJ2
    );

    static State CalculateJ4StateAt(
        const COE& aClassicalOrbitalElementSet,
        const Instant& anEpoch,
        const Derived& aGravitationalParameter,
        const Instant& anInstant,
        const Length& anEquatorialRadius,
        const Real& aJ2,
        const Real& aJ4
    );

    static Integer CalculateJ4RevolutionNumberAt(
        const COE& aClassicalOrbitalElementSet,
        const Instant& anEpoch,
        const Derived& aGravitationalParameter,
        const Instant& anInstant,
        const Length& anEquatorialRadius,
        const Real& aJ2,
        const Real& aJ4
    );
};

}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
