# defensive_cpp
A collection of macros to protect your classes from silently loosing implicitly-declared constructors or to enforce rule of 5/3.

## Build Instructions
`mkdir build && cd build`

### Windows
`..\conan.bat && cmake .. -A x64 && cmake --build . && bin\defensive_tests.exe`

### Unixes
`../conan.sh && cmake .. && cmake --build . && bin/defensive_tests`