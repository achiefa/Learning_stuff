cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

#message(SUBDIR_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
#message(SOURCE_DIR_FROM_SUBDIR="${CMAKE_SOURCE_DIR}")

if (NOT ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR})
  project(${CMAKE_CURRENT_SOURCE_DIR}) 
endif()

add_executable(std_forward_1
    std_forward_1.cc
   )

add_executable(std_forward_wrong
   std_forward_wrong.cc
  )

add_executable(debug_decorator
debug_decorator.cc
)

set_target_properties(
   std_forward_1
   std_forward_wrong
   debug_decorator
   PROPERTIES
   FOLDER "Forwarding_reference"
   )
