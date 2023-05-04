cmake_minimum_required(VERSION 3.18)
project(layouts LANGUAGES CXX)

find_package(mdspan)

add_library(utils INTERFACE)
target_include_directories(utils INTERFACE include)
target_link_libraries(utils INTERFACE std::mdspan)

add_executable(2Ddata)
target_sources(2Ddata
   PRIVATE
   2Ddata.cpp)
set_target_properties(2Ddata PROPERTIES
  CXX_STANDARD 23
)
target_link_libraries(2Ddata PRIVATE std::mdspan utils)

add_executable(2DdataPadding)
target_sources(2DdataPadding
   PRIVATE
   2DdataPadding.cpp)
set_target_properties(2DdataPadding PROPERTIES
  CXX_STANDARD 23
)
target_link_libraries(2DdataPadding PRIVATE std::mdspan utils)
