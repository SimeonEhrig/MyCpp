cmake_minimum_required(VERSION 3.28)

project(cxx_modules LANGUAGES CXX)

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_VERSION LESS 16)
    message(FATAL_ERROR "only Clang 16+ is supported for this example")
endif()

# the settings are required for modules at the moment
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(math)
# compile modules
target_sources(math PUBLIC FILE_SET fmt_module TYPE CXX_MODULES FILES math.cppm)

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
    main.cpp
)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE math)


