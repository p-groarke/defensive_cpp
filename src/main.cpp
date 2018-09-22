#include "defensive.hpp"

struct test_me {
	test_me() = default;
	~test_me() = default;
	test_me(const test_me&) = default;
	test_me(test_me&&) = default;
	test_me& operator=(const test_me&) = default;
	test_me& operator=(test_me&&) = default;
};
FEA_FULFILLS_RULE_OF_5(test_me);
FEA_FULFILLS_RULE_OF_6(test_me);
FEA_FULFILLS_FAST_VECTOR(test_me);
// FEA_FULFILLS_MOVE_ONLY(test_me);

int main(int, char**) {
	return 0;
}
