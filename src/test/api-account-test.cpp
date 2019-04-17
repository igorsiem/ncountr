/**
 * \file api-test.cpp
 * Tests for the API accounts interface
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <catch2/catch.hpp>
#include <api/api.h>

// Tests for handling of account paths
TEST_CASE("account path", "[unit]")
{
    // Account Names may not contain the account path separator
    std::wstring
        good_name{ L"abc" }
        , bad_name = std::wstring(L"abc")
            + ncountr::api::account::account_path_separator
            + std::wstring(L"xyz");

    REQUIRE(ncountr::api::account::valid_name(good_name));
    REQUIRE(ncountr::api::account::valid_name(bad_name) == false);

    // Account paths can be split into account name vectors
    std::wstring path = L"abc";
    path += ncountr::api::account::account_path_separator;
    path += L"xyz";
    path += ncountr::api::account::account_path_separator;
    path += L"abc";

    std::vector<std::wstring> p_split =
        ncountr::api::account::split_path(path);

    REQUIRE(p_split.size() == 3);
    REQUIRE(p_split[0] == L"abc");
    REQUIRE(p_split[1] == L"xyz");
    REQUIRE(p_split[2] == L"abc");

    // Account name vectors can be concatenated into paths
    REQUIRE(ncountr::api::account::concatenate_path(p_split) == path);
}   // end account path test
