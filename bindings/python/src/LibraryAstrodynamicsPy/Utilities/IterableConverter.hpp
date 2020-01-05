////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           LibraryAstrodynamicsPy/Utilities/IterableConverter.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __LibraryAstrodynamicsPy_Utilities_IterableConverter__
#define __LibraryAstrodynamicsPy_Utilities_IterableConverter__

#include <boost/python/stl_iterator.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Container>
struct ToListConverter
{

	static PyObject*            convert                                     (   const   Container&                  aContainer                                  )
    {

        boost::python::list list ;

        for (const auto& element : aContainer)
        {
            list.append(element) ;
        }

        return boost::python::incref(list.ptr()) ;

	}

} ;

struct IterableConverter
{

    /// @brief                  Registers converter from a python interable type to the provided type

                                template <typename Container>
    IterableConverter&          from_python                                 ( )
    {

        boost::python::converter::registry::push_back
        (
            &IterableConverter::convertible,
            &IterableConverter::construct<Container>,
            boost::python::type_id<Container>()
        ) ;

        return *this ;

    }

    /// @brief                  Registers converter from the provided type to a python interable type

                                template <typename Container>
    IterableConverter&          to_python                                   ( )
    {

        boost::python::to_python_converter<Container, ToListConverter<Container>>() ;

        return *this ;

    }

    /// @brief                  Check if PyObject is iterable

    static void*                convertible                                 (           PyObject*                   anObject                                    )
    {
        return PyObject_GetIter(anObject) ? anObject : nullptr ;
    }

    /// @brief Convert iterable PyObject to C++ container type.
    ///
    /// Container Concept requirements:
    ///
    ///   * Container::value_type is CopyConstructable.
    ///   * Container can be constructed and populated with two iterators.
    ///     I.e. Container(begin, end)

                                template <typename Container>
    static void                 construct                                   (           PyObject*                   object,
                                                                                        boost::python::converter::rvalue_from_python_stage1_data* data          )
    {

        namespace python = boost::python ;

        // Object is a borrowed reference, so create a handle indicting it is borrowed for proper reference counting

        python::handle<> handle(python::borrowed(object)) ;

        // Obtain a handle to the memory block that the converter has allocated for the C++ type

        typedef python::converter::rvalue_from_python_storage<Container> storage_type ;

        void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes ;

        typedef python::stl_input_iterator<typename Container::value_type> iterator ;

        // Allocate the C++ type into the converter's memory block, and assign
        // its handle to the converter's convertible variable.  The C++
        // container is populated by passing the begin and end iterators of
        // the python object to the container's constructor.

        new (storage) Container
        (
            iterator(python::object(handle)), // begin
            iterator() // end
        ) ;

        data->convertible = storage ;

    }

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
