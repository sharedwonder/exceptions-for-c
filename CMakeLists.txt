cmake_minimum_required(VERSION 3.13)

project("exceptions-for-c")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/targets)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/targets)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/targets)

function (lib_target TYPE)
    string(TOLOWER ${TYPE} LOWER_TYPE)
    set(NAME "lib.${LOWER_TYPE}")

    add_library(${NAME} ${TYPE} "src/exception.c")

    target_compile_definitions(${NAME} PRIVATE "_EXCEPTIONS_FOR_C_BUILDING")

    if (MSVC)
        if (${CMAKE_CXX_FLAGS} MATCHES "/W[0-4]")
            string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        else ()
            target_compile_options(${NAME} PRIVATE "/W4")
        endif ()
        target_compile_options(${NAME} PRIVATE "/Zc:preprocessor")
    else ()
        target_compile_options(${NAME} PRIVATE "-Wall" "-Wextra")
    endif ()

    if (${TYPE} STREQUAL "SHARED")
        target_compile_definitions(${NAME} PUBLIC "_EXCEPTIONS_FOR_C_SHARED_LIBRARY")
    elseif (WIN32)
        set_target_properties(${NAME} PROPERTIES PREFIX "lib")
    endif ()

    set_target_properties(${NAME} PROPERTIES OUTPUT_NAME "exceptions-for-c")
endfunction ()

lib_target(SHARED)
lib_target(STATIC)
