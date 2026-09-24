/// Apache License 2.0

#ifndef __OpenSpaceToolkitPy_Utility_EigenSequenceCasting__
#define __OpenSpaceToolkitPy_Utility_EigenSequenceCasting__

#include <nanobind/eigen/dense.h>
#include <nanobind/nanobind.h>

#include <Eigen/Dense>

/// @brief                      Accept any python sequence where a dense Eigen vector or matrix is expected.
///
///                             pybind11's Eigen support converted lists and tuples on the caller's behalf.
///                             nanobind's only accepts objects exposing DLPack or the buffer protocol, so
///                             `Position.meters([1.0, 2.0, 3.0], frame)` would stop working. These casters
///                             run nanobind's own conversion first and fall back to `numpy.asarray` for
///                             anything else, restoring the previous behaviour.
///
///                             This header must be included before any binding code that mentions one of
///                             the types below, since a specialization may not follow its first use.

namespace nanobind
{
namespace detail
{

template <class MatrixType>
struct EigenSequenceCaster
{
    using Scalar = typename MatrixType::Scalar;

    /// @brief                  A layout-identical Eigen::Array.
    ///
    ///                         This specialization replaces nanobind's own dense caster for MatrixType,
    ///                         which then can no longer be named. Eigen::Array is a distinct type with
    ///                         the same storage, so nanobind's caster stays reachable through it.

    using Bridge = Eigen::Array<
        Scalar,
        MatrixType::RowsAtCompileTime,
        MatrixType::ColsAtCompileTime,
        MatrixType::Options,
        MatrixType::MaxRowsAtCompileTime,
        MatrixType::MaxColsAtCompileTime>;

    using BridgeCaster = make_caster<Bridge>;

    NB_TYPE_CASTER(MatrixType, BridgeCaster::Name)

    bool from_python(handle aSource, uint8_t someFlags, cleanup_list* aCleanupList) noexcept
    {
        BridgeCaster caster;

        if (caster.from_python(aSource, someFlags, aCleanupList))
        {
            value = caster.value.matrix();

            return true;
        }

        if ((someFlags & static_cast<uint8_t>(cast_flags::convert)) == 0)
        {
            return false;
        }

        // Turn the sequence into an array of the right scalar type. Asking for the dtype matters:
        // a list holding an `ostk::core::type::Real` (or any object with __float__) would otherwise
        // come back with object dtype, which nanobind rejects.

        object array;

        try
        {
            array = module_::import_("numpy").attr("asarray")(aSource, numpyDtype());
        }
        catch (...)
        {
            return false;
        }

        if (!caster.from_python(array.ptr(), someFlags, aCleanupList))
        {
            return false;
        }

        // `caster.value` views `array`, which is alive for the duration of this copy.

        value = caster.value.matrix();

        return true;
    }

    /// @brief                  The numpy dtype string matching Scalar, e.g. "f8" for a double.

    static const char* numpyDtype() noexcept
    {
        static_assert(sizeof(Scalar) <= 9, "Unsupported scalar size.");

        static const char dtype[3] = {
            std::is_floating_point_v<Scalar> ? 'f' : (std::is_signed_v<Scalar> ? 'i' : 'u'),
            static_cast<char>('0' + sizeof(Scalar)),
            '\0',
        };

        return dtype;
    }

    static handle from_cpp(const MatrixType& aMatrix, rv_policy, cleanup_list* aCleanupList) noexcept
    {
        // The bridge is a fresh copy, so it can always be moved from. No binding in this project
        // returns a plain Eigen value under a view policy, which would need the original storage.

        return BridgeCaster::from_cpp(Bridge(aMatrix.array()), rv_policy::move, aCleanupList);
    }
};

#define OSTK_EIGEN_SEQUENCE_CASTER(...)                                \
    template <>                                                        \
    struct type_caster<__VA_ARGS__> : EigenSequenceCaster<__VA_ARGS__> \
    {                                                                  \
    };

// Matches the aliases in open-space-toolkit-mathematics' Object/Vector.hpp.
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::Vector2i)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::Vector3i)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::Vector4i)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::VectorXi)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::Vector2d)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::Vector3d)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::Vector4d)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::Matrix<double, 6, 1>)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::VectorXd)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::Matrix2i)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::Matrix3i)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::Matrix4i)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::MatrixXi)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::Matrix2d)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::Matrix3d)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::Matrix4d)
OSTK_EIGEN_SEQUENCE_CASTER(Eigen::MatrixXd)

#undef OSTK_EIGEN_SEQUENCE_CASTER

}  // namespace detail
}  // namespace nanobind

#endif
