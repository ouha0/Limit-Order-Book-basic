# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(LimitOrderBook)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the name of the executable
set(EXECUTABLE_NAME lob_simulator)

# Tell CMake where to find our header files
include_directories(include)

# Find all our source files
file(GLOB SOURCES "src/*.cpp")

# Add the executable target, linking our source files
add_executable(${EXECUTABLE_NAME} ${SOURCES})
