#include "defensive/defensive.hpp"
#include <cstdio>
#include <gtest/gtest.h>

struct test_me {
	test_me() = default;
	~test_me() = delete;
	test_me(const test_me&) = default;
	test_me(test_me&&) = default;
	test_me& operator=(const test_me&) = default;
	test_me& operator=(test_me&&) = default;
};
FEA_GENERATED_5_CTORS(test_me);

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
