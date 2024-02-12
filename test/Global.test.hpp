/// Apache License 2.0

#include <gtest/gtest.h>

#define EXPECT_VECTORS_ALMOST_EQUAL(v1, v2, tolerance)                                                          \
    do                                                                                                          \
    {                                                                                                           \
        if ((v1).size() != (v2).size())                                                                         \
        {                                                                                                       \
            FAIL() << "Vectors have different sizes.";                                                          \
        }                                                                                                       \
        else                                                                                                    \
        {                                                                                                       \
            for (int i = 0; i < (v1).size(); ++i)                                                               \
            {                                                                                                   \
                if (std::abs((v1)(i) - (v2)(i)) > (tolerance))                                                  \
                {                                                                                               \
                    FAIL() << "Vectors differ at index " << i << ". Values: " << (v1)(i) << " vs. " << (v2)(i); \
                }                                                                                               \
            }                                                                                                   \
        }                                                                                                       \
    } while (0)

namespace ostk
{
namespace astrodynamics
{
namespace test
{
namespace global
{

extern std::string someGlobalString;

}  // namespace global
}  // namespace test
}  // namespace astrodynamics
}  // namespace ostk
