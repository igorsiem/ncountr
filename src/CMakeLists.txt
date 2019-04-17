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

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Using conan for dependencies
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_definitions(${CONAN_DEFINES})

# Qt stuff
find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Sql
    Widgets
)

# Note that qlib is an external dependency of this project. It is expected
# be in the same directory as this checkout. At some point, qLib should be
# packaged for conan as well.

include_directories(
    .
    ../..
    ${CONAN_INCLUDE_DIRS_QT}/QtCore
    ${CONAN_INCLUDE_DIRS_QT}/QtSql
    ${CONAN_INCLUDE_DIRS_QT}/QtWidgets
)

add_subdirectory(api)
add_subdirectory(gui)
# add_subdirectory(sqlite_datastore)
add_subdirectory(test)
