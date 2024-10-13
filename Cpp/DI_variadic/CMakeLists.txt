cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

if (NOT ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR})
  project(${CMAKE_CURRENT_SOURCE_DIR}) 
endif()


include_directories(PUBLIC ./)
add_executable(bad_example bad_type_map.cpp)
add_executable(example type_map_example.cpp)

set_target_properties(
  bad_example
  example
  PROPERTIES
  FOLDER "DI_variadic"
  )
