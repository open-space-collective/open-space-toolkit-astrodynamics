/// Apache License 2.0

#ifndef __OpenSpaceToolkitAstrodynamicsPy_Utility_BooleanCasting__
#define __OpenSpaceToolkitAstrodynamicsPy_Utility_BooleanCasting__

#include <functional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>
#include <nanobind/trampoline.h>

/// Boolean values returned by Python callables
///
/// nanobind's `bool` caster only accepts `True` and `False`. pybind11 converted the value returned by a Python
/// callable to `bool` with its truth value (`bool(result)`), and Python callbacks and overrides commonly return
/// `numpy.bool_` (for example `lambda state: state.get_coordinates()[2] > 0.0`). With nanobind's caster alone those
/// raise `std::bad_cast` from inside C++. This header restores pybind11's behaviour for values returned to C++ by:
/// - Python callables passed as `std::function<bool(...)>` (event condition evaluators, access and state filters,
///   temporal conditions), through a more specialized `type_caster<std::function<bool(Args...)>>`;
/// - Python overrides of pure virtual `bool` methods, through `OSTK_NB_OVERRIDE_PURE_NAME_BOOL`.
/// `bool` function arguments keep nanobind's caster (only `True` and `False` are accepted).

namespace nanobind
{
namespace detail
{

/// @brief                      Truth value of a Python object, as `bool(anObject)`
///
/// @param                      [in] anObject A Python object
/// @return                     Truth value

inline bool ostkTruthValue(handle anObject)
{
    if (anObject.ptr() == Py_True)
    {
        return true;
    }

    if (anObject.ptr() == Py_False)
    {
        return false;
    }

    const int truthValue = PyObject_IsTrue(anObject.ptr());

    if (truthValue < 0)
    {
        raise_python_error();
    }

    return truthValue != 0;
}

/// @brief                      Type caster for std::function returning bool
///
///                             Same as nanobind's std::function caster (nanobind/stl/function.h), except that the
///                             value returned by a Python callable is converted with its truth value.

template <typename... Args>
struct type_caster<std::function<bool(Args...)>>
{
    NB_TYPE_CASTER(
        std::function<bool(Args...)>,
        const_name("collections.abc.Callable[[") + concat(make_caster<Args>::Name...) + const_name("], ") +
            make_caster<bool>::Name + const_name("]")
    )

    struct pyfunc_wrapper_t : pyfunc_wrapper
    {
        using pyfunc_wrapper::pyfunc_wrapper;

        bool operator()(Args... args) const
        {
            gil_scoped_acquire acquire;

            if (!acquire.is_valid())
            {
                raise("nanobind: cannot invoke a Python callable, the interpreter is shutting down!");
            }

            return ostkTruthValue(handle(f)((forward_t<Args>)args...));
        }
    };

    bool from_python(handle src, uint32_t flags, cleanup_list*) noexcept
    {
        if (src.is_none())
        {
            return flags & (uint32_t)cast_flags::convert;
        }

        if (!PyCallable_Check(src.ptr()))
        {
            return false;
        }

        value = pyfunc_wrapper_t(src.ptr());

        return true;
    }

    static handle from_cpp(const Value& value, rv_policy policy, cleanup_list*) noexcept
    {
        const pyfunc_wrapper_t* wrapper = value.template target<pyfunc_wrapper_t>();

        if (wrapper)
        {
            return handle(wrapper->f).inc_ref();
        }

        if (policy == rv_policy::none)
        {
            return handle();
        }

        if (!value)
        {
            return none().release();
        }

        return cpp_function(value).release();
    }
};

}  // namespace detail
}  // namespace nanobind

/// @brief                      Same as NB_OVERRIDE_PURE_NAME, for a pure virtual method returning bool
///
///                             The value returned by the Python override is converted with its truth value.

#define OSTK_NB_OVERRIDE_PURE_NAME_BOOL(name, func, ...)                    \
    constexpr uint64_t nb_hash = nanobind::detail::str_hash(name);          \
    nanobind::detail::ticket nb_ticket(nb_trampoline, name, nb_hash, true); \
    return nanobind::detail::ostkTruthValue(nb_trampoline.base().attr(nb_ticket.key)(__VA_ARGS__))

#endif
