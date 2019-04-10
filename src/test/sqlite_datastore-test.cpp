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


    // Open the Datastore, and check that it is ready for action, and
    // initialise it.
    QString dbFilePath = "test-output/sqlite_datastore-test.db";
    auto ds =
        std::make_unique<ncountr::datastores::sqlite::datastore>(dbFilePath);

    REQUIRE(ds->is_ready());

    REQUIRE_NOTHROW(ds->initialise());

    // File format version is 1 (for now)
    REQUIRE(ds->file_format_version() == 1);

    // We can record and retrieve basic document info
    REQUIRE_NOTHROW(ds->set_name(L"Datastore name"));
    REQUIRE_NOTHROW(ds->set_description(L"Datastore description"));

    REQUIRE(ds->name() == L"Datastore name");
    REQUIRE(ds->description() == L"Datastore description");

    // Explicitly delete the Datastore so that we can verify that no
    // exceptions are thrown.
    REQUIRE_NOTHROW(ds = nullptr);

    // qlib::logger::instance().clear();

}   // end sqlite datastore test
