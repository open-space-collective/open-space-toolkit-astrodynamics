/// Apache License 2.0

#ifndef __OpenSpaceToolkitAstrodynamicsPy_Utility_Hash__
#define __OpenSpaceToolkitAstrodynamicsPy_Utility_Hash__

#include <nanobind/nanobind.h>

/// @brief                      Make the classes that define `__eq__` unhashable, recursively
///
///                             Python sets `__hash__ = None` on a class whose body defines `__eq__` without
///                             `__hash__`, and pybind11 did the same for bound classes. nanobind adds methods after
///                             the type is created, so its classes keep `object.__hash__` (identity), which breaks
///                             the hash / equality contract for these value types. This restores pybind11's
///                             behaviour for every nanobind class reachable from the given scope.

inline void disableHashOnEquality(nanobind::handle aScope)
{
    using namespace nanobind;

    // A module's __dict__ is a dict; a type's is a read-only proxy of its tp_dict.
    const dict scopeDictionary = PyModule_Check(aScope.ptr()) ? borrow<dict>(PyModule_GetDict(aScope.ptr()))
                                                              : borrow<dict>(((PyTypeObject*)aScope.ptr())->tp_dict);

    for (const auto& item : scopeDictionary)
    {
        const handle value = item.second;

        if (PyModule_Check(value.ptr()))
        {
            disableHashOnEquality(value);
        }
        else if (type_check(value))
        {
            const dict typeDictionary = borrow<dict>(((PyTypeObject*)value.ptr())->tp_dict);

            if (typeDictionary.contains("__eq__") && !typeDictionary.contains("__hash__"))
            {
                setattr(value, "__hash__", none());
            }

            disableHashOnEquality(value);
        }
    }
}

#endif
