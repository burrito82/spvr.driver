# $Id:$

cmake_minimum_required(VERSION 2.8)
project(driver_smartvr)

# gcc
option(ENABLE_CUSTOM_GCC_FLAGS "g++ specific flags" on)
option(CUSTOM_GCC_FLAGS_COLORED "Colored g++ output" off)
option(CUSTOM_GCC_FLAGS_WALL "As many warnings as good" on)
option(CUSTOM_GCC_FLAGS_WPEDANTIC "Pedantic" on)
option(CUSTOM_GCC_FLAGS_WINLINE "Inline warnings" off)
option(CUSTOM_GCC_FLAGS_NATIVE "Native" on)
option(CUSTOM_GCC_FLAGS_OPTIMIZE "Optimize" on)

# g++
option(CUSTOM_GPLUSPLUS_FLAGS_EFFCPP "Efficient C++ warnings" on)
option(CUSTOM_GPLUSPLUS_FLAGS_SANITIZE "Sanitize" off)
option(CUSTOM_GPLUSPLUS_FLAGS_C11 "C++11" off)
option(CUSTOM_GPLUSPLUS_FLAGS_C14 "C++14" on)

set(CUSTOM_GCC_FLAGS "-O3 -s")

if (CUSTOM_GCC_FLAGS_COLORED)
    set(CUSTOM_GCC_FLAGS ${CUSTOM_GCC_FLAGS} "-fdiagnostics-color=always")
endif (CUSTOM_GCC_FLAGS_COLORED)

if (CUSTOM_GCC_FLAGS_WALL)
    set(CUSTOM_GCC_FLAGS ${CUSTOM_GCC_FLAGS} "-Wall -Wextra -Wfatal-errors -Wdouble-promotion -Wswitch-default -Wstrict-overflow -Wshadow -Wconversion -Wuseless-cast -Winvalid-pch -Wold-style-cast -Woverloaded-virtual -Wdisabled-optimization")
endif (CUSTOM_GCC_FLAGS_WALL)

if (CUSTOM_GCC_FLAGS_WPEDANTIC)
    set(CUSTOM_GCC_FLAGS ${CUSTOM_GCC_FLAGS} "-Wpedantic")
endif (CUSTOM_GCC_FLAGS_WPEDANTIC)

if (CUSTOM_GCC_FLAGS_WINLINE)
    set(CUSTOM_GCC_FLAGS ${CUSTOM_GCC_FLAGS} "-Winline")
endif (CUSTOM_GCC_FLAGS_WINLINE)

if (CUSTOM_GCC_FLAGS_NATIVE)
    set(CUSTOM_GCC_FLAGS ${CUSTOM_GCC_FLAGS} "-march=native")
endif (CUSTOM_GCC_FLAGS_NATIVE)

if (CUSTOM_GCC_FLAGS_OPTIMIZE)
    set(CUSTOM_GCC_FLAGS ${CUSTOM_GCC_FLAGS} "-mfpmath=sse -Ofast -flto -funroll-loops -ftree-vectorize -ffast-math -ftree-vectorizer-verbose=2")
endif (CUSTOM_GCC_FLAGS_OPTIMIZE)

if (CUSTOM_GPLUSPLUS_FLAGS_EFFCPP)
    set(CUSTOM_GPLUSPLUS_FLAGS ${CUSTOM_GPLUSPLUS_FLAGS} "-Weffc++")
endif (CUSTOM_GPLUSPLUS_FLAGS_EFFCPP)

if (CUSTOM_GPLUSPLUS_FLAGS_SANITIZE)
    set(CUSTOM_GPLUSPLUS_FLAGS ${CUSTOM_GPLUSPLUS_FLAGS} "-fsanitize=undefined")
endif (CUSTOM_GPLUSPLUS_FLAGS_SANITIZE)

if (CUSTOM_GPLUSPLUS_FLAGS_C11)
    set(CUSTOM_GPLUSPLUS_FLAGS ${CUSTOM_GPLUSPLUS_FLAGS} "-std=c++11")
endif (CUSTOM_GPLUSPLUS_FLAGS_C11)

if (CUSTOM_GPLUSPLUS_FLAGS_C14)
    set(CUSTOM_GPLUSPLUS_FLAGS ${CUSTOM_GPLUSPLUS_FLAGS} "-std=c++14")
endif (CUSTOM_GPLUSPLUS_FLAGS_C14)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    if (ENABLE_CUSTOM_GCC_FLAGS)
        add_definitions(${CUSTOM_GCC_FLAGS})
        add_definitions(${CUSTOM_GPLUSPLUS_FLAGS})
    endif (ENABLE_CUSTOM_GCC_FLAGS)
endif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

set (CUSTOM_LIBRARIES)

# Including the source files of all source subfolders recursively

include("./_SourceFiles.cmake")

add_library(driver_smartvr SHARED ${ProjectSources})
target_link_libraries(driver_smartvr
    ${CUSTOM_LIBRARIES}
)

