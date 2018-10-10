#include "defensive/defensive.hpp"
#include <cstdio>

namespace {
struct test1 {
	~test1() {
	}
	test1(const test1&) {
	}
	test1(test1&&) {
	}
	test1& operator=(const test1&) = default;
	test1& operator=(test1&&) = default;
};
FEA_FULFILLS_5_CTORS(test1);
FEA_FULFILLS_RULE_OF_5(test1);

struct test2 {
	~test2() = default;
	test2(const test2&) = default;
	test2(test2&&) = default;
	test2& operator=(const test2&) = default;
	test2& operator=(test2&&) = default;
};
FEA_FULFILLS_5_CTORS(test2);
FEA_FULFILLS_RULE_OF_5(test2);
FEA_FULFILLS_FAST_VECTOR(test2);

struct test3 {};
FEA_FULFILLS_5_CTORS(test3);
FEA_FULFILLS_RULE_OF_5(test3);
FEA_FULFILLS_FAST_VECTOR(test3);

struct test4 {
	test4() {
	}
	~test4() = default;
	test4(const test4&) = default;
	test4(test4&&) = default;
	test4& operator=(const test4&) = default;
	test4& operator=(test4&&) = default;
};
FEA_FULFILLS_6_CTORS(test4);
FEA_FULFILLS_RULE_OF_6(test4);
FEA_FULFILLS_FAST_VECTOR(test4);

struct test5 {
	test5() = default;
	~test5() {
	}
	test5(const test5&) {
	}
	test5(test5&&) {
	}
	test5& operator=(const test5&) {
	}
	test5& operator=(test5&&) {
	}
};
FEA_FULFILLS_6_CTORS(test5);
FEA_FULFILLS_RULE_OF_6(test5);

struct test6 {};
FEA_FULFILLS_FAST_VECTOR(test6);

struct test7 {
	~test7() = default;
	test7(const test7&) = default;
	test7(test7&&) = delete;
};
FEA_FULFILLS_FAST_VECTOR(test7);

struct test8 {
	test8(const test8&) = delete;
	test8(test8&&) noexcept {
	}
};
FEA_FULFILLS_FAST_VECTOR(test8);

struct test9 {
	~test9() = default;
	test9(const test9&) = delete;
	test9(test9&&) = default;
	test9& operator=(const test9&) = delete;
	test9& operator=(test9&&) = default;
};
FEA_FULFILLS_MOVE_ONLY(test9);

struct test10 {
	test10(test10&&) = default;
	test10& operator=(test10&&) = default;
};
FEA_FULFILLS_MOVE_ONLY(test10);

struct test11 {
	test11(test11&&) {
	}
	test11& operator=(test11&&) {
	}
};
FEA_FULFILLS_MOVE_ONLY(test11);
} // namespace

int main(int, char**) {
	return 0;
}
