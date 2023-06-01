/// Apache License 2.0

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>

namespace pybind11
{
namespace detail
{

using ostk::core::ctnr::Array;

template <typename T>
struct type_caster<Array<T>> : list_caster<Array<T>, T>
{
};

// using list_caster::load ;
// using list_caster::cast ;

}  // namespace detail
}  // namespace pybind11
