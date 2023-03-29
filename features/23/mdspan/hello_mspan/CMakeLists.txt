cmake_minimum_required(VERSION 3.18)
project(hello_mspan LANGUAGES CXX)

# set default c++ standard to c++20
# c++23 supports the multidemensional access operator.
if(NOT "${CMAKE_CXX_STANDARD}")
  set(CMAKE_CXX_STANDARD 20)
endif()

find_package(mdspan REQUIRED)

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME}
  PRIVATE
  main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE std::mdspan)
