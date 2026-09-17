/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm.hpp>

using namespace pybind11;

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

    bool isApplicable(const CloseApproach& aCloseApproach) const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            bool, ProbabilityOfCollisionAlgorithm, "is_applicable", isApplicable, aCloseApproach
        );
    }

    ProbabilityOfCollisionAlgorithm::ProbabilityRegion computeProbabilityRegion(
        const CloseApproach& aCloseApproach,
        const Length& aCombinedHardBodyRadius,
        const Real& aScalingFactorLowerBound,
        const Real& aScalingFactorUpperBound,
        const RootSolver& aRootSolver,
        const bool& scaleObject1Covariance,
        const bool& scaleObject2Covariance
    ) const override
    {
        PYBIND11_OVERRIDE_NAME(
            ProbabilityOfCollisionAlgorithm::ProbabilityRegion,
            ProbabilityOfCollisionAlgorithm,
            "compute_probability_region",
            computeProbabilityRegion,
            aCloseApproach,
            aCombinedHardBodyRadius,
            aScalingFactorLowerBound,
            aScalingFactorUpperBound,
            aRootSolver,
            scaleObject1Covariance,
            scaleObject2Covariance
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

            Can inherit and provide the virtual methods:
                - get_name
                - is_maximum_probability_of_collision_algorithm
                - is_applicable
                - compute_probability_region
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

                Returns:
                    bool: True if the algorithm computes a maximum probability of collision.
            )doc"
        )

        .def(
            "is_applicable",
            &ProbabilityOfCollisionAlgorithm::isApplicable,
            R"doc(
                Check if the algorithm is applicable to a close approach.

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

                Args:
                    close_approach (CloseApproach): A close approach.
                    combined_hard_body_radius (Length): A combined hard-body radius.
                    scaling_factor_lower_bound (float, optional): A lower bound for the covariance scaling factor. Defaults to 0.1.
                    scaling_factor_upper_bound (float, optional): An upper bound for the covariance scaling factor. Defaults to 10.0.
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

        ;
}
