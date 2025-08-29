# minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(LimitOrderBook)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Compiler Settings ---
# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Project Configuration ---
# Define the name of the executable
set(EXECUTABLE_NAME lob)

# Compiler header files
include_directories(include)

# --- Build Target ---
# This is more robust than using file(GLOB).
add_executable(${EXECUTABLE_NAME}
    src/main.cpp
    src/LimitOrderBook.cpp
    src/Parser.cpp
)
