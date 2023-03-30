cmake_minimum_required(VERSION 3.18)
project(rank_types LANGUAGES CXX)

find_package(mdspan)

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME}
   PRIVATE
   main.cpp)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE std::mdspan)
