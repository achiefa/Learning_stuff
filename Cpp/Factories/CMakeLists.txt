cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

#message(SUBDIR_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
#message(SOURCE_DIR_FROM_SUBDIR="${CMAKE_SOURCE_DIR}")

if (NOT ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR})
  project(${CMAKE_CURRENT_SOURCE_DIR}) 
endif()

add_executable(basic_factory basic_factory.cpp)
add_executable(basic_factory_2 basic_factory_2.cpp)
add_executable(arguments_factory arguments_factory.cpp)

set_target_properties(
  basic_factory
  arguments_factory
  basic_factory_2
  PROPERTIES
  FOLDER "Forwarding_reference"
  )
