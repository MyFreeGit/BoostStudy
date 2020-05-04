cmake_minimum_required(VERSION 3.0.0)
project(BoostStudyMsm VERSION 0.1.0)

enable_testing()

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

add_executable(SimpleMSM  SimpleMSM.cpp)