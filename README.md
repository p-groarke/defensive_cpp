# defensive_cpp
A collection of macros to protect your classes from silently loosing implicitly-declared constructors or to enforce rule of 5, fast vector and move only at compile time.
See [Easy Defensive Programming](https://philippegroarke.com/posts/2018/easy_defensive_programming/) for an introduction to the concept and its beginnings.

## Build Instructions
`mkdir build && cd build`

### Windows
`..\conan.bat && cmake .. -A x64 && cmake --build . && bin\defensive_tests.exe`

### Unixes
`../conan.sh && cmake .. && cmake --build . && bin/defensive_tests`

## Example
More examples can be found in `test/main.cpp`.

```
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

```
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

// Failure error messages:
test1 : doesn't fulfill rule of 5
- test1 : must implement user-defined move assignement operator	defensive_tests
- test1 : must implement user-defined copy assignement operator	defensive_tests
```