/// Apache License 2.0

#ifndef __OpenSpaceToolkitAstrodynamicsPy_Utility_EigenCasting__
#define __OpenSpaceToolkitAstrodynamicsPy_Utility_EigenCasting__

#include <cstring>
#include <type_traits>

#include <nanobind/eigen/dense.h>
#include <nanobind/nanobind.h>

namespace nanobind
{
namespace detail
{

/// @brief                      Type caster for dense Eigen::Matrix types that also accepts Python sequences
///
///                             nanobind's own caster (nanobind/eigen/dense.h) only accepts inputs that implement
///                             DLPack or the buffer protocol (NumPy arrays and friends). pybind11's Eigen caster
///                             also accepted any sequence that NumPy can convert (for example a list of floats)
///                             when implicit conversions are allowed, and OSTk code relies on that
///                             (`Point.vector([1.0, 2.0, 3.0])`, `Linear([0.0, 1.0], [0.0, 2.0])`, ...).
///
///                             For array inputs it behaves exactly like nanobind's caster. It adds a fallback: when
///                             implicit conversions are allowed and nanobind's conversion fails, a non-string input
///                             is first converted with `numpy.asarray(input)` and then loaded like an array, which
///                             also converts the dtype (int -> float) and the memory order. As with pybind11, an
///                             (N, 1) array is accepted for a column vector of size N.
///
///                             Return values follow nanobind's policies, with one change: a copy (or a move) is
///                             made into a heap object owned by a capsule and handed to NumPy as a view
///                             (`numpy.asarray`), instead of through `numpy.copy`. The result is still an
///                             independent, writeable array, and returning a Vector3d is about 4x faster.
///
///                             Its name says the same in signatures and stubs: parameters take
///                             `Sequence[float] | NDArray` (`Sequence[Sequence[float]] | NDArray` for a matrix), return
///                             values are `NDArray`. Renaming is safe because these types are not bound classes:
///                             nanobind reads the name of a bound class at runtime, to recognise its constructors.
///
///                             C++ cannot rank a second partial specialization of `type_caster` against
///                             nanobind's `enable_if`-constrained one (GCC reports them as ambiguous), so the
///                             caster is attached to each concrete type with an explicit specialization, see
///                             OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER below. This header must be included before any
///                             binding code that converts one of these types. Eigen::Map, Eigen::Ref, Eigen::Array
///                             and the types not listed below keep nanobind's caster.

template <typename T>
struct eigen_sequence_caster
{
    using Scalar = typename T::Scalar;
    using NDArray = array_for_eigen_t<T>;
    using NDArrayCaster = make_caster<NDArray>;

    static_assert(std::is_floating_point_v<Scalar>, "The parameter name below declares a sequence of float.");

    // Parameter name, then return value name (see `io_name`). The ndarray name holds no type placeholder, so it
    // can be embedded: nanobind copies this text verbatim.
    NB_TYPE_CASTER(
        T,
        const_name('@') +
            const_name<ndim_v<T> == 1>(
                "collections.abc.Sequence[float]", "collections.abc.Sequence[collections.abc.Sequence[float]]"
            ) +
            const_name(" | ") + NDArrayCaster::Name + const_name('@') + NDArrayCaster::Name + const_name('@')
    )

    bool from_python(handle src, uint32_t flags, cleanup_list* cleanup) noexcept
    {
        if (fromArray(src, flags, cleanup))
        {
            return true;
        }

        if (!(flags & (uint32_t)cast_flags::convert) || PyUnicode_Check(src.ptr()) || PyBytes_Check(src.ptr()))
        {
            return false;
        }

        try
        {
            object array = module_::import_("numpy").attr("asarray")(src);

            // Like pybind11, accept an (N, 1) array for a column vector of size N.
            if constexpr ((ndim_v<T> == 1) && (T::ColsAtCompileTime == 1))
            {
                const nanobind::tuple shape = nanobind::cast<nanobind::tuple>(array.attr("shape"));

                if ((shape.size() == 2) && (nanobind::cast<size_t>(shape[1]) == 1))
                {
                    array = array.attr("reshape")(-1);
                }
            }

            if (array.is(src))
            {
                return false;
            }

            return fromArray(array, flags, cleanup);
        }
        catch (...)
        {
            // Ragged or non-numeric input: report a failed conversion rather than raising.
            PyErr_Clear();
            return false;
        }
    }

