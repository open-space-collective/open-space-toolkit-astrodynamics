/// Apache License 2.0

#ifndef __OpenSpaceToolkitPy_Utility_BooleanFunctionCasting__
#define __OpenSpaceToolkitPy_Utility_BooleanFunctionCasting__

#include <functional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>

/// @brief                      Accept any number as the result of a python predicate.
///
///                             nanobind's bool caster takes `True` and `False` and nothing else, so a
///                             callback such as `lambda state: state.get_coordinates()[0] > 0.0`,
///                             which yields `numpy.bool_`, would fail where C++ expects a
///                             `std::function<bool(...)>`. pybind11 accepted it, and predicates are
///                             the toolkit's main extension point (access filters, event condition
///                             evaluators), so the behaviour is restored here.
///
///                             Only objects implementing the number protocol are converted, which
///                             leaves strings, containers and None rejected. This specialization is
///                             more specialized than nanobind's own `std::function` caster, so it
///                             takes precedence for bool-returning predicates only.

namespace nanobind
{
namespace detail
{

template <typename... Args>
struct type_caster<std::function<bool(Args...)>>
{
    NB_TYPE_CASTER(
        std::function<bool(Args...)>,
        const_name(NB_TYPING_CALLABLE "[[") + concat(make_caster<Args>::Name...) + const_name("], bool]")
    )

    struct PredicateWrapper : pyfunc_wrapper
    {
        using pyfunc_wrapper::pyfunc_wrapper;

        bool operator()(Args... args) const
        {
            gil_scoped_acquire acquire;

            // `operator()` already returns an owning reference, and raises on failure.

            object result = handle(f)((forward_t<Args>)args...);

            if (result.ptr() == Py_True)
            {
                return true;
            }

            if (result.ptr() == Py_False)
            {
                return false;
            }

            if (PyNumber_Check(result.ptr()) == 0)
            {
                throw cast_error();
            }

            const int truth = PyObject_IsTrue(result.ptr());

            if (truth == -1)
            {
                raise_python_error();
            }

            return truth == 1;
        }
    };

    bool from_python(handle aSource, uint8_t someFlags, cleanup_list*) noexcept
    {
        if (aSource.is_none())
        {
            return someFlags & static_cast<uint8_t>(cast_flags::convert);
        }

        if (!PyCallable_Check(aSource.ptr()))
        {
            return false;
        }

        value = PredicateWrapper(aSource.ptr());

        return true;
    }

    static handle from_cpp(const Value& aFunction, rv_policy aPolicy, cleanup_list*) noexcept
    {
        // Hand back the original python object when there is one, as nanobind's caster does.

        if (const PredicateWrapper* wrapper = aFunction.template target<PredicateWrapper>())
        {
            return handle(wrapper->f).inc_ref();
        }

        if (aPolicy == rv_policy::none)
        {
            return handle();
        }

        if (!aFunction)
        {
            return none().release();
        }

        return cpp_function(aFunction).release();
    }
};

}  // namespace detail
}  // namespace nanobind

#endif
