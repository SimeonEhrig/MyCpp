cmake_minimum_required(VERSION 3.18)
project(dump_operator LANGUAGES CXX)

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME}
   PRIVATE
   main.cpp)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
)
