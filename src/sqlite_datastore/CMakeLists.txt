# Cmake file for building the `sql_datastore` library
#
# Copyright Igor Siemienowicz 2018
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE SQLITE_DATASTORE_SRC *.cpp)
add_library(sqlite-datastore ${SQLITE_DATASTORE_SRC})

