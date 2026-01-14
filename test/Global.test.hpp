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

#define EXPECT_INSTANTS_ALMOST_EQUAL(i1, i2, tolerance)                                                              \
    do                                                                                                               \
    {                                                                                                                \
        EXPECT_TRUE((i1).isNear((i2), (tolerance)))                                                                 \
            << "Expected " << (i1).toString() << " to be near " << (i2).toString()                                  \
            << " with tolerance " << (tolerance).toString()                                                         \
            << " but was " << ((i1) - (i2)).toString();                                                             \
    } while (0)

#define ASSERT_INSTANTS_ALMOST_EQUAL(i1, i2, tolerance)                                                             \
    do                                                                                                              \
    {                                                                                                               \
        ASSERT_TRUE((i1).isNear((i2), (tolerance)))                                                                \
            << "Expected " << (i1).toString() << " to be near " << (i2).toString()                                 \
            << " with tolerance " << (tolerance).toString()                                                        \
            << " but was " << ((i1) - (i2)).toString();                                                            \
    } while (0)

#define EXPECT_DURATIONS_ALMOST_EQUAL(d1, d2, tolerance)                                                            \
    do                                                                                                              \
    {                                                                                                               \
        EXPECT_TRUE((d1).isNear((d2), (tolerance)))                                                                \
            << "Expected " << (d1).toString() << " to be near " << (d2).toString()                                 \
            << " with tolerance " << (tolerance).toString()                                                        \
            << " but was " << ((d1) - (d2)).toString();                                                            \
    } while (0)

#define ASSERT_DURATIONS_ALMOST_EQUAL(d1, d2, tolerance)                                                            \
    do                                                                                                              \
    {                                                                                                               \
        ASSERT_TRUE((d1).isNear((d2), (tolerance)))                                                                \
            << "Expected " << (d1).toString() << " to be near " << (d2).toString()                                 \
            << " with tolerance " << (tolerance).toString()                                                        \
            << " but was " << ((d1) - (d2)).toString();                                                            \
    } while (0)

#define EXPECT_INTERVALS_ALMOST_EQUAL(i1, i2, tolerance)                                  \
    do                                                                                    \
    {                                                                                     \
        EXPECT_INSTANTS_ALMOST_EQUAL((i1).getStart(), (i2).getStart(), tolerance);        \
        EXPECT_INSTANTS_ALMOST_EQUAL((i1).getEnd(), (i2).getEnd(), tolerance);            \
    } while (0)

#define EXPECT_STATES_ALMOST_EQUAL(s1, s2, instantTolerance, positionTolerance, velocityTolerance)                      \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((s1).getFrame() != (s2).getFrame())                                                                        \
        {                                                                                                              \
            ADD_FAILURE() << "States have different frames.";                                                          \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            EXPECT_INSTANTS_ALMOST_EQUAL((s1).getInstant(), (s2).getInstant(), instantTolerance);                      \
            const auto pos1 = (s1).getPosition().inMeters().accessCoordinates();                     \
            const auto pos2 = (s2).getPosition().inUnit(Position::Unit::Meter).accessCoordinates();                     \
            EXPECT_TRUE(pos1.isNear(pos2, positionTolerance))                                                          \
                << "Positions differ. Values: " << pos1.toString() << " vs. " << pos2.toString();                      \
            const auto vel1 = (s1).getVelocity().inMetersPerSecond().accessCoordinates();           \
            const auto vel2 = (s2).getVelocity().inMetersPerSecond().accessCoordinates();           \
            EXPECT_TRUE(vel1.isNear(vel2, velocityTolerance))                                                          \
                << "Velocities differ. Values: " << vel1.toString() << " vs. " << vel2.toString();                     \
        }                                                                                                              \
    } while (0)

#define ASSERT_STATES_ALMOST_EQUAL(s1, s2, instantTolerance, positionTolerance, velocityTolerance)                    \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((s1).getFrame() != (s2).getFrame())                                                                        \
        {                                                                                                              \
            FAIL() << "States have different frames.";                                                                 \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            ASSERT_INSTANTS_ALMOST_EQUAL((s1).getInstant(), (s2).getInstant(), instantTolerance);                       \
            const auto pos1 = (s1).getPosition().inUnit(Position::Unit::Meter).accessCoordinates();                     \
            const auto pos2 = (s2).getPosition().inUnit(Position::Unit::Meter).accessCoordinates();                     \
            ASSERT_TRUE(pos1.isNear(pos2, positionTolerance))                                                          \
                << "Positions differ. Values: " << pos1.toString() << " vs. " << pos2.toString();                      \
            const auto vel1 = (s1).getVelocity().inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();           \
            const auto vel2 = (s2).getVelocity().inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();           \
            ASSERT_TRUE(vel1.isNear(vel2, velocityTolerance))                                                          \
                << "Velocities differ. Values: " << vel1.toString() << " vs. " << vel2.toString();                     \
        }                                                                                                              \
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
