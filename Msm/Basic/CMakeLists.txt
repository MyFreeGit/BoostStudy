cmake_minimum_required(VERSION 3.0.0)
project(BoostStudyMsm VERSION 0.1.0)

enable_testing()

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB_RECURSE CPP_SRC ./*.cpp)
foreach(cpp_file IN LISTS CPP_SRC)
    get_filename_component(pure_filename ${cpp_file} NAME_WLE)
    message("add exmaple executable ${pure_filename} with source file ${cpp_file}")
    add_executable(${pure_filename} ${cpp_file})
    target_link_libraries(${pure_filename} ${GTEST_BOTH_LIBRARIES} gmock)
    GTEST_ADD_TESTS(${pure_filename} "" ${cpp_file})
endforeach()
