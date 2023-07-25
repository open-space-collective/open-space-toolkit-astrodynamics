/// Apache License 2.0

#include <sstream>

/// @brief                      Shift to String Template Function
///
///                             Template function used for __str__ and __repr__
///                             methods on classes exposed in python.

template <class T>
std::string shiftToString(const T& aClass)
{
    std::ostringstream out;
    out << aClass;
    return out.str();
}
