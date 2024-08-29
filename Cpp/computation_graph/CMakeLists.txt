cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

if (NOT ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR})
  project(${CMAKE_CURRENT_SOURCE_DIR}) 
endif()

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

#add_executable(app app.cpp )
add_executable(app test.cpp)
target_link_libraries(app PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
target_include_directories(app PUBLIC ./)

set_target_properties(
  app
  PROPERTIES
  FOLDER "computation_graph"
  )
