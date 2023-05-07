cmake_minimum_required(VERSION 3.13)

project("Exception for C")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/targets)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/targets)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/targets)

function (setup TYPE)
    set_target_properties(ExceptionForC-${TYPE} PROPERTIES OUTPUT_NAME exception-for-c)
    if (WIN32)
        set_target_properties(ExceptionForC-${TYPE} PROPERTIES PREFIX "lib")
    endif ()

    if (MSVC)
        if(${CMAKE_CXX_FLAGS} MATCHES "/W[0-4]")
            string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        else()
            target_compile_options(ExceptionForC-${TYPE} PRIVATE /W4)
        endif()
    else ()
        target_compile_options(ExceptionForC-${TYPE} PRIVATE -Wall -Wextra)
    endif ()

    if (NOT WIN32 AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        target_compile_options(ExceptionForC-${TYPE} PRIVATE -stdlib=libc++)
    endif ()
endfunction ()

add_library(ExceptionForC-shared SHARED src/exception.c)
add_library(ExceptionForC-static STATIC src/exception.c)

setup(static)
setup(shared)
