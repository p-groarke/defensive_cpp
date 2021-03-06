cmake_minimum_required (VERSION 3.10)
project(defensive_cpp VERSION 1.0.0 LANGUAGES CXX)

include(GoogleTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Require up-to-date compiler.
set(CMAKE_CXX_STANDARD 11)

# Dependency directories
set(LIB_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)

# Organize unrelated targets to clean IDE hierarchy.
set(DEPENDENCY_FOLDER "Dependencies")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ${DEPENDENCY_FOLDER})

# Output binary to predictable location (fixes cyclic dependency issues).
set(BINARY_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# Conan things.
option(BUILD_TESTING "Build and run tests." Off)
if (${BUILD_TESTING})
	include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
	conan_basic_setup(TARGETS)
endif()


# Libs
file(GLOB_RECURSE LIB_HEADERS "include/*.hpp" "include/*.h" "include/*.tpp")
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${LIB_HEADERS}>
)
target_include_directories(${PROJECT_NAME} INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.clang-format ${CMAKE_CURRENT_BINARY_DIR}/.clang-format COPYONLY)

##
# Tests
##
if (${BUILD_TESTING})
	enable_testing()

	set(TEST_NAME ${PROJECT_NAME}_tests)
	file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp" "tests/*.c" "tests/*.hpp" "tests/*.h" "tests/*.tpp")
	add_executable(${TEST_NAME} ${TEST_SOURCES})
	target_include_directories(${TEST_NAME} INTERFACE ${LIB_INCLUDE_DIR})
	target_link_libraries(${TEST_NAME} ${PROJECT_NAME} CONAN_PKG::gtest)

	gtest_discover_tests(${TEST_NAME})

	if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
		# using Intel C++
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TEST_NAME})
		target_compile_options(${TEST_NAME} PRIVATE /W4 /WX /EHsc /Zc:__cplusplus /permissive-)
		add_definitions(-DNOMINMAX)
	endif()
endif()

# Install Package Configuration
set(DEFENSIVE_INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/defensive")
install(FILES ${LIB_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/defensive")

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/defensiveConfigVersion.cmake"
	VERSION ${${PROJECT_NAME}_VERSION}
	COMPATIBILITY AnyNewerVersion
	)

configure_package_config_file(
	cmake/defensiveConfig.cmake.in
	${DEFENSIVE_INSTALL_CONFIGDIR}/defensiveConfig.cmake
	INSTALL_DESTINATION ${DEFENSIVE_INSTALL_CONFIGDIR}
	PATH_VARS CMAKE_INSTALL_INCLUDEDIR
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
	)

install(
	FILES
	"${CMAKE_CURRENT_BINARY_DIR}/${DEFENSIVE_INSTALL_CONFIGDIR}/defensiveConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/defensiveConfigVersion.cmake"
	DESTINATION ${DEFENSIVE_INSTALL_CONFIGDIR}
	)

install(TARGETS ${PROJECT_NAME} EXPORT defensiveTargets)

export(EXPORT defensiveTargets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/defensiveTargets.cmake"
	)

install(
	EXPORT defensiveTargets FILE defensiveTargets.cmake
	DESTINATION ${DEFENSIVE_INSTALL_CONFIGDIR}
	)

export(PACKAGE ${PROJECT_NAME})
