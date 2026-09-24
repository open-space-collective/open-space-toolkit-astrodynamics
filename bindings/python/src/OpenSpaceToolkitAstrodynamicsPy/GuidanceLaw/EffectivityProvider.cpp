/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider/Composite.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider/QLaw.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw_EffectivityProvider(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Real;
    using ostk::core::type::Shared;
    using ostk::core::type::Size;
    using ostk::core::type::String;

    using ostk::mathematics::object::Vector3d;

    using ostk::astrodynamics::guidancelaw::EffectivityProvider;
    using ostk::astrodynamics::guidancelaw::QLaw;
    using ostk::astrodynamics::trajectory::State;

    namespace effectivityprovider = ostk::astrodynamics::guidancelaw::effectivityprovider;

    // Trampoline class so Python subclasses can implement the interface.
    class PyEffectivityProvider : public EffectivityProvider
    {
       public:
        using EffectivityProvider::EffectivityProvider;

        bool isEffective(const State& aState, const Vector3d& aThrustDirectionThetaRH, const Real& aThrustAcceleration)
            const override
        {
            PYBIND11_OVERRIDE_PURE_NAME(
                bool,
                EffectivityProvider,
                "is_effective",
                isEffective,
                aState,
                aThrustDirectionThetaRH,
                aThrustAcceleration
            );
        }

        Real computeScore(const State& aState, const Vector3d& aThrustDirectionThetaRH, const Real& aThrustAcceleration)
            const override
        {
            PYBIND11_OVERRIDE_NAME(
                Real,
                EffectivityProvider,
                "compute_score",
                computeScore,
                aState,
                aThrustDirectionThetaRH,
                aThrustAcceleration
            );
        }
    };

    // Submodule: ostk.astrodynamics.guidance_law.effectivity_provider
    auto submodule = aModule.def_submodule("effectivity_provider");

    class_<EffectivityProvider, PyEffectivityProvider, Shared<EffectivityProvider>>(
        aModule,
        "EffectivityProvider",
        R"doc(
            Abstract effectivity provider.

            A provider decides whether a proposed thrust action at a given state should be
            applied (gate), and may optionally supply a scalar score so that guidance laws can
            rank candidate directions.

            Subclass this in Python by implementing ``is_effective`` (required) and optionally
            ``compute_score``. Direction is given in theta-R-H [theta, R, H]; direction-agnostic
            providers (eclipse, power) may ignore it.

        )doc"
    )
        .def(
            init<const String&>(),
            R"doc(
                Constructor.

                Args:
                    name (str): The provider name.
            )doc",
            arg("name")
        )
        .def("get_name", &EffectivityProvider::getName, R"doc(
            Get the provider name.

            Returns:
                str: The provider name.
        )doc")
        .def("__str__", &(shiftToString<EffectivityProvider>))
        .def("__repr__", &(shiftToString<EffectivityProvider>))
        .def(
            "is_effective",
            &EffectivityProvider::isEffective,
            R"doc(
                Gate: should the proposed thrust be applied?

                Args:
                    state (State): Current state.
                    thrust_direction_theta_r_h (np.array): Proposed direction in theta-R-H.
                    thrust_acceleration (float): Proposed thrust acceleration magnitude.

                Returns:
                    bool: True if the action should be applied.
            )doc",
            arg("state"),
            arg("thrust_direction_theta_r_h"),
            arg("thrust_acceleration")
        )
        .def(
            "compute_score",
            &EffectivityProvider::computeScore,
            R"doc(
                Optional scalar score for ranking candidate directions (higher = better).

                Returns ``Real.undefined()`` if the provider has no opinion on ranking.

                Args:
                    state (State): Current state.
                    thrust_direction_theta_r_h (np.array): Proposed direction in theta-R-H.
                    thrust_acceleration (float): Proposed thrust acceleration magnitude.

                Returns:
                    Real: Score (higher = better) or undefined.
            )doc",
            arg("state"),
            arg("thrust_direction_theta_r_h"),
            arg("thrust_acceleration")
        );

    // QLaw effectivity provider.
    class_<effectivityprovider::QLaw, EffectivityProvider, Shared<effectivityprovider::QLaw>>(
        submodule,
        "QLaw",
        R"doc(
            Effectivity provider backed by a QLaw's direction-aware effectivity metric.

            - Gate: passes when both the relative and absolute effectivity (where defined) clear
              their thresholds.
            - Score: returns the relative effectivity for the proposed direction.

            If both thresholds are passed as ``Real.undefined()``, the provider falls back to
            thresholds carried on ``QLaw.Parameters`` (deprecated path).

        )doc"
    )
        .def(
            init<const Shared<const QLaw>&, const Real&, const Real&>(),
            R"doc(
                Constructor.

                Args:
                    q_law (QLaw): QLaw whose effectivity is consulted. Must be non-null.
                    absolute_effectivity_threshold (Real, optional): Absolute threshold in [0, 1].
                        ``Real.undefined()`` disables that gate.
                    relative_effectivity_threshold (Real, optional): Relative threshold in [0, 1].
                        ``Real.undefined()`` disables that gate.
            )doc",
            arg("q_law"),
            arg("absolute_effectivity_threshold") = Real::Undefined(),
            arg("relative_effectivity_threshold") = Real::Undefined()
        )
        .def("__str__", &(shiftToString<effectivityprovider::QLaw>))
        .def("__repr__", &(shiftToString<effectivityprovider::QLaw>))
        .def("access_q_law", &effectivityprovider::QLaw::accessQLaw, R"doc(
            Access the held QLaw.

            Returns:
                QLaw: The held QLaw.
        )doc")
        .def(
            "get_absolute_effectivity_threshold",
            &effectivityprovider::QLaw::getAbsoluteEffectivityThreshold,
            R"doc(
                Get the absolute-effectivity threshold (or undefined if disabled).
            )doc"
        )
        .def(
            "get_relative_effectivity_threshold",
            &effectivityprovider::QLaw::getRelativeEffectivityThreshold,
            R"doc(
                Get the relative-effectivity threshold (or undefined if disabled).
            )doc"
        );

    // Composite effectivity provider.
    class_<effectivityprovider::Composite, EffectivityProvider, Shared<effectivityprovider::Composite>>(
        submodule,
        "Composite",
        R"doc(
            Effectivity provider combining several providers.

            Gate: AND of the sub-providers' gates (short-circuits on the first failing gate).
            Score: delegated to a single nominated provider (defaults to index 0).

        )doc"
    )
        .def(
            init<const Array<Shared<EffectivityProvider>>&, const Size&>(),
            R"doc(
                Constructor.

                Args:
                    providers (list[EffectivityProvider]): Sub-providers. Must be non-empty.
                    primary_index (int, optional): Index of the provider supplying the score.
                        Defaults to 0.
            )doc",
            arg("providers"),
            arg("primary_index") = 0,
            // Keep the Python providers list alive as long as the Composite. Without this,
            // Python-derived providers can be garbage-collected once their last Python
            // reference (the list literal) goes out of scope, even though the C++ Composite
            // still holds the shared_ptrs — leading to "pure virtual function" errors when
            // the trampoline tries to dispatch back to Python.
            keep_alive<1, 2>()
        )
        .def("__str__", &(shiftToString<effectivityprovider::Composite>))
        .def("__repr__", &(shiftToString<effectivityprovider::Composite>))
        .def("access_providers", &effectivityprovider::Composite::accessProviders, R"doc(
            Access the held providers (in construction order).
        )doc")
        .def("get_primary_index", &effectivityprovider::Composite::getPrimaryIndex, R"doc(
            Get the index of the provider supplying the score.
        )doc");
}
