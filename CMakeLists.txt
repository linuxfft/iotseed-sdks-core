cmake_minimum_required(VERSION 3.8)
project(iotseed_sdks_core)

set(CMAKE_CXX_STANDARD 11)
include_directories(inc)
set(SOURCE_FILES src/mqtt.cpp src/mqtt.h)
add_library(iotseed_sdks_core SHARED ${SOURCE_FILES})