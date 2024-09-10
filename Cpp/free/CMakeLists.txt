cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

if (NOT ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR})
  project(${CMAKE_CURRENT_SOURCE_DIR}) 
endif()



add_executable(icci implicit_conversion_and_copy_initialization.cpp)
add_executable(deleted_key deleted_key.cpp)

set_target_properties(
  icci
  deleted_key
  PROPERTIES
  FOLDER "free"
  )
