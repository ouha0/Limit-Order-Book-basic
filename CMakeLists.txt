# minimum required version of CMake
cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0135 NEW)
project(LimitOrderBook)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

# --- Project Configuration ---
# Define the name of the executable
set(EXECUTABLE_NAME lob)

# Compiler header files
include_directories(include)


# --- Main Part ---
add_executable(${EXECUTABLE_NAME}
    src/main.cpp
    src/LimitOrderBook.cpp
    src/Parser.cpp
)

# --- Testing ---
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
FetchContent_MakeAvailable(googletest) #Make the google test framework

# Create test executable
add_executable(lob_tests
    tests/LOB_tests.cpp
    src/LimitOrderBook.cpp
)

# Link gtest to test executable
target_link_libraries(lob_tests PRIVATE GTest::gtest_main)

# Tell Ctest how to discover and run these tests
include(GoogleTest)
gtest_discover_tests(lob_tests)
