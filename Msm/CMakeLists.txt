cmake_minimum_required(VERSION 3.0.0)
project(BoostStudyMsm VERSION 0.1.0)

enable_testing()

#add_subdirectory(FirstStep)
add_subdirectory(Basic)

find_package(spdlog)
include_directories(./Basic/Includes)
include_directories(./Basic/TracerLib)
add_executable(Sample SampleCode.cpp)
target_link_libraries(Sample TracerLib)