/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Utility_String__
#define __OpenSpaceToolkit_Astrodynamics_Utility_String__

#include <cmath>
#include <string>

#include <boost/lexical_cast.hpp>

#include <OpenSpaceToolkit/Core/Type/String.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace utility
{

using ostk::core::type::String;

/// @brief Convert a double to a string
///
/// Internal helper. The output matches the former ostk::core::type::Real::toString() output, except that NaN prints as
/// "NaN" (there is no undefined double): infinities print as "+Inf" / "-Inf", integral values keep a trailing ".0", and
/// other values use the round-trip representation without trailing zeros.
///
/// @code{.cpp}
///     DoubleToString(1.0) ; // "1.0"
///     DoubleToString(1.23) ; // "1.23"
/// @endcode
///
/// @param [in] aValue A value
/// @return String representation
inline String DoubleToString(double aValue)
{
    if (std::isnan(aValue))
    {
        return "NaN";
    }

    if (std::isinf(aValue))
    {
        return (aValue > 0.0) ? "+Inf" : "-Inf";
    }

    std::string string = boost::lexical_cast<std::string>(aValue);

    if (string.find('e') == std::string::npos)
    {
        double integralPart;

        if (std::modf(aValue, &integralPart) == 0.0)
        {
            string += ".0";
        }
        else
        {
            string.erase(string.find_last_not_of('0') + 1, std::string::npos);  // Remove trailing zeros if any
        }
    }

    return string;
}

}  // namespace utility
}  // namespace astrodynamics
}  // namespace ostk

#endif
