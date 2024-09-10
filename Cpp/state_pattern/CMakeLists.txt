cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

if (NOT ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR})
  project(${CMAKE_CURRENT_SOURCE_DIR}) 
endif()

add_executable(simple_state simple_state.cpp)

set_target_properties(
  simple_state
  PROPERTIES
  FOLDER "state_pattern"
  )
