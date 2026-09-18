/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Conjunction/ProbabilityOfCollisionAlgorithm/Frisbee2015.cpp>

using namespace pybind11;

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::physics::unit::Length;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
using ostk::astrodynamics::RootSolver;

// Trampoline class for virtual member functions
class PyProbabilityOfCollisionAlgorithm : public ProbabilityOfCollisionAlgorithm
{
   public:
    using ProbabilityOfCollisionAlgorithm::ProbabilityOfCollisionAlgorithm;

    // Trampoline (need one for each virtual function)

    String getName() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(String, ProbabilityOfCollisionAlgorithm, "get_name", getName);
    }

    bool isMaximumProbabilityOfCollisionAlgorithm() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            bool,
            ProbabilityOfCollisionAlgorithm,
            "is_maximum_probability_of_collision_algorithm",
            isMaximumProbabilityOfCollisionAlgorithm
        );
    }

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

    Real computeProbabilityOfCollision(const CloseApproach& aCloseApproach, const Length& aCombinedHardBodyRadius)
        const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            Real,
            ProbabilityOfCollisionAlgorithm,
            "compute_probability_of_collision",
            computeProbabilityOfCollision,
            aCloseApproach,
            aCombinedHardBodyRadius
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

            One should check the algorithm applicability (either via is_applicable() or
            identify_unsatisfied_assumptions()) before computing the probability of collision.

            Can inherit and provide the virtual methods:
                - get_name
                - is_maximum_probability_of_collision_algorithm
                - identify_unsatisfied_assumptions
                - compute_probability_of_collision
            to create a custom probability of collision algorithm.
        )doc"
        );

    enum_<ProbabilityOfCollisionAlgorithm::ProbabilityRegion>(
        probabilityOfCollisionAlgorithm,
        "ProbabilityRegion",
        R"doc(
            Probability region of a close approach.
        )doc"
    )

        .value("Robust", ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Robust, "Robust")
        .value("Diluted", ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted, "Diluted")
        .value("Unknown", ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Unknown, "Unknown")

        ;

    probabilityOfCollisionAlgorithm

        .def(init<>())

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
            "is_maximum_probability_of_collision_algorithm",
            &ProbabilityOfCollisionAlgorithm::isMaximumProbabilityOfCollisionAlgorithm,
            R"doc(
                Check if the algorithm computes a maximum probability of collision.

                Use this to interpret the produced probability of collision as an
                actual probability or as an upper bound.

                Returns:
                    bool: True if the algorithm computes a maximum probability of collision.
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

                unsatisfied_assumptions = algorithm.identify_unsatisfied_assumptions(close_approach)
                if not unsatisfied_assumptions:
                    algorithm.compute_probability_of_collision(close_approach, combined_hard_body_radius)

                Args:
                    close_approach (CloseApproach): A close approach.

                Returns:
                    list[str]: A non-exhaustive array of unsatisfied assumption descriptions.
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
                    combined_hard_body_radius (Length): A combined hard-body radius.

                Returns:
                    float: The probability of collision.
            )doc",
            arg("close_approach"),
            arg("combined_hard_body_radius")
        )

        .def(
            "is_applicable",
            &ProbabilityOfCollisionAlgorithm::isApplicable,
            R"doc(
                Check if the algorithm is applicable to a close approach.

                Use this before computing the probability of collision if you want to know
                whether the algorithm is applicable, but do not necessarily care about the reasons why.

                if algorithm.is_applicable(close_approach):
                    algorithm.compute_probability_of_collision(close_approach, combined_hard_body_radius)

                Args:
                    close_approach (CloseApproach): A close approach.

                Returns:
                    bool: True if the algorithm is applicable to the close approach.
            )doc",
            arg("close_approach")
        )

        .def(
            "compute_probability_region",
            &ProbabilityOfCollisionAlgorithm::computeProbabilityRegion,
            R"doc(
                Compute the probability region of a close approach.

                This function attempts to locate the maximum probability of collision by
                looking at the differentially-computed slope of the probability of collision as it scales
                the covariance(s) over the scaling interval.

                If the maximum is found to the "right" (i.e. a scaling factor greater than 1), then the
                close approach is in the robust region (ProbabilityRegion.Robust).

                If the maximum is found to the "left" (i.e. a scaling factor less than 1), then the
                close approach is in the diluted region (ProbabilityRegion.Diluted).

                This function may return ProbabilityRegion.Unknown under certain conditions:
                - The root solver did not converge
                - The algorithm is a maximum probability of collision algorithm
                - The algorithm is not applicable to the close approach

                Args:
                    close_approach (CloseApproach): A close approach.
                    combined_hard_body_radius (Length): A combined hard-body radius.
                    scaling_factor_lower_bound (float, optional): A lower bound for the covariance scaling factor. Defaults to 0.1 (i.e. as low as 0.1x the original covariance).
                    scaling_factor_upper_bound (float, optional): An upper bound for the covariance scaling factor. Defaults to 10.0 (i.e. as high as 10x the original covariance).
                    root_solver (RootSolver, optional): A root solver. Defaults to a Root Solver with 100 iterations and a tolerance of 0.1.
                    scale_object_1_covariance (bool, optional): Whether to scale Object 1 covariance. Defaults to True.
                    scale_object_2_covariance (bool, optional): Whether to scale Object 2 covariance. Defaults to True.

                Returns:
                    ProbabilityRegion: The probability region of the close approach.
            )doc",
            arg("close_approach"),
            arg("combined_hard_body_radius"),
            arg_v("scaling_factor_lower_bound", Real(0.1), "0.1"),
            arg_v("scaling_factor_upper_bound", Real(10.0), "10.0"),
            arg_v("root_solver", RootSolver(100, 0.1), "RootSolver(100, 0.1)"),
            arg("scale_object_1_covariance") = true,
            arg("scale_object_2_covariance") = true
        )

        ;

    // Create "probability_of_collision_algorithm" python submodule
    auto probabilityOfCollisionAlgorithmModule = aModule.def_submodule("probability_of_collision_algorithm");

    OpenSpaceToolkitAstrodynamicsPy_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015(
        probabilityOfCollisionAlgorithmModule
    );
}
