# defensive_cpp
A collection of compile-time validation rules to harden your classes.

Compile-time errors with nice error messages to prevent silently loosing implicitly-declared constructors or default constructors, to enforce the rule of 5, to ensure a type is fast inside a vector, enforce move only, etc.

See [Easy Defensive Programming](https://philippegroarke.com/posts/2018/easy_defensive_programming/) for an introduction to the concept and its beginnings. [Easy Defensive Programming - Up To Eleven](https://philippegroarke.com/posts/2018/easy_defensive_programming_2/) for a follow-up post about how the library is implemented and its limitations.

## Build Instructions
`mkdir build && cd build`

### Windows
`..\conan.bat && cmake .. -A x64 && cmake --build .`
or
`cmake .. -A x64 -DBUILD_TESTING=Off && cmake --build .`

### Unixes
`../conan.sh && cmake .. && cmake --build .`
or
`cmake .. -DBUILD_TESTING=Off && cmake --build .`

## Usage
Include `<defensive/defensive.hpp>` and apply a rule macro after your class.

```cpp
struct example {
	~example() {
	}
	example(const example&) {
	}
	example(example&&) {
	}
	example& operator=(const example&) {
	}
	example& operator=(example&&) {
	}
};

FEA_FULFILLS_RULE_OF_5(example);	// Passes
```

```cpp
struct example {
	~example() {
	}
	example(const example&) {
	}
	example(example&&) {
	}
	example& operator=(const example&) = default;
	example& operator=(example&&) = default;
};

FEA_FULFILLS_RULE_OF_5(example);	// Fails

/**
 * Failure error messages:
 * - test1 : doesn't fulfill rule of 5
 * - test1 : must implement user-defined move assignement operator
 * - test1 : must implement user-defined copy assignement operator
 */
```

## Rules and Examples
More examples can be found in `tests/main.cpp`.

### FEA_FULFULLS_RULE_OF_5
Makes sure a class fulfulls Rule of 5. All 5 constructors/operators are present (destructor, copy constructor, move constructor, copy assignement operator, move assignement operator). If you implement 1 custom constructor/destructor/operator, you need to implement all of them.

There is a withstanding C++ bug that prevents perfect execution of this rule. If you start by implementing a custom destructor, then it is impossible to detect whether the copy constructor and move constructor are user-defined or generated.

MSVC sometimes has issues, use `= default;` to fix those.

#### Example
```cpp
struct pass {
	int t = 42;
};
FEA_FULFILLS_RULE_OF_5(pass); // PASS

struct fail {
	fail& operator=(const fail&) {}
	
	int t = 42;
};
FEA_FULFILLS_RULE_OF_5(fail); // FAIL
```

### FEA_FULFULLS_5_CTORS
Makes sure 5 constructors/operators are present (destructor, copy constructor, move constructor, copy assignement constructor, move assignement constructor). Useful when using rule of 0 or when using `= default`.

#### Example
```cpp
struct pass {
	int t = 42;
};
FEA_FULFILLS_5_CTORS(pass); // PASS

struct fail {
	fail(const fail&) = delete;
	
	int t = 42;
};
FEA_FULFILLS_5_CTORS(fail); // FAIL
```

### FEA_FULFULLS_RULE_OF_6
Rule of 5 with an extra check to make sure your class has a default constructor. All 6 constructors/operators are present (default constructor, destructor, copy constructor, move constructor, copy assignement operator, move assignement operator). If you implement 1 of 5 custom constructor/destructor/operator, you need to implement all of them (destructor, copy constructor, move constructor, copy assignement operator, move assignement operator).

#### Example
```cpp
struct pass {
	int t = 42;
};
FEA_FULFILLS_RULE_OF_6(pass); // PASS

struct fail {
	fail(int) {}
	
	int t = 42;
};
FEA_FULFILLS_RULE_OF_6(fail); // FAIL
```

### FEA_FULFULLS_6_CTORS
Make sure all 6 constructors/operators are present (default, destructor, copy constructor, move constructor, copy assignement operator, move assignement operator). Useful when using rule of 0 or `= default`.

#### Example
```cpp
struct pass {
	int t = 42;
};
FEA_FULFILLS_6_CTORS(pass); // PASS

struct fail {
	fail() = delete;
	
	int t = 42;
};
FEA_FULFILLS_6_CTORS(fail); // FAIL
```

### FEA_FULFILLS_FAST_VECTOR
Ensures your class is optimized to be stored in a vector. Checks whether it is trivially destructible (skips destructor call on resize) and trivially copy constructible (use memcpy on resize). If not, falls back to ensure your class is noexcept move constructible (vector resize cannot use your move constructor if it isn't marked noexcept).

#### Example
```cpp
struct pass {
	int t = 42;
};
FEA_FULFILLS_FAST_VECTOR(pass); // PASS

struct fail {
	~fail() {}
	fail(fail&&) {}
	
	int t = 42;
};
FEA_FULFILLS_FAST_VECTOR(fail); // FAIL
```

### FEA_FULFILLS_MOVE_ONLY
Ensures your class is move only. There is no copy constructor and no copy assignement operator. The move constructor and move assignement operator is present.

#### Example
```cpp
struct pass {
	pass(const pass&) = delete;
	pass& operator=(const pass&) = delete;
	int t = 42;
};
FEA_FULFILLS_MOVE_ONLY(pass); // PASS

struct fail {
	int t = 42;
};
FEA_FULFILLS_MOVE_ONLY(fail); // FAIL
```

### FEA_FULFILLS_NON_CONSTRUCTIBLE
Ensures your class is non constructible, aka had no default constructor, destructor, copy constructor, move constructor, copy assignement operator and move assignement operator. Useful when writing static singleton classes.

#### Example
```cpp
struct pass {
	pass() = delete;
	~pass() = delete;
	pass(const pass&) = delete;
	pass(pass&&) = delete;
	pass& operator=(const pass&) = delete;
	pass& operator=(pass&&) = delete;
	int t = 42;
};
FEA_FULFILLS_NON_CONSTRUCTIBLE(pass); // PASS

struct fail {
	int t = 42;
};
FEA_FULFILLS_NON_CONSTRUCTIBLE(fail); // FAIL
```
