cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

if (NOT ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR})
  project(${CMAKE_CURRENT_SOURCE_DIR}) 
endif()

find_package(PkgConfig REQUIRED)
pkg_search_module(EIGEN3 REQUIRED eigen3)

find_program(NNAD_CONFIG nnad-config REQUIRED)
if (NNAD_CONFIG)
    exec_program(${NNAD_CONFIG}
        ARGS --cflags
        OUTPUT_VARIABLE NNAD_CFLAGS
    )
    set(NNAD_CFLAGS ${NNAD_CFLAGS} CACHE STRING INTERNAL)
endif(NNAD_CONFIG)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${NNAD_CFLAGS}")

add_executable(test1 test1.cpp)
add_executable(policy test_policy.cpp)
add_executable(crtp test_CRTP.cpp)

set_target_properties(
  crtp
  test1
  policy
  PROPERTIES
  FOLDER "tets"
  )
