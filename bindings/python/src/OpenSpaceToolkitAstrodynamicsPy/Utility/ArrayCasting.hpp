/// Apache License 2.0

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/complex.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unique_ptr.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>

namespace nanobind
{
namespace detail
{

using ostk::core::container::Array;

template <typename T>
struct type_caster<Array<T>> : list_caster<Array<T>, T>
{
};

// using list_caster::load ;
// using list_caster::cast ;

}  // namespace detail
}  // namespace nanobind
