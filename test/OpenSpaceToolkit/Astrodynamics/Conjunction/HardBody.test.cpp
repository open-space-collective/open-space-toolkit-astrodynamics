/// Apache License 2.0

#include <cmath>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/2D/Object/Point.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;

using ostk::mathematics::geometry::d2::object::Point;

using ostk::astrodynamics::conjunction::HardBody;

class OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_CrossSection : public ::testing::Test
{
   protected:
    // Circle of radius 1.0
    const HardBody::CrossSection circle_ = {Array<Point>({{0.0, 0.0}}), 1.0};

    // Square of side 1.0
    const HardBody::CrossSection square_ = {Array<Point>({{0.5, 0.5}, {0.5, -0.5}, {-0.5, -0.5}, {-0.5, 0.5}}), 0.0};

    // Stadium: segment of length 2.0 dilated by a radius of 0.5
    const HardBody::CrossSection stadium_ = {Array<Point>({{-1.0, 0.0}, {1.0, 0.0}}), 0.5};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_CrossSection, Constructor)
{
    {
        EXPECT_NO_THROW(HardBody::CrossSection(Array<Point>({{0.0, 0.0}}), 1.0));
    }

    {
        EXPECT_NO_THROW(HardBody::CrossSection(Array<Point>({{0.5, 0.5}, {0.5, -0.5}, {-0.5, -0.5}, {-0.5, 0.5}}), 0.0)
        );
    }

    {
        EXPECT_NO_THROW(HardBody::CrossSection(Array<Point>({{-1.0, 0.0}, {1.0, 0.0}}), 0.5));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_CrossSection, GetEnclosingRadius)
{
    {
        EXPECT_DOUBLE_EQ(1.0, circle_.getEnclosingRadius());
    }

    {
        EXPECT_DOUBLE_EQ(std::sqrt(0.5), square_.getEnclosingRadius());
    }

    {
        EXPECT_DOUBLE_EQ(1.5, stadium_.getEnclosingRadius());
    }

    // Point at the origin with no dilation
    {
        const HardBody::CrossSection crossSection = {Array<Point>({{0.0, 0.0}}), 0.0};

        EXPECT_DOUBLE_EQ(0.0, crossSection.getEnclosingRadius());
    }

    // Convex hull offset from the origin: the enclosing circle is still centered at the origin
    {
        const HardBody::CrossSection crossSection = {Array<Point>({{2.0, 0.0}, {3.0, 0.0}, {3.0, 1.0}}), 0.5};

        EXPECT_DOUBLE_EQ(std::sqrt(10.0) + 0.5, crossSection.getEnclosingRadius());
    }

    // Enclosing radius does not depend on the vertex starting point
    {
        const HardBody::CrossSection crossSection = {
            Array<Point>({{-0.5, -0.5}, {-0.5, 0.5}, {0.5, 0.5}, {0.5, -0.5}}), 0.0
        };

        EXPECT_DOUBLE_EQ(square_.getEnclosingRadius(), crossSection.getEnclosingRadius());
    }

    {
        const HardBody::CrossSection crossSection = {Array<Point>::Empty(), 1.0};

        EXPECT_THROW(
            try { crossSection.getEnclosingRadius(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Convex hull} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_CrossSection, Combine)
{
    // Circle + circle: circle with the sum of the radii
    {
        const HardBody::CrossSection anotherCircle = {Array<Point>({{0.0, 0.0}}), 2.0};

        const HardBody::CrossSection crossSection = HardBody::CrossSection::combine(circle_, anotherCircle);

        EXPECT_DOUBLE_EQ(3.0, crossSection.getEnclosingRadius());
        EXPECT_EQ(
            crossSection.getEnclosingRadius(),
            HardBody::CrossSection::combine(anotherCircle, circle_).getEnclosingRadius()
        );
    }

    // Circle + square: square with rounded corners
    {
        const HardBody::CrossSection crossSection = HardBody::CrossSection::combine(circle_, square_);

        EXPECT_DOUBLE_EQ(std::sqrt(0.5) + 1.0, crossSection.getEnclosingRadius());
    }

    // Square + square: square of side 2.0
    {
        const HardBody::CrossSection crossSection = HardBody::CrossSection::combine(square_, square_);

        EXPECT_DOUBLE_EQ(std::sqrt(2.0), crossSection.getEnclosingRadius());
    }

    // Square + stadium: rectangle of size 3.0 x 1.0 dilated by 0.5
    {
        const HardBody::CrossSection crossSection = HardBody::CrossSection::combine(square_, stadium_);

        EXPECT_DOUBLE_EQ(std::sqrt(1.5 * 1.5 + 0.5 * 0.5) + 0.5, crossSection.getEnclosingRadius());
    }

    // Stadium + perpendicular stadium: square of side 2.0 dilated by 1.0
    {
        const HardBody::CrossSection perpendicularStadium = {Array<Point>({{0.0, -1.0}, {0.0, 1.0}}), 0.5};

        const HardBody::CrossSection crossSection = HardBody::CrossSection::combine(stadium_, perpendicularStadium);

        EXPECT_DOUBLE_EQ(std::sqrt(2.0) + 1.0, crossSection.getEnclosingRadius());
    }

    // Stadium + stadium: collinear vertex sums collapse into a segment of length 4.0 dilated by 1.0
    {
        const HardBody::CrossSection crossSection = HardBody::CrossSection::combine(stadium_, stadium_);

        EXPECT_DOUBLE_EQ(3.0, crossSection.getEnclosingRadius());
    }

    // Offset convex hulls: offsets add up
    {
        const HardBody::CrossSection aCrossSection = {Array<Point>({{1.0, 0.0}}), 0.0};
        const HardBody::CrossSection anotherCrossSection = {Array<Point>({{0.0, 2.0}, {0.0, 3.0}}), 0.0};

        const HardBody::CrossSection crossSection = HardBody::CrossSection::combine(aCrossSection, anotherCrossSection);

        EXPECT_DOUBLE_EQ(std::sqrt(10.0), crossSection.getEnclosingRadius());
    }

    {
        const HardBody::CrossSection emptyCrossSection = {Array<Point>::Empty(), 1.0};

        EXPECT_THROW(
            try {
                HardBody::CrossSection::combine(emptyCrossSection, circle_);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Convex hull} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            HardBody::CrossSection::combine(circle_, emptyCrossSection), ostk::core::error::runtime::Undefined
        );
    }
}
