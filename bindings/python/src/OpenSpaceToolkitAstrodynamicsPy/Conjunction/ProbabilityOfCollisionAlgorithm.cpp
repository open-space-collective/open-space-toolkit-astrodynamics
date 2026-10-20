/// Apache License 2.0

#include <pybind11/stl.h>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Conjunction/ProbabilityOfCollisionAlgorithm/Alfano2005.cpp>

using namespace pybind11;

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::HardBody;
using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;

// Trampoline class for virtual member functions
class PyProbabilityOfCollisionAlgorithm : public ProbabilityOfCollisionAlgorithm
{
   public:
    // Public constructor (the base class constructor is protected)
    PyProbabilityOfCollisionAlgorithm(const String& aName)
        : ProbabilityOfCollisionAlgorithm(aName)
    {
    }

    // Trampoline (need one for each virtual function)

    Array<String> identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            Array<String>,
            ProbabilityOfCollisionAlgorithm,
            "identify_unsatisfied_assumptions",
            identifyUnsatisfiedAssumptions,
            aCloseApproach
        );
    }

    Real computeProbabilityOfCollision(
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr
    ) const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            Real,
            ProbabilityOfCollisionAlgorithm,
            "compute_probability_of_collision",
            computeProbabilityOfCollision,
            aCloseApproach,
            aHardBody1SPtr,
            aHardBody2SPtr
        );
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_ProbabilityOfCollisionAlgorithm(pybind11::module& aModule)
{
    class_<ProbabilityOfCollisionAlgorithm, PyProbabilityOfCollisionAlgorithm, Shared<ProbabilityOfCollisionAlgorithm>>
        probabilityOfCollisionAlgorithm(
            aModule,
            "ProbabilityOfCollisionAlgorithm",
            R"doc(
            Probability of collision algorithm (abstract).

            Base class for algorithms that compute the probability of collision for a close approach.

            The recommended way to use an algorithm is as follows:
            1. Check if the algorithm is applicable to the close approach.
            2. If the algorithm is applicable, compute the probability of collision analysis,
            otherwise fall back to another algorithm.
            3. If the nominal probability of collision is in the robust region, take it, if it is in the diluted region,
            take the maximum probability of collision, otherwise (unknown region) the maximum probability of collision
            could not be located.

            Can inherit (calling the constructor with the algorithm name) and provide the virtual methods:
                - identify_unsatisfied_assumptions
                - compute_probability_of_collision
            to create a custom probability of collision algorithm.

            Examples:
                >>> if algorithm.is_applicable(close_approach):
                ...     analysis = algorithm.compute_probability_analysis(close_approach, hard_body_1, hard_body_2)
                ...     if analysis.region == ProbabilityOfCollisionAlgorithm.ProbabilityRegion.Robust:
                ...         probability_of_collision = analysis.probability_of_collision
                ...     elif analysis.region == ProbabilityOfCollisionAlgorithm.ProbabilityRegion.Diluted:
                ...         probability_of_collision = analysis.maximum_probability_of_collision
        )doc"
        );

    enum_<ProbabilityOfCollisionAlgorithm::ProbabilityRegion>(
        probabilityOfCollisionAlgorithm,
        "ProbabilityRegion",
        R"doc(
            Probability region of a close approach.
        )doc"
    )

        .value(
            "Robust",
            ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Robust,
            "The probability of collision is in the robust region."
        )
        .value(
            "Diluted",
            ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted,
            "The probability of collision is in the diluted region."
        )
        .value(
            "Unknown",
            ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Unknown,
            "The maximum probability of collision could not be located."
        )

        ;

    class_<ProbabilityOfCollisionAlgorithm::Analysis>(
        probabilityOfCollisionAlgorithm,
        "Analysis",
        R"doc(
            Probability of collision analysis of a close approach.
        )doc"
    )

        .def_readonly(
            "probability_of_collision",
            &ProbabilityOfCollisionAlgorithm::Analysis::probabilityOfCollision,
            "The probability of collision."
        )
        .def_readonly("region", &ProbabilityOfCollisionAlgorithm::Analysis::region, "The probability region.")
        .def_readonly(
            "maximum_probability_of_collision",
            &ProbabilityOfCollisionAlgorithm::Analysis::maximumProbabilityOfCollision,
            "The maximum probability of collision (None in the unknown region)."
        )
        .def_readonly(
            "maximum_probability_of_collision_scaling_factor",
            &ProbabilityOfCollisionAlgorithm::Analysis::maximumProbabilityOfCollisionScalingFactor,
            "The covariance scaling factor yielding the maximum probability of collision (None in the unknown region)."
        )
        .def_readonly(
            "maximum_probability_of_collision_sigma_scaling_factor",
            &ProbabilityOfCollisionAlgorithm::Analysis::maximumProbabilityOfCollisionSigmaScalingFactor,
            "The sigma scaling factor (i.e. the square root of the covariance scaling factor) yielding the maximum "
            "probability of collision (None in the unknown region)."
        )

        ;

    probabilityOfCollisionAlgorithm

        .def(
            init<const String&>(),
            R"doc(
                Constructor.

                Args:
                    name (str): The name of the probability of collision algorithm.
            )doc",
            arg("name")
        )

        .def(
            "get_name",
            &ProbabilityOfCollisionAlgorithm::getName,
            R"doc(
                Get the name of the probability of collision algorithm.

                Returns:
                    str: The name of the algorithm.
            )doc"
        )

        .def(
            "identify_unsatisfied_assumptions",
            &ProbabilityOfCollisionAlgorithm::identifyUnsatisfiedAssumptions,
            R"doc(
                Identify unsatisfied assumptions for a close approach.

                Use this before computing the probability of collision if you want to know
                why the algorithm might not be applicable to a close approach.

                The returned array might be non-exhaustive, as the algorithm implementation might
                exit early upon encountering an unsatisfied assumption.

                An empty array means that the algorithm is applicable to the close approach.

                Use is_applicable() instead if you only want to know if the algorithm is applicable,
                but do not necessarily care about the reasons why.

                Args:
                    close_approach (CloseApproach): A close approach.

                Returns:
                    list[str]: A non-exhaustive array of unsatisfied assumption descriptions.

                Examples:
                    >>> unsatisfied_assumptions = algorithm.identify_unsatisfied_assumptions(close_approach)
                    >>> if not unsatisfied_assumptions:
                    ...     analysis = algorithm.compute_probability_analysis(close_approach, hard_body_1, hard_body_2)
                    >>> if not unsatisfied_assumptions:
                    ...     probability_of_collision = algorithm.compute_probability_of_collision(
                    ...         close_approach, hard_body_1, hard_body_2
                    ...     )
            )doc",
            arg("close_approach")
        )

        .def(
            "compute_probability_of_collision",
            &ProbabilityOfCollisionAlgorithm::computeProbabilityOfCollision,
            R"doc(
                Compute the probability of collision.

                Args:
                    close_approach (CloseApproach): A close approach.
                    hard_body_1 (HardBody): The hard body of Object 1.
                    hard_body_2 (HardBody): The hard body of Object 2.

                Returns:
                    float: The probability of collision.
            )doc",
            arg("close_approach"),
            arg("hard_body_1"),
            arg("hard_body_2")
        )

        .def(
            "is_applicable",
            &ProbabilityOfCollisionAlgorithm::isApplicable,
            R"doc(
                Check if the algorithm is applicable to a close approach.

                Use this before computing the probability of collision if you want to know
                whether the algorithm is applicable, but do not necessarily care about the reasons why.

                Args:
                    close_approach (CloseApproach): A close approach.

                Returns:
                    bool: True if the algorithm is applicable to the close approach.

                Examples:
                    >>> if algorithm.is_applicable(close_approach):
                    ...     analysis = algorithm.compute_probability_analysis(close_approach, hard_body_1, hard_body_2)
                    >>> if algorithm.is_applicable(close_approach):
                    ...     probability_of_collision = algorithm.compute_probability_of_collision(
                    ...         close_approach, hard_body_1, hard_body_2
                    ...     )
            )doc",
            arg("close_approach")
        )

        .def(
            "compute_probability_analysis",
            &ProbabilityOfCollisionAlgorithm::computeProbabilityAnalysis,
            R"doc(
                Compute the probability of collision analysis of a close approach.

                This function computes the probability of collision and locates the maximum probability of
                collision as it scales the covariance(s): starting from the unscaled covariance(s), a root solver
                (bracket and solve) locates where the slope of the probability of collision with respect to the
                (logarithmic) scaling factor changes sign. The probability of collision is assumed to have a single
                maximum.

                - If the maximum probability of collision is found to the "right" of the nominal probability of
                collision (i.e. a scaling factor greater than or equal to 1), then the close approach is in the robust
                region (ProbabilityRegion.Robust).
                - If the maximum probability of collision is found to the "left" of the nominal probability of
                collision (i.e. a scaling factor less than 1), then the close approach is in the diluted region
                (ProbabilityRegion.Diluted).
                - If the root solver does not converge (e.g. the probability of collision does not change with the
                covariance scaling, or has no maximum), then the region is unknown (ProbabilityRegion.Unknown).

                The scaling factors yielding the maximum probability of collision are also returned, both as a
                covariance scaling factor and as a sigma scaling factor. In the unknown region
                (ProbabilityRegion.Unknown), the maximum probability of collision and its scaling factors are None.

                Args:
                    close_approach (CloseApproach): A close approach.
                    hard_body_1 (HardBody): The hard body of Object 1.
                    hard_body_2 (HardBody): The hard body of Object 2.
                    scale_object_1_covariance (bool, optional): Whether to scale Object 1 covariance. Defaults to True.
                    scale_object_2_covariance (bool, optional): Whether to scale Object 2 covariance. Defaults to True.

                Returns:
                    ProbabilityOfCollisionAlgorithm.Analysis: The probability of collision analysis of the close
                        approach.

                Raises:
                    RuntimeError: If neither covariance is to be scaled.
            )doc",
            arg("close_approach"),
            arg("hard_body_1"),
            arg("hard_body_2"),
            arg("scale_object_1_covariance") = true,
            arg("scale_object_2_covariance") = true
        )

        ;

    // Create "probability_of_collision_algorithm" python submodule
    auto probabilityOfCollisionAlgorithmModule = aModule.def_submodule("probability_of_collision_algorithm");

    OpenSpaceToolkitAstrodynamicsPy_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005(
        probabilityOfCollisionAlgorithmModule
    );
}
