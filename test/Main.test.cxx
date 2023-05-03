// Copyright © Loft Orbital Solutions Inc.

#include <Setup.test.hpp>

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    ::testing::AddGlobalTestEnvironment(new ostk::astro::test::Environment(argc >= 2 ? argv[1] : ""));

    return RUN_ALL_TESTS();
}
