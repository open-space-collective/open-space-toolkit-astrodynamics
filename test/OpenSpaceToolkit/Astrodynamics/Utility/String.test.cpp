/// Apache License 2.0

#include <limits>

#include <OpenSpaceToolkit/Astrodynamics/Utility/String.hpp>

#include <Global.test.hpp>

using ostk::astrodynamics::utility::DoubleToString;

TEST(OpenSpaceToolkit_Astrodynamics_Utility_String, DoubleToString)
{
    {
        EXPECT_EQ("1.0", DoubleToString(1.0));
        EXPECT_EQ("-2.0", DoubleToString(-2.0));
        EXPECT_EQ("0.5", DoubleToString(0.5));
        EXPECT_EQ("1.23", DoubleToString(1.23));
        EXPECT_EQ("+Inf", DoubleToString(std::numeric_limits<double>::infinity()));
        EXPECT_EQ("-Inf", DoubleToString(-std::numeric_limits<double>::infinity()));
    }

    {
        // NaN is printed as such: there is no undefined double
        EXPECT_EQ("NaN", DoubleToString(std::numeric_limits<double>::quiet_NaN()));
    }
}
