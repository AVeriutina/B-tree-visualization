cmake_minimum_required(VERSION 3.27)
set(CMAKE_CXX_COMPILER "clang++-18")

project(B_plus_tree_visualization)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(source)

add_executable(
        B_plus_tree_visualization
        source/BPlusTree.cpp
        source/test.cpp
)
