cmake_minimum_required(VERSION 3.13)
project(function)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS: "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fsanitize=undefined,address -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3")

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/gtest)

add_executable(run-tests
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc
        function.h
        tests.cpp)

target_link_libraries(run-tests -lpthread)





