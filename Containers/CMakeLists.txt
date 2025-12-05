cmake_minimum_required(VERSION 3.15)
project(Containers LANGUAGES CXX)

add_library(vectorlib INTERFACE)
target_include_directories(vectorlib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(vector vector/tests/vector_template_tests.cpp)
target_link_libraries(vector PUBLIC vectorlib)