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
#include <api/sqlite_datastore/sqlite_datastore.h>

using namespace ncountr;
namespace sql_ds = datastores::sqlite;

// Unit tests for the Sqlite Datastore
TEST_CASE("sqlite datastore", "[unit]")
{

    // Open the Datastore, and check that it is ready for action, and
    // initialise it.
    //
    // Can use an in-memory database or a file by uncommenting the
    // appropriate line.
    QString dbFilePath =
        "test-output/sqlite_datastore-test-sqlite_datastore.db";
        // ":memory:";

    auto ds = std::make_unique<sql_ds::datastore>(dbFilePath);

    REQUIRE(ds->is_ready());
    REQUIRE_NOTHROW(ds->initialise());

    // File format version is 1 (for now)
    REQUIRE(ds->file_format_version() == 1);

    // We can record and retrieve basic document info
    REQUIRE_NOTHROW(ds->set_name(L"Datastore name"));
    REQUIRE_NOTHROW(ds->set_description(L"Datastore description"));

    REQUIRE(ds->name() == L"Datastore name");
    REQUIRE(ds->description() == L"Datastore description");

    // Explicitly delete the Datastore object so that we can verify that no
    // exceptions are thrown.
    REQUIRE_NOTHROW(ds = nullptr);

}   // end sqlite datastore test

// Verify basic QDate / API date conversions
TEST_CASE("qdate with boost date", "[unit]")
{

    ncountr::api::date ncd1(2002, 1, 1);
    REQUIRE(sql_ds::to_qdate(ncd1) == QDate(2002, 1, 1));

    QDate qd1(2010, 2, 2);
    REQUIRE(sql_ds::to_api_date(qd1) == api::date(2010, 2, 2));

}   // end qdate with boost date test
