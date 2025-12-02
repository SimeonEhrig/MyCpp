cmake_minimum_required(VERSION 3.19)

project(constexprFuncConcept LANGUAGES CXX)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:SHELL:-fconcepts-diagnostics-depth=4>")
endif()

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
    main.cpp
)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
)
