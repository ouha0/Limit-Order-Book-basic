# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(LimitOrderBook)

# --- For Your Editor (Neovim/VSCode/etc.) ---
# This line is crucial! It tells CMake to generate the compile_commands.json
# file, which your editor's C++ language server (clangd) needs to find
# your header files and provide proper autocompletion and error checking.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Compiler Settings ---
# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Project Configuration ---
# Define the name of the executable
set(EXECUTABLE_NAME lob_simulator)

# Tell the compiler where to find our header files (e.g., #include "lob/Order.h")
include_directories(include)

# --- Build Target ---
# Explicitly list all source files to be compiled into the executable.
# This is more robust than using file(GLOB).
add_executable(${EXECUTABLE_NAME}
    src/main.cpp
    src/LimitOrderBook.cpp
)
