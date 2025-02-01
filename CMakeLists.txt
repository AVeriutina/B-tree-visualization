cmake_minimum_required(VERSION 3.27)
project(B_plus_tree_visualization)

set(CMAKE_CXX_STANDARD 20)

include_directories(source)

add_executable(B_plus_tree_visualization
        source/BPlusTree.cpp
        source/BPlusTree.h
        source/test.cpp)
