/// Apache License 2.0

#ifndef __OpenSpaceToolkitAstrodynamicsPy_Utilities_MapConverter__
#define __OpenSpaceToolkitAstrodynamicsPy_Utilities_MapConverter__

// https://stackoverflow.com/questions/42952781/how-to-wrap-a-c-class-with-a-constructor-that-takes-a-stdmap-or-stdvector
// https://stackoverflow.com/questions/6116345/boostpython-possible-to-automatically-convert-from-dict-stdmap
// https://stackoverflow.com/questions/15842126/feeding-a-python-list-into-a-function-taking-in-a-vector-with-boost-python
// https://misspent.wordpress.com/2009/09/27/how-to-write-boost-python-converters/
// https://www.boost.org/doc/libs/1_39_0/libs/python/doc/v2/faq.html#custom_string

template <typename Container>
struct ToDictConverter
{
    static PyObject* convert(const Container& aContainer)
    {
        boost::python::dict dict;

        for (const auto& element : aContainer)
        {
            dict[element.first] = element.second;
        }

        return boost::python::incref(dict.ptr());
    }
};

struct MapConverter
{
    /// @brief                  Registers converter from a python iterable type to the provided type

    template <typename Container>
    MapConverter& from_python()
    {
        boost::python::converter::registry::push_back(
            &MapConverter::convertible, &MapConverter::construct<Container>, boost::python::type_id<Container>()
        );

        return *this;
    }

    /// @brief                  Registers converter from the provided type to a python iterable type

    template <typename Container>
    MapConverter& to_python()
    {
        boost::python::to_python_converter<Container, ToDictConverter<Container>>();

        return *this;
    }

    /// @brief                  Check if PyObject is iterable

    static void* convertible(PyObject* anObject)
    {
        auto* iterator = PyObject_GetIter(anObject);

        if (iterator != nullptr)
        {
            boost::python::decref(iterator);
            return anObject;
        }

        return nullptr;
    }

    /// @brief Convert iterable PyObject to C++ container type.
    ///
    /// Container Concept requirements:
    ///
    ///   * Container to be constructed is a map.

    template <typename Container>
    static void construct(PyObject* object, boost::python::converter::rvalue_from_python_stage1_data* data)
    {
        namespace python = boost::python;

        // Object is a borrowed reference, so create a handle indicting it is borrowed for proper reference counting

        python::handle<> handle(python::borrowed(object));

        // Obtain a handle to the memory block that the converter has allocated for the C++ type

        typedef python::converter::rvalue_from_python_storage<Container> storage_type;

        void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

        // Allocate the C++ type into the converter's memory block

        new (storage) Container();

        // Iterate over the dictionary and populate the map

        python::dict dict(handle);

        Container& map(*(static_cast<Container*>(storage)));

        python::list keys(dict.keys());
        int keyCount(static_cast<int>(python::len(keys)));

        for (int i = 0; i < keyCount; ++i)
        {
            python::object keyObj(keys[i]);
            python::extract<typename Container::key_type> keyProxy(keyObj);

            if (!keyProxy.check())
            {
                PyErr_SetString(PyExc_KeyError, "Bad key type.");
                python::throw_error_already_set();
            }

            typename Container::key_type key = keyProxy();

            python::object valObj(dict[keyObj]);
            python::extract<typename Container::mapped_type> valProxy(valObj);

            if (!valProxy.check())
            {
                PyErr_SetString(PyExc_ValueError, "Bad value type.");
                python::throw_error_already_set();
            }

            typename Container::mapped_type val = valProxy();

            map.insert({key, val});
        }

        data->convertible = storage;
    }
};

#endif
