cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

if (NOT ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR})
  project(${CMAKE_CURRENT_SOURCE_DIR}) 
endif()



add_executable(smart_ptr smart_ptr.cpp)
add_executable(smart_ptr_concept smart_ptr_concept.cpp)
add_executable(no_template_policy no_template_policy.cpp)
add_executable(smartptr_for_no_template smartptr_for_no_template.cpp)
add_executable(template_template template_template.cpp)
add_executable(template_template2 template_template2.cpp)


set_target_properties(
  template_template2
  template_template
  smartptr_for_no_template
  no_template_policy
  smart_ptr_concept
  smart_ptr
  PROPERTIES
  FOLDER "policy"
  )
