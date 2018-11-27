macro(configure_build_type)
    set(DEFAULT_BUILD_TYPE RelWithDebInfo)

    if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
        message(STATUS "No build type selected, set to ${DEFAULT_BUILD_TYPE}")
        set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE})
    endif()

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(WARNING "Debug build type selected, forcing to RelWithDebInfo")
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
    endif()

    if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g -DNDEBUG")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
    endif()
endmacro()

macro(configure_compiler_flags)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endmacro()

