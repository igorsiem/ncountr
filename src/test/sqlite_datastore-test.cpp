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

    // Account-related tests
    SECTION("account")
    {

        // Low-level SQL Database operations
        SECTION("low-level operations")
        {

        }   // end low-level operations section

        // TODO - higher-level business-logic tests

    }   // ed acccount section

    // Explicitly delete the Datastore object so that we can verify that no
    // exceptions are thrown.
    REQUIRE_NOTHROW(ds = nullptr);

}   // end sqlite datastore test

TEST_CASE("sqlite account", "[unit]")
{

    SECTION("low-level operations")
    {
        // Open the Datastore and initialise it.
        //
        // Can use an in-memory database or a file by uncommenting the
        // appropriate line.
        QString dbFilePath =
            "test-output/sqlite_datastore-test-sqlite_account-1.db";
            // ":memory:";

        auto ds =
            std::make_unique<ncountr::datastores::sqlite::datastore>(
                dbFilePath);
        ds->initialise();

        // There are no accounts in the database yet.
        REQUIRE(ncountr::datastores::sqlite::account::max_id(ds->db()) ==
            0);

        // Low-level Account creation method
        REQUIRE_NOTHROW(
            ncountr::datastores::sqlite::account::create_record(
                ds->db()
                , 1
                , QString("abc")
                , QString("Description of account \"abc\"")
                , ncountr::api::account::type_t::asset));

        // Should have one Account record now.
        REQUIRE(ncountr::datastores::sqlite::account::max_id(ds->db()) ==
            1);

        // Retrieve the record by ID
        auto ar =
            ncountr::datastores::sqlite::account::find_by_id(
                ds->db()
                , 1);

        if (ar == boost::none) FAIL("expected to retrieve a record");
        REQUIRE(ar->count() == 6);
            
        // Just check one field value - the full-path
        auto full_path_idx = ar->indexOf("full_path");
        REQUIRE(ar->value(full_path_idx).value<QString>() == "abc");

        // Retrieve the record by full path
        ar = ncountr::datastores::sqlite::account::find_by_full_path(
            ds->db(), "abc");
        if (ar == boost::none) FAIL("expected to retrieve a record");

        // Check the account type is 'asset', using the appropriate string
        // transformation
        auto accout_type_idx = ar->indexOf("account_type");
        REQUIRE(
            ar->value(accout_type_idx).value<QString>()
            == ncountr::datastores::sqlite::account::to_qstring(
                ncountr::api::account::type_t::asset));

        // Attempt to retrieve a non-existent record
        ar =
            ncountr::datastores::sqlite::account::find_by_id(
                ds->db()
                , 2);                

        if (ar != boost::none) FAIL("expected no record to be returned");

        // Destroy the record, and attempt to locate it again.
        ncountr::datastores::sqlite::account::destroy_record_by_id(
            ds->db()
            , 1);

        ar =
            ncountr::datastores::sqlite::account::find_by_id(
                ds->db()
                , 1);                

        if (ar != boost::none) FAIL("expected no record to be returned");

        ar =
            ncountr::datastores::sqlite::account::find_by_full_path(
                ds->db()
                , "abc");                

        if (ar != boost::none) FAIL("expected no record to be returned");

    }   // end low-level section

}   // end Sqlite Account test

// Verify basic QDate / API date conversions
TEST_CASE("qdate with boost date", "[unit]")
{

    ncountr::api::date ncd1(2002, 1, 1);
    REQUIRE(ncountr::datastores::sqlite::to_qdate(ncd1) ==
        QDate(2002, 1, 1));

    QDate qd1(2010, 2, 2);
    REQUIRE(ncountr::datastores::sqlite::to_api_date(qd1) ==
        ncountr::api::date(2010, 2, 2));

}   // end qdate with boost date test
