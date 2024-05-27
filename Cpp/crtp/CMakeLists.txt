cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

#message(SUBDIR_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
#message(SOURCE_DIR_FROM_SUBDIR="${CMAKE_SOURCE_DIR}")

if (NOT ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR})
  project(${CMAKE_CURRENT_SOURCE_DIR}) 
endif()

add_executable(class_registry class_registry.cpp)
add_executable(destructor destructor.cpp)
add_executable(basic basic.cpp)
add_executable(base_size base_size.cpp)
add_executable(static_polymorphism static_polymorphism.cpp)

set_target_properties(
  static_polymorphism
  base_size
  basic
  destructor
  class_registry
  PROPERTIES
  FOLDER "Forwarding_reference"
  )
