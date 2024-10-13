cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(${CMAKE_CURRENT_SOURCE_DIR}) 

set(CMAKE_CXX_STANDARD 20)

# Option to enable or disable building the documentation
option(BUILD_DOC "Build documentation" ON)

# Find Doxygen if it is installed
find_package(Doxygen)

if (DOXYGEN_FOUND)
  # Set output directory for documentation
  set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs")

  # Path to the Doxyfile (Doxygen config file)
  message("Current source dir ${CMAKE_CURRENT_SOURCE_DIR}")
  set(DOXYGEN_IN "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")
  set(DOXYGEN_OUT "${CMAKE_BINARY_DIR}/Doxyfile")

  # Configure the Doxyfile (replace variables in Doxyfile.in)
  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

  # Add a custom target for building the documentation
  add_custom_target(doc ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM
      DEPENDS ${CMAKE_SOURCE_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/*.hpp
)
else()
  message("Doxygen not found, unable to build documentation")
endif()

include_directories(PUBLIC ./)
add_executable(bad_example bad_type_map.cpp)
add_executable(app app.cpp)
add_executable(example type_map_example.cpp)

set_target_properties(
  bad_example
  app
  example
  PROPERTIES
  FOLDER "DI_variadic"
  )
