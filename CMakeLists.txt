cmake_minimum_required(VERSION 3.20)

project(CppSerialize CXX)

add_subdirectory(CppSerialize)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_executable(example example.cpp)
    target_link_libraries(example PRIVATE CppSerialize)
endif()
