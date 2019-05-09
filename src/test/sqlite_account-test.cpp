/**
 * \file sqlite_account-test.cpp
 * Test for the `ncountr::datastores::sqlite::account` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <catch2/catch.hpp>
#include <api/sqlite_datastore/sqlite_datastore.h>
#include <QVariant>

using namespace ncountr;
namespace sql_ds = datastores::sqlite;

// Unit tests for Sqlite Account Table (low level SQL) operations
TEST_CASE("sqlite account table", "[unit]")
{
    // Open a new Datastore, and initialise it.
    //
    // Can use an in-memory database, so we can re-use it multiple times.
    QString dbFilePath = ":memory:";

    auto ds = std::make_unique<sql_ds::datastore>(dbFilePath);
    ds->initialise();

    // Initially, there are no Account records, and the max-id is 0.
    REQUIRE(sql_ds::account::max_id(ds->db()) == 0);

    SECTION("balance account")
    {
        // Create a balance account record
        REQUIRE_NOTHROW(sql_ds::account::create_record(
            ds->db()
            , 1
            , "assets"
            , boost::none
            , "all assets"
            , QDate(2010, 1, 1)
            , 0));

        // Create a balance account record that is a child of the first
        REQUIRE_NOTHROW(sql_ds::account::create_record(
            ds->db()
            , 2
            , "savings"
            , 1
            , "savings account"
            , QDate(2010, 1, 1)
            , 100.0));

        // Check all the valid data
        SECTION("validity checks")
        {
            // Now, the Account table has two records, and we can retrieve
            // their values
            REQUIRE(sql_ds::account::max_id(ds->db()) == 2);

            SECTION("first record (assets)")
            {

                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QString>(
                        ds->db()
                        , "account"
                        , "name"
                        , "id = 1")
                    == "assets");

                // Parent ID is Null (account is at the root)
                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QVariant>(
                        ds->db()
                        , "account"
                        , "parent_id"
                        , "id = 1").isNull());

                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QString>(
                        ds->db()
                        , "account"
                        , "description"
                        , "id = 1")
                    == "all assets");

                // This is a running balance account
                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<bool>(
                        ds->db()
                        , "account"
                        , "has_running_balance"
                        , "id = 1")
                    == true);

                // Opening date is converted from Julian Day
                REQUIRE(
                    QDate::fromJulianDay(
                        sql_ds::retrieveSingleRecordFieldValue<int>(
                        ds->db()
                        , "account"
                        , "opening_date"
                        , "id = 1")) == QDate(2010, 1, 1));
                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<double>(
                        ds->db()
                        , "account"
                        , "opening_balance"
                        , "id = 1") == 0.0);

            }   // end first record section

            SECTION("second record (saving)")
            {

                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QString>(
                        ds->db()
                        , "account"
                        , "name"
                        , "id = 2")
                    == "savings");

                // Parent ID is 1
                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QVariant>(
                        ds->db()
                        , "account"
                        , "parent_id"
                        , "id = 2").toInt() == 1);

                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QString>(
                        ds->db()
                        , "account"
                        , "description"
                        , "id = 2")
                    == "savings account");

                // We DO have a running balance and opening data
                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<bool>(
                        ds->db()
                        , "account"
                        , "has_running_balance"
                        , "id = 2")
                    == true);

                REQUIRE(
                    QDate::fromJulianDay(
                        sql_ds::retrieveSingleRecordFieldValue<int>(
                            ds->db()
                            , "account"
                            , "opening_date"
                            , "id = 2")) == QDate(2010, 1, 1));

                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<double>(
                        ds->db()
                        , "account"
                        , "opening_balance"
                        , "id = 2") == 100.0);

            }   // end record 2 section

        }   // end valid checks section

        SECTION("exception checks - non-balance accounts")
        {
            // We cannot create another record with the same ID            
            REQUIRE_THROWS_AS(
                sql_ds::account::create_record(
                    ds->db()
                    , 1
                    , "another account"
                    , boost::none
                    , "description of account"
                    , QDate(2010, 2, 2)
                    , 0)
                , api::error);

            // Cannot create a record referencing a non-existent parent
            REQUIRE_THROWS_AS(
                sql_ds::account::create_record(
                    ds->db()
                    , 5
                    , "second account"
                    , 100
                    , "description of second account"
                    , QDate(2010, 1, 1)
                    , 100)
                , api::error);

            // We cannot create another record the same name and (null)
            // parent
            REQUIRE_THROWS_AS(
                sql_ds::account::create_record(
                    ds->db()
                    , 3
                    , "assets"
                    , boost::none
                    , "a second root assets account"
                    , QDate(2010, 3, 3)
                    , 100)
                , api::error);

            // Cannot create a record with the same name under the same
            // parent
            REQUIRE_THROWS_AS(
                sql_ds::account::create_record(
                    ds->db()
                    , 3
                    , "savings"
                    , 1
                    , "another savings account"
                    , QDate(2011, 1, 1)
                    , 100)
                , api::error);

        }   // end exception checks

    }   // end balance account section

    SECTION("non-balance record")
    {

        // Create a non-balance account record
        REQUIRE_NOTHROW(sql_ds::account::create_record(
            ds->db()
            , 1
            , "expenses"
            , boost::none
            , "all expenses"));

        // Create a non-balance account record that is a child of the first
        REQUIRE_NOTHROW(sql_ds::account::create_record(
            ds->db()
            , 2
            , "rent"
            , 1
            , "accommodation rental"));

        // Check all the valid data
        SECTION("validity checks - non-balance accounts")
        {
            // Now, the Account table has two records, and we can retrieve
            // their values
            REQUIRE(sql_ds::account::max_id(ds->db()) == 2);

            SECTION("first record - expenses")
            {
                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QString>(
                        ds->db()
                        , "account"
                        , "name"
                        , "id = 1")
                    == "expenses");

                // Parent ID is Null (account is at the root)
                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QVariant>(
                        ds->db()
                        , "account"
                        , "parent_id"
                        , "id = 1").isNull());

                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QString>(
                        ds->db()
                        , "account"
                        , "description"
                        , "id = 1")
                    == "all expenses");

                // No running balance data
                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<bool>(
                        ds->db()
                        , "account"
                        , "has_running_balance"
                        , "id = 1")
                    == false);

                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QVariant>(
                        ds->db()
                        , "account"
                        , "opening_date"
                        , "id = 1").isNull());

                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QVariant>(
                        ds->db()
                        , "account"
                        , "opening_balance"
                        , "id = 1").isNull());
            }   // end first record section

            SECTION("second record - rent")
            {
                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QString>(
                        ds->db()
                        , "account"
                        , "name"
                        , "id = 2")
                    == "rent");

                // Parent ID is 1
                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QVariant>(
                        ds->db()
                        , "account"
                        , "parent_id"
                        , "id = 2").toInt() == 1);

                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QString>(
                        ds->db()
                        , "account"
                        , "description"
                        , "id = 2")
                    == "accommodation rental");

                // No running balance data
                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<bool>(
                        ds->db()
                        , "account"
                        , "has_running_balance"
                        , "id = 2")
                    == false);

                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QVariant>(
                        ds->db()
                        , "account"
                        , "opening_date"
                        , "id = 2").isNull());

                REQUIRE(
                    sql_ds::retrieveSingleRecordFieldValue<QVariant>(
                        ds->db()
                        , "account"
                        , "opening_balance"
                        , "id = 2").isNull());

            }   // end record 2 section

        }   // end valid checks section

        // Check some error cases
        SECTION("exception checks - non-balance accounts")
        {
            // We cannot create another record with the same ID            
             REQUIRE_THROWS_AS(sql_ds::account::create_record(
                ds->db()
                , 1
                , "another non-balance account"
                , boost::none
                , "description of account"), api::error);

             REQUIRE_THROWS_AS(sql_ds::account::create_record(
                ds->db()
                , 2
                , "another non-balance account"
                , boost::none
                , "description of account"), api::error);

            // Cannot create a record referencing a non-existent parent
             REQUIRE_THROWS_AS(sql_ds::account::create_record(
                ds->db()
                , 3
                , "second account"
                , 100
                , "description of second account"), api::error);

            // We cannot create another record the same name and (null)
            // parent
             REQUIRE_THROWS_AS(sql_ds::account::create_record(
                ds->db()
                , 3
                , "expenses"
                , boost::none
                , "another expenses description"), api::error);

            // Cannot create a record with the same name under the same
            // parent
             REQUIRE_THROWS_AS(sql_ds::account::create_record(
                ds->db()
                , 3
                , "rent"
                , 1
                , "another rent account"), api::error);

        }   // end exception checks

    }   // end non-balance record section

    SECTION("both types of account")
    {
        // Create four diffent accounts - two with running balance, and two
        // without
        sql_ds::account::create_record(
            ds->db()
            , 1
            , "assets"
            , boost::none
            , "all assets"
            , QDate(2010, 1, 1)
            , 0);

        sql_ds::account::create_record(
            ds->db()
            , 2
            , "savings"
            , 1
            , "savings account"
            , QDate(2010, 1, 1)
            , 100.0);

        sql_ds::account::create_record(
            ds->db()
            , 3
            , "expenses"
            , boost::none
            , "all expenses");

        sql_ds::account::create_record(
            ds->db()
            , 4
            , "rent"
            , 3
            , "accommodation rental");

        // Now we have four accounts
        REQUIRE(sql_ds::account::max_id(ds->db()) == 4);

        SECTION("exception checks - both account types")
        {
            // Cannot put a Running Balance Account under a non-RBA.
            REQUIRE_THROWS_AS(
                sql_ds::account::create_record(
                    ds->db()
                    , 5
                    , "bank loan"
                    , 3         // <- expenses parent account
                    , "loan from the bank"
                    , QDate(2012, 1, 1)
                    , -1000
                    )
                , api::error);

            // Cannot put an NRBA under an RBA
            REQUIRE_THROWS_AS(
                sql_ds::account::create_record(
                    ds->db()
                    , 5
                    , "salary"
                    , 2     // <- savings parent account
                    , "salary description")
                , api::error);

            FAIL("tests are incomplete - exception cases");
        }

    }
}   // end Sqlite Account tests
