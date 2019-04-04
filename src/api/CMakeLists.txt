# Cmake file for building the API library
#
# Copyright Igor Siemienowicz 2018
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt)

file(GLOB_RECURSE API_SRC *.cpp)
add_library($ENV{QPRJ_PROJECT_NAME}-api ${API_SRC})