    template <typename T2>
    static handle from_cpp(T2&& v, rv_policy policy, cleanup_list* cleanup) noexcept
    {
        policy = infer_policy<T2>(policy);

        if constexpr (std::is_pointer_v<T2>)
        {
            return fromCppInternal((const T&)*v, policy, cleanup);
        }
        else
        {
            return fromCppInternal((const T&)v, policy, cleanup);
        }
    }

   private:
    bool fromArray(handle src, uint32_t flags, cleanup_list* cleanup) noexcept
    {
        // A copy is made in any case, so read-only inputs are fine.
        using NDArrayConst = array_for_eigen_t<T, const Scalar>;

        make_caster<NDArrayConst> caster;

        if (!caster.from_python(src, flags & ~(uint32_t)cast_flags::accepts_none, cleanup))
        {
            return false;
        }

        const NDArrayConst& array = caster.value;

        if constexpr (ndim_v<T> == 1)
        {
            value.resize((Eigen::Index)array.shape(0));
        }
        else
        {
            value.resize((Eigen::Index)array.shape(0), (Eigen::Index)array.shape(1));
        }

        // The layout is contiguous and compatible thanks to array_for_eigen_t<T>.
        std::memcpy(value.data(), array.data(), array.size() * sizeof(Scalar));

        return true;
    }

    // Same as nanobind's own caster (nanobind/eigen/dense.h), except for the copy / move export.
    static handle fromCppInternal(const T& v, rv_policy policy, cleanup_list* cleanup) noexcept
    {
        size_t shape[ndim_v<T>];
        int64_t strides[ndim_v<T>];

        if constexpr (ndim_v<T> == 1)
        {
            shape[0] = (size_t)v.size();
            strides[0] = v.innerStride();
        }
        else
        {
            shape[0] = (size_t)v.rows();
            shape[1] = (size_t)v.cols();
            strides[0] = v.rowStride();
            strides[1] = v.colStride();
        }

        void* ptr = (void*)v.data();

        object owner;

        if ((policy == rv_policy::move) || (policy == rv_policy::copy))
        {
            // Hand NumPy a view of a heap copy owned by a capsule. Exporting through
            // `numpy.asarray` (view) is several times cheaper than through `numpy.copy`.
            T* temp = (policy == rv_policy::move) ? new T((T&&)v) : new T(v);
            owner = capsule(
                temp,
                [](void* p) noexcept
                {
                    delete (T*)p;
                }
            );
            ptr = temp->data();
            policy = rv_policy::reference;
        }
        else if (policy == rv_policy::reference_internal)
        {
            // reference_internal needs a self pointer; give up if unavailable.
            if (!cleanup || !cleanup->self())
            {
                return handle();
            }

            owner = borrow(cleanup->self());
            policy = rv_policy::reference;
        }

        object o = steal(NDArrayCaster::from_cpp(NDArray(ptr, ndim_v<T>, shape, owner, strides), policy, cleanup));

        return o.release();
    }
};

/// @brief                      Attach the sequence-accepting caster to a concrete Eigen::Matrix type
///
///                             Must be used inside `namespace nanobind::detail`, once per type and module.

#define OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER(...)                         \
    template <>                                                          \
    struct type_caster<__VA_ARGS__> : eigen_sequence_caster<__VA_ARGS__> \
    {                                                                    \
    }

OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER(Eigen::Matrix<double, 2, 1>);
OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER(Eigen::Matrix<double, 3, 1>);
OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER(Eigen::Matrix<double, 4, 1>);
OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER(Eigen::Matrix<double, 5, 1>);
OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER(Eigen::Matrix<double, 6, 1>);
OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER(Eigen::Matrix<double, Eigen::Dynamic, 1>);
OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER(Eigen::Matrix<double, 2, 2>);
OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER(Eigen::Matrix<double, 3, 3>);
OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER(Eigen::Matrix<double, 4, 4>);
OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER(Eigen::Matrix<double, 5, 3>);
OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER(Eigen::Matrix<double, 5, 5>);
OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER(Eigen::Matrix<double, 6, 6>);
OSTK_NANOBIND_EIGEN_SEQUENCE_CASTER(Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>);

}  // namespace detail
}  // namespace nanobind

#endif
