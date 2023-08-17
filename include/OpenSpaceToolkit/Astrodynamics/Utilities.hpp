/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Utilities__
#define __OpenSpaceToolkit_Astrodynamics_Utilities__

#include <magic_enum.hpp>
#include <stdexcept>

#include <OpenSpaceToolkit/Core/Error.hpp>

#define DEFINE_ENUM_CONVERSIONS(EnumClass)                                 \
    inline static std::string StringFrom##EnumClass(EnumClass value)       \
    {                                                                      \
        return magic_enum::enum_name(value).data();                        \
    }                                                                      \
                                                                           \
    inline static EnumClass EnumClass##FromString(const std::string& name) \
    {                                                                      \
        auto opt_val = magic_enum::enum_cast<EnumClass>(name);             \
        if (opt_val.has_value())                                           \
        {                                                                  \
            return opt_val.value();                                        \
        }                                                                  \
        else                                                               \
        {                                                                  \
            throw std::invalid_argument("Invalid enum name: " + name);     \
        }                                                                  \
    }

#endif
