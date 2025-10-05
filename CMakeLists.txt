cmake_minimum_required(VERSION 3.10)
project(main C)

set(CMAKE_C_COMPILER "clang")

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fopenmp -march=native -ffast-math -funroll-loops -fomit-frame-pointer -flto -Wall -Wextra -Wpedantic -Werror -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wnull-dereference -Wdouble-promotion -Wcast-align -fstrict-aliasing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wpedantic -Werror")

include_directories(include)

file(GLOB_RECURSE SRC_FILES "src/*.c" "src/shapes/*.c" "src/scenes/*.c")
list(FILTER SRC_FILES EXCLUDE REGEX ".*main\\.c$")

add_library(${PROJECT_NAME}_lib STATIC ${SRC_FILES})

add_executable(${PROJECT_NAME} src/main.c)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

enable_testing()

add_subdirectory(tests)
