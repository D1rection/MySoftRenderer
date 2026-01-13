cmake_minimum_required(VERSION 3.12...3.26)

get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT is_multi_config AND NOT (CMAKE_BUILD_TYPE OR DEFINED ENV{CMAKE_BUILD_TYPE}))
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Release default")
endif()

project(tiny_renderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(iwyu "Run include-what-you-use" OFF)
if(iwyu)
  find_program(IWYU_EXE NAMES include-what-you-use REQUIRED)
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_EXE})
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|Intel")
  add_compile_options(-Wall -Wextra)
endif()

find_package(OpenMP COMPONENTS CXX)

add_subdirectory(src)
add_subdirectory(apps)

file(GENERATE OUTPUT .gitignore CONTENT "*")
