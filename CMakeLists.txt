cmake_minimum_required(VERSION 3.13)

project("Exception for C")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/targets)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/targets)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/targets)

function (setup TYPE)
    target_compile_definitions(ExceptionForC-${TYPE} PRIVATE _EXCEPTION_FOR_C_BUILDING)

    if (MSVC)
        if(${CMAKE_CXX_FLAGS} MATCHES "/W[0-4]")
            string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        else()
            target_compile_options(ExceptionForC-${TYPE} PRIVATE /W4)
        endif()
        target_compile_options(ExceptionForC-${TYPE} PRIVATE /Zc:preprocessor)
    else ()
        target_compile_options(ExceptionForC-${TYPE} PRIVATE -Wall -Wextra)
    endif ()

    if (NOT WIN32 AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        target_compile_options(ExceptionForC-${TYPE} PRIVATE -stdlib=libc++)
        target_link_options(ExceptionForC-${TYPE} PRIVATE -stdlib=libc++)
    endif ()

    if (${TYPE} STREQUAL "shared")
        target_compile_definitions(ExceptionForC-${TYPE} PUBLIC _EXCEPTION_FOR_C_SHARED_LIBRARY)
    elseif (WIN32)
        set_target_properties(ExceptionForC-${TYPE} PROPERTIES PREFIX "lib")
    endif ()

    set_target_properties(ExceptionForC-${TYPE} PROPERTIES OUTPUT_NAME exception-for-c)
endfunction ()

add_library(ExceptionForC-static STATIC src/exception.c)
setup(static)
add_library(ExceptionForC-shared SHARED src/exception.c)
setup(shared)
