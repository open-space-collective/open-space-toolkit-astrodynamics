/// Apache License 2.0

#include <gtest/gtest.h>

#define EXPECT_VECTORS_ALMOST_EQUAL(v1, v2, tolerance)                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((v1).size() != (v2).size())                                                                                \
        {                                                                                                              \
            ADD_FAILURE() << "Vectors have different sizes.";                                                          \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            for (int i = 0; i < (v1).size(); ++i)                                                                      \
            {                                                                                                          \
                if (std::abs((v1)(i) - (v2)(i)) > (tolerance))                                                         \
                {                                                                                                      \
                    ADD_FAILURE() << "Vectors differ at index " << i << ". Values: " << (v1)(i) << " vs. " << (v2)(i); \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define ASSERT_VECTORS_ALMOST_EQUAL(v1, v2, tolerance)                                                          \
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

#define EXPECT_MATRICES_ALMOST_EQUAL(m1, m2, tolerance)                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((m1).rows() != (m2).rows() || (m1).cols() != (m2).cols())                                                  \
        {                                                                                                              \
            ADD_FAILURE() << "Matrices have different sizes.";                                                         \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            for (int i = 0; i < (m1).rows(); ++i)                                                                      \
            {                                                                                                          \
                for (int j = 0; j < (m1).cols(); ++j)                                                                  \
                {                                                                                                      \
                    if (std::abs((m1)(i, j) - (m2)(i, j)) > (tolerance))                                               \
                    {                                                                                                  \
                        ADD_FAILURE() << "Matrices differ at index (" << i << ", " << j << "). Values: " << (m1)(i, j) \
                                      << " vs. " << (m2)(i, j);                                                        \
                    }                                                                                                  \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define ASSERT_MATRICES_ALMOST_EQUAL(m1, m2, tolerance)                                                         \
    do                                                                                                          \
    {                                                                                                           \
        if ((m1).rows() != (m2).rows() || (m1).cols() != (m2).cols())                                           \
        {                                                                                                       \
            FAIL() << "Matrices have different sizes.";                                                         \
        }                                                                                                       \
        else                                                                                                    \
        {                                                                                                       \
            for (int i = 0; i < (m1).rows(); ++i)                                                               \
            {                                                                                                   \
                for (int j = 0; j < (m1).cols(); ++j)                                                           \
                {                                                                                               \
                    if (std::abs((m1)(i, j) - (m2)(i, j)) > (tolerance))                                        \
                    {                                                                                           \
                        FAIL() << "Matrices differ at index (" << i << ", " << j << "). Values: " << (m1)(i, j) \
                               << " vs. " << (m2)(i, j);                                                        \
                    }                                                                                           \
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
