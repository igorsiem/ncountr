# Top-level Cmake project file
#
# Copyright Igor Siemienowicz 2018
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.10)
project($ENV{QPRJ_PROJECT_NAME} LANGUAGES CXX)
# set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 14)

# Using conan for dependencies
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_definitions(${CONAN_DEFINES})

# Note that qlib is an external dependency of this project. It is expected
# be in the same directory as this checkout. At some point, qLib should be
# packaged for conan as well.

include_directories(
    .
    ../..
    ${CONAN_INCLUDE_DIRS_QT}/QtWidgets
)

add_subdirectory(api)
add_subdirectory(gui)
add_subdirectory(test)
