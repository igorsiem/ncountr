/**
 * \file sqlite_datastore-test.cpp
 * Test for the `ncountr::datastores::sqlite::datastore` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <catch2/catch.hpp>
#include <sqlite_datastore/sqlite_datastore.h>

// Unit tests for the Sqlite Datastore
TEST_CASE("sqlite datastore", "[unit]")
{

    // qlib::logger::instance().set_for_console();

    QString dbFilePath = "test-output/sqlite_datastore-test.db";

    // Open the database, and check that it is ready for action.
    auto db = std::make_unique<ncountr::datastores::sqlite::datastore>(
        dbFilePath);
    REQUIRE(db->is_ready());

    // qlib::logger::instance().clear();

///    FAIL("tests are incomplete");
}   // end sqlite datastore test
