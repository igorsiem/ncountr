/**
 * \file api-test.cpp
 * Tests for *nCountr* API
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <catch2/catch.hpp>
#include <api/api.h>

// simple test for the 'version' API function
TEST_CASE("version", "[unit]")
{
    // ensure that 'version' returns a non-empty string
    REQUIRE(ncountr::api::version().empty() == false);
    REQUIRE(ncountr::api::wversion().empty() == false);
}   // end version test

