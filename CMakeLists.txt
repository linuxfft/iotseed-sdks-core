cmake_minimum_required(VERSION 3.8)
project(iotseed_sdks_core)

set(CMAKE_CXX_STANDARD 11)
include_directories(inc)
set(SOURCE_FILES lib/mongoose/mongoose.c)
set(SOURCE_FILES src/mqtt.cpp src/str_util.cpp)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -DMG_ENABLE_SSL -g -W -Wall -Werror -Wno-unused-function -DMG_ENABLE_HTTP=0")
add_library(iotseed_sdks_core SHARED ${SOURCE_FILES})