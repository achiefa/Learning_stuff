cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

if (NOT ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR})
  project(${CMAKE_CURRENT_SOURCE_DIR}) 
endif()

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

find_program(NNAD_CONFIG nnad-config REQUIRED)
if (NNAD_CONFIG)
    exec_program(${NNAD_CONFIG}
        ARGS --cflags
        OUTPUT_VARIABLE NNAD_CFLAGS
    )
    set(NNAD_CFLAGS ${NNAD_CFLAGS} CACHE STRING INTERNAL)
endif(NNAD_CONFIG)

add_executable(app test_NNAD.cpp)
#add_executable(app app.cpp)
target_compile_options(app PUBLIC ${NNAD_CFLAGS})
target_link_libraries(app PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
target_include_directories(app PUBLIC ./)

set_target_properties(
  app
  PROPERTIES
  FOLDER "computation_graph"
  )
