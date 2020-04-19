cmake_minimum_required(VERSION 3.0.0)
project(BoostStudyMsm VERSION 0.1.0)

enable_testing()

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(MsmStateTest  MsmStateTest.cpp)
message(${GTEST_BOTH_LIBRARIES})
target_link_libraries(MsmStateTest ${GTEST_BOTH_LIBRARIES} gmock)
GTEST_ADD_TESTS(MsmStateTest "" MsmStateTest.cpp)
