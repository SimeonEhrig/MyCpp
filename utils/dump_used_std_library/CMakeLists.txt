cmake_minimum_required(VERSION 3.22)

project(showLibCppImplementation)

option(CUDA "Enable CUDA compiler" OFF)
option(HIP "Enable ROCM compiler" OFF)
option(LIBCPP "use libc++ (llvm implementation)" OFF)

# fix bug in the CUDA compiler detection
if(CUDA)
    if("${CMAKE_CUDA_COMPILER}" MATCHES "clang*")
        message(STATUS "use clang as CUDA compiler")
        
        # Because of a bug, we need to set the C++ standard manually for clang
        # as cuda compiler. Otherwise the check if clang can compile some 
        # example code fails.
        # Because of another bug, we cannot reuse the C++ standard later via
        # CUDA_STANDARD. But nobody should use C++98 ;-) 
        set(_CMAKE_CUDA_FLAGS_BEFORE ${CMAKE_CUDA_FLAGS})
        set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -std=c++98")
        enable_language(CUDA)
        set(CMAKE_CUDA_FLAGS ${_CMAKE_CUDA_FLAGS_BEFORE})
        unset(_CMAKE_CUDA_FLAGS_BEFORE)
    else()
        message(STATUS "use nvcc as CUDA compiler")
        
        if("${CMAKE_CUDA_HOST_COMPILER}" STREQUAL "")
            message(STATUS "set to CMAKE_CUDA_HOST_COMPILER to CMAKE_CXX_COMPILER")
            set(CMAKE_CUDA_HOST_COMPILER ${CMAKE_CXX_COMPILER})
        endif()

        enable_language(CUDA)
    endif()
    message("${CMAKE_CUDA_COMPILER}")
endif()

if(CUDA)
    set_source_files_properties(main.cpp PROPERTIES LANGUAGE CUDA)
endif()

if(HIP)
    enable_language(HIP)
    set_source_files_properties(main.cpp PROPERTIES LANGUAGE HIP)
endif()

if(LIBCPP AND CUDA AND "${CMAKE_CUDA_COMPILER}" MATCHES "nvcc*")
    message(FATAL_ERROR "nvcc does not support libc++")
endif()



if(LIBCPP)
    if(CUDA)
        list(APPEND CMAKE_CUDA_FLAGS "-stdlib=libc++")
        list(APPEND CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++")
    elseif(HIP AND NOT LIBCPP_PATH)
        list(APPEND CMAKE_HIP_FLAGS "-stdlib=libc++")
    else()
        list(APPEND CMAKE_CXX_FLAGS "-stdlib=libc++")
    endif()
endif()

add_library(headers INTERFACE)
target_include_directories(headers INTERFACE include)

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
    main.cpp
)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE headers)

if(LIBCPP_PATH)
    message(STATUS "set custom libc++ path: ${LIBCPP_PATH}")
    
    add_library(custom_libcpp_path INTERFACE)
    # https://libcxx.llvm.org/UsingLibcxx.html#using-a-custom-built-libc
    target_compile_options  (custom_libcpp_path INTERFACE -nostdinc++ -nostdlib++)
    target_include_directories(custom_libcpp_path SYSTEM INTERFACE ${LIBCPP_PATH}/include/c++/v1)
    target_include_directories(custom_libcpp_path SYSTEM INTERFACE ${LIBCPP_PATH}/include/x86_64-unknown-linux-gnu/c++/v1/)
    target_link_directories(custom_libcpp_path INTERFACE ${LIBCPP_PATH}/lib)
    target_link_directories(custom_libcpp_path INTERFACE ${LIBCPP_PATH}/lib/x86_64-unknown-linux-gnu/)
    target_link_libraries(custom_libcpp_path INTERFACE -lc++)
    
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE custom_libcpp_path)
endif()
