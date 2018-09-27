#include "defensive/defensive.hpp"
#include <cstdio>
#include <gtest/gtest.h>

namespace {

// struct test1 {
//	~test1() {
//	}
//	test1(const test1&) = default;
//	test1(test1&&) {
//	}
//	test1& operator=(const test1&) {
//	}
//	test1& operator=(test1&&) {
//	}
//};
// FEA_FULFILLS_5_CTORS(test1);
// FEA_FULFILLS_RULE_OF_5(test1);

struct test2 {
	~test2() = default;
	test2(const test2&) = default;
	test2(test2&&) = default;
	test2& operator=(const test2&) = default;
	test2& operator=(test2&&) = default;
};
// FEA_FULFILLS_5_CTORS(test2);
FEA_FULFILLS_RULE_OF_5(test2);

constexpr bool b = detail::trivial_checker<test2>::all_non_trivial();
constexpr bool b2 = detail::trivial_checker<test2>::all_trivial();

constexpr bool failed = FEA_GENERATED_5_IMPL(test2)
		&& !(detail::trivial_checker<test2>::all_trivial()
				   || detail::trivial_checker<test2>::all_non_trivial());

} // namespace

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
