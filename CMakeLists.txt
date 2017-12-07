cmake_minimum_required(VERSION 3.8)
project(iotseed_sdks_core)

set(CMAKE_CXX_STANDARD 11)
include_directories(inc lib)
add_definitions(-DMG_ENABLE_CALLBACK_USERDATA -DMG_ENABLE_HTTP=0 -DMG_ENABLE_SSL -DMG_ENABLE_THREADS)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -Werror -Wno-unused-function")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow -Wextra")

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -DIOTSEED_DEBUG -g")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -DIOTSEED_DEBUG -g")

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
FIND_PACKAGE (Threads REQUIRED)
message(${CMAKE_THREAD_LIBS_INIT})

FIND_PACKAGE(OpenSSL REQUIRED)
if (${OPENSSL_FOUND})
    include_directories(${OPENSSL_INCLUDE_DIRS})
    set(OPENSSL_USE_STATIC_LIBS TRUE)
    message(STATUS "Using OpenSSL ${OPENSSL_LIBRARIES}")
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
endif ()


set(SOURCE_FILES lib/mongoose/mongoose.c src/mqtt.cpp src/str_utils.cpp src/log.cpp src/operation_utils.cpp src/time_utils.cpp src/recipe.cpp)
add_library(iotseed_sdks_core SHARED ${SOURCE_FILES})
#SET_TARGET_PROPERTIES(iotseed_sdks_core PROPERTIES OUTPUT_NAME "iotseed_sdks_core_static")
#add_library(iotseed_sdks_core_static STATIC ${SOURCE_FILES})
target_link_libraries (iotseed_sdks_core ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (iotseed_sdks_core ${OPENSSL_LIBRARIES})
