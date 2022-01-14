# Versions >= 3.9 have improved support for MPI: https://cliutils.gitlab.io/modern-cmake/chapters/packages/MPI.html
# The command add_compile_definitions was new in CMake version 3.12.
# file(CREATE_LINK) is new in CMake 3.14
cmake_minimum_required(VERSION 3.14)

# Comment out the next line when speed is needed
set(CMAKE_BUILD_TYPE Debug)
#add_definitions(-O2)

set(EXECUTABLE tests)

project(${EXECUTABLE} LANGUAGES CXX)

# Tell "make" to print out the commands used for compiling and linking:
set(CMAKE_VERBOSE_MAKEFILE on)

include_directories(include)
include_directories(external/doctest)

# Get a list of all sources:
file(GLOB TEST_SOURCES tests/*.cpp)
message("Test sources: ${TEST_SOURCES}")
add_executable(${EXECUTABLE} ${TEST_SOURCES})

# We need the c++11 standard for doctest
set_property(TARGET ${EXECUTABLE} PROPERTY CXX_STANDARD 11)

# Make links to test data file in the build directory
file(GLOB TEST_DATA tests/data/*)
message("Test data: ${TEST_DATA}")
foreach(DATA_FILE "${TEST_DATA}")
  message("Creating symlink for test data file ${DATA_FILE}")
  get_filename_component(FILENAME ${DATA_FILE} NAME)
  file(CREATE_LINK ${DATA_FILE} ${FILENAME} SYMBOLIC)
endforeach()
