/**
 * \file api-date-test.cpp
 * Tests for the date type and associated methods in the API
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <catch2/catch.hpp>
#include <api/api.h>

// Basic verification of Boost-based date functionality
TEST_CASE("boost date functionality", "[unit]")
{
    using namespace ncountr::api;

    date d(2002, Jan, 1);

    REQUIRE(d.year() == 2002);
    REQUIRE(d.month().as_number() == 1);
    REQUIRE(d.day() == 1);

}   // end date tests
