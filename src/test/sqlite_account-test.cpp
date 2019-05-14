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

                auto rec = sql_ds::account::find_by_id(ds->db(), 1);
                if (rec == boost::none)
                    FAIL("expected to retrieve record with ID 1, but no "
                        "record was returned");

                REQUIRE(rec->value("name").toString() == "assets");

                // Parent ID is Null (account is at the root)
                REQUIRE(rec->value("parent_id").isNull());

                REQUIRE(rec->value("description").toString()
                    == "all assets");

                // This is a running balance account
                REQUIRE(rec->value("has_running_balance").toBool() == true);
                REQUIRE(
                    QDate::fromJulianDay(rec->value("opening_date").toInt())
                    == QDate(2010, 1, 1));
                REQUIRE(rec->value("opening_balance").toDouble() == 0.0);

            }   // end first record section

            SECTION("second record (saving)")
            {

                auto rec = sql_ds::account::find_by_id(ds->db(), 2);
                if (rec == boost::none)
                    FAIL("expected to retrieve record with ID 2, but no "
                        "record was returned");

                REQUIRE(rec->value("name").toString() == "savings");
                REQUIRE(rec->value("parent_id").toInt() == 1);
                REQUIRE(rec->value("description").toString()
                    == "savings account");

                // We DO have a running balance and opening data
                REQUIRE(rec->value("has_running_balance").toBool() == true);
                REQUIRE(rec->value("opening_date").toInt()
                    == QDate(2010, 1, 1).toJulianDay());
                REQUIRE(rec->value("opening_balance").toDouble() == 100.0);

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
                // Retrieve the existing record, and check its values
                auto rec = sql_ds::account::find_by_id(ds->db(), 1);
                if (rec == boost::none)
                    FAIL("expected to retrieve record with ID 1, but no "
                        "record was returned");

                REQUIRE(rec->value("name").toString() == "expenses");

               // Parent ID is Null (account is at the root)
                REQUIRE(rec->value("parent_id").isNull());

                REQUIRE(rec->value("description").toString()
                    == "all expenses");

                // No running balance data
                REQUIRE(rec->value("has_running_balance").toBool() == false);
                REQUIRE(rec->value("opening_date").isNull());
                REQUIRE(rec->value("opening_balance").isNull());
            }   // end first record section

            SECTION("second record - rent")
            {
                // Retrieve the existing record, and check its values
                auto rec = sql_ds::account::find_by_id(ds->db(), 2);
                if (rec == boost::none)
                    FAIL("expected to retrieve record with ID 1, but no "
                        "record was returned");

                REQUIRE(rec->value("name").toString() == "rent");
                
                // Parent ID is 1
                REQUIRE(rec->value("parent_id").toInt() == 1);

                REQUIRE(rec->value("description").toString()
                    == "accommodation rental");

                // No running balance data
                REQUIRE(rec->value("has_running_balance").toBool() == false);
                REQUIRE(rec->value("opening_date").isNull());
                REQUIRE(rec->value("opening_balance").isNull());

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

        }   // end exception checks section

        SECTION("finding records")
        {
            // Note that finding by record ID is verified in most of the
            // other sections

            SECTION("find by parent ID and name")
            {
                auto rec =
                    sql_ds::account::find_by_parent_id_and_name(
                        ds->db(), boost::none, "assets");

                if (rec == boost::none)
                    FAIL("expected to find record, but none was returned");

                REQUIRE(rec->value("id").toInt() == 1);

                rec = sql_ds::account::find_by_parent_id_and_name(
                    ds->db(), 3, "rent");

                if (rec == boost::none)
                    FAIL("expected to find record, but none was returned");

                REQUIRE(rec->value("id").toInt() == 4);

            } // end finding by parent ID and name

            SECTION("find by full path")
            {
                auto rec =
                    sql_ds::account::find_by_full_path(ds->db(), "assets");

                if (rec == boost::none)
                    FAIL("expected to find record, but none was returned");

                REQUIRE(rec->value("id").toInt() == 1);

                rec =
                    sql_ds::account::find_by_full_path(
                        ds->db()
                        , "assets/savings");

                if (rec == boost::none)
                    FAIL("expected to find record, but none was returned");

                REQUIRE(rec->value("id").toInt() == 2);

                rec =
                    sql_ds::account::find_by_full_path(
                        ds->db()
                        , "expenses");

                if (rec == boost::none)
                    FAIL("expected to find record, but none was returned");

                REQUIRE(rec->value("id").toInt() == 3);

                rec =
                    sql_ds::account::find_by_full_path(
                        ds->db()
                        , "expenses/rent");

                if (rec == boost::none)
                    FAIL("expected to find record, but none was returned");

                REQUIRE(rec->value("id").toInt() == 4);

                // Failure cases - non-existed record paths
                rec =
                    sql_ds::account::find_by_full_path(
                        ds->db()
                        , "");

                if (rec != boost::none)
                    FAIL("retrieved a record when none was expected");

                rec =
                    sql_ds::account::find_by_full_path(
                        ds->db()
                        , "abc");

                if (rec != boost::none)
                    FAIL("retrieved a record when none was expected");

                rec =
                    sql_ds::account::find_by_full_path(
                        ds->db()
                        , "abc/123");

                if (rec != boost::none)
                    FAIL("retrieved a record when none was expected");

                rec =
                    sql_ds::account::find_by_full_path(
                        ds->db()
                        , "abc/");

                if (rec != boost::none)
                    FAIL("retrieved a record when none was expected");

                rec =
                    sql_ds::account::find_by_full_path(
                        ds->db()
                        , "/");

                if (rec != boost::none)
                    FAIL("retrieved a record when none was expected");

                rec =
                    sql_ds::account::find_by_full_path(
                        ds->db()
                        , "/abc");

                if (rec != boost::none)
                    FAIL("retrieved a record when none was expected");

            }   // end find_by_full_path section

        }   // end find records section

    }   // end mixed account types section
    
}   // end Sqlite Account Table tests

TEST_CASE("sqlite account objects", "[unit]")
{
    // Open a new Datastore, and initialise it.
    //
    // Can use an in-memory database, so we can re-use it multiple times.
    QString dbFilePath = ":memory:";
    //QString dbFilePath =
    //    "test-output/sqlite_account-test-sqlite_account_objects.db";

    auto ds = std::make_unique<sql_ds::datastore>(dbFilePath);
    ds->initialise();

    // Account creation - 4 main accounts and 8 subaccounts
    api::account_spr assets_ac = nullptr;
    REQUIRE_NOTHROW(
        assets_ac = ds->create_account(
                L"assets"
                , nullptr
                , L"all assets"
                , api::date(2010, 1, 1)
                , 0.0));

    // Set up a child account of the assets account
    REQUIRE_NOTHROW(
        ds->create_account(
            L"bank"
            , assets_ac
            , L"savings account in bank"
            , api::date(2010, 1, 1)
            , 1000.0));

    REQUIRE_NOTHROW(
        ds->create_account(
            L"cash"
            , assets_ac
            , L"cash in hand"
            , api::date(2010, 1, 1)
            , 50));

    api::account_spr liabilities_ac = nullptr;
    REQUIRE_NOTHROW(liabilities_ac =
        ds->create_account(
            L"liabilities"
            , nullptr
            , L"all liabilities"
            , api::date(2010, 1, 1)
            , 0));


    REQUIRE_NOTHROW(
        ds->create_account(
            L"credit card"
            , liabilities_ac
            , L"credit card"
            , api::date(2010, 1, 1)
            , -100));

    REQUIRE_NOTHROW(
        ds->create_account(
            L"car loan"
            , liabilities_ac
            , L"loan for car"
            , api::date(2010, 1, 1)
            , -2000));

    api::account_spr income_ac = nullptr;
    REQUIRE_NOTHROW(
        income_ac = ds->create_account(L"income", nullptr, L"all income"));
    REQUIRE_NOTHROW(
        ds->create_account(L"salary", income_ac, L"regular salary"));
    REQUIRE_NOTHROW(
        ds->create_account(
            L"contract work"
            , income_ac
            , L"income from side contracts"));

    api::account_spr expenses_ac = nullptr;
    REQUIRE_NOTHROW(
        expenses_ac = ds->create_account(
                        L"expenses"
                        , nullptr
                        , L"all expenses"));
    REQUIRE_NOTHROW(
        ds->create_account(L"rent", expenses_ac, L"accommodation rental"));
    REQUIRE_NOTHROW(
        ds->create_account(
            L"groceries"
            , expenses_ac
            , L"food and essentials"));

    SECTION("account retrieval")
    {
        // Retrieve individual accounts
        auto ac = ds->find_account(L"assets");
        REQUIRE(ac);
        REQUIRE(ac->name() == L"assets");
        REQUIRE(ac->parent_path().empty());
        REQUIRE(ac->full_path() == L"assets");
        REQUIRE(ac->description() == L"all assets");
        REQUIRE(ac->has_running_balance());
        REQUIRE(ac->opening_data()
            == std::make_tuple(api::date(2010, 1, 1), 0.0));

        ac = ds->find_account(L"assets/bank");
        REQUIRE(ac);
        REQUIRE(ac->name() == L"bank");
        REQUIRE(ac->parent_path() == L"assets");
        REQUIRE(ac->full_path() == L"assets/bank");
        REQUIRE(ac->description() == L"savings account in bank");
        REQUIRE(ac->has_running_balance());
        REQUIRE(ac->opening_data()
            == std::make_tuple(api::date(2010, 1, 1), 1000.0));

        // Retrieve children
        //
        // Children at the root.
        auto root_accounts = ds->find_children_of(L"");
        REQUIRE(root_accounts.size() == 4);
        REQUIRE(root_accounts.find(L"assets") != root_accounts.end());
        REQUIRE(root_accounts.find(L"liabilities") != root_accounts.end());
        REQUIRE(root_accounts.find(L"income") != root_accounts.end());
        REQUIRE(root_accounts.find(L"expenses") != root_accounts.end());

        // Children of asset accounts
        auto asset_children = ds->find_children_of(L"assets");
        REQUIRE(asset_children.size() == 2);
        REQUIRE(asset_children.find(L"assets/bank") != asset_children.end());
        REQUIRE(asset_children.find(L"assets/cash") != asset_children.end());

        // Child accounts of accounts with no children
        auto cash_children = ds->find_children_of(L"assets/cash");
        REQUIRE(cash_children.size() == 0);
    }   // end account retrieval section

    SECTION("field value updates")
    {
        // Update the assets account name to "all assets"
        REQUIRE_NOTHROW(assets_ac->set_name(L"all assets"));

        // It's name and full path have changed, and so have the full paths
        // of descendents.
        REQUIRE(assets_ac->name() == L"all assets");
        REQUIRE(assets_ac->full_path() == L"all assets");

        auto bank_ac = ds->find_account(L"all assets/bank");
        REQUIRE(bank_ac);
        REQUIRE(bank_ac->full_path() == L"all assets/bank");
        auto cash_ac = ds->find_account(L"all assets/cash");
        REQUIRE(cash_ac);
        REQUIRE(cash_ac->full_path() == L"all assets/cash");

        // Change back - everything is bank to normal
        REQUIRE_NOTHROW(assets_ac->set_name(L"assets"));
        
        REQUIRE(assets_ac->name() == L"assets");
        REQUIRE(assets_ac->full_path() == L"assets");
        REQUIRE(bank_ac->full_path() == L"assets/bank");
        REQUIRE(cash_ac->full_path() == L"assets/cash");

        // Child rename
        REQUIRE_NOTHROW(cash_ac->set_name(L"my cash"));
        REQUIRE(cash_ac->name() == L"my cash");
        REQUIRE(cash_ac->full_path() == L"assets/my cash");

        REQUIRE_NOTHROW(cash_ac->set_name(L"cash"));
        REQUIRE(cash_ac->name() == L"cash");
        REQUIRE(cash_ac->full_path() == L"assets/cash");

        // Changing parent
        //
        // Move rent to being an income account instead of an expense (the
        // Full Path of the Child changes), and move it back again
        auto rent_ac = ds->find_account(L"expenses/rent");
        REQUIRE(rent_ac);
        REQUIRE_NOTHROW(rent_ac->set_parent(income_ac));

        REQUIRE(rent_ac->full_path() == L"income/rent");

        REQUIRE_NOTHROW(rent_ac->set_parent(expenses_ac));
        REQUIRE(rent_ac->full_path() == L"expenses/rent");

        // Move rent to being a root account.
        REQUIRE_NOTHROW(rent_ac->set_parent(nullptr));
        REQUIRE(rent_ac->full_path() == L"rent");
        REQUIRE(rent_ac->parent_path().empty());

        // Setting running balance - account at root starts with no running
        // balance, and is then given one, and is then changed back.
        auto test_ac =
            ds->create_account(L"test", nullptr, L"running balance test");
        REQUIRE(test_ac->has_running_balance() == false);
        REQUIRE_THROWS_AS(test_ac->opening_data(), api::error);

        REQUIRE_NOTHROW(
            test_ac->set_running_balance_true(api::date(2010, 1, 1), 10.0));
        REQUIRE(test_ac->has_running_balance() == true);
        REQUIRE(
            test_ac->opening_data()
            == std::make_tuple(api::date(2010, 1, 1), 10.0));

        REQUIRE_NOTHROW(test_ac->set_running_balance_false());
        REQUIRE(test_ac->has_running_balance() == false);
        REQUIRE_THROWS_AS(test_ac->opening_data(), api::error);
    }   // end field value updates

    SECTION("account destruction")
    {
        // Delete cash - can't retrieve any more.
        REQUIRE_NOTHROW(ds->destroy_account(L"assets/cash"));
        REQUIRE(ds->find_account(L"assets/cash") == nullptr);

        // Delete bank and assets accounts - can't retrieve those any more
        // either
        REQUIRE_NOTHROW(ds->destroy_account(L"assets/bank"));
        REQUIRE_NOTHROW(ds->destroy_account(L"assets"));

        REQUIRE(ds->find_account(L"assets") == nullptr);
        REQUIRE(ds->find_account(L"assets/bank") == nullptr);
    }

    SECTION("exception cases")
    {
        SECTION("locating non-existent records")
        {
            auto ac = ds->find_account(L"abc");
            REQUIRE(ac == nullptr);
            ac = ds->find_account(L"abc/xyz");
            REQUIRE(ac == nullptr);
            ac = ds->find_account(L"assets/abc");
            REQUIRE(ac == nullptr);
            ac = ds->find_account(L"abc/assets");
            REQUIRE(ac == nullptr);
            ac = ds->find_account(L"");
            REQUIRE(ac == nullptr);
            ac = ds->find_account(L"/abc");
            REQUIRE(ac == nullptr);
            ac = ds->find_account(L"abc/");
            REQUIRE(ac == nullptr);
            ac = ds->find_account(L"/");
            REQUIRE(ac == nullptr);
            ac = ds->find_account(L"/assets");
            REQUIRE(ac == nullptr);
            ac = ds->find_account(L"assets/");
            REQUIRE(ac == nullptr);
        }   // end location non-existent records section

        SECTION("locating children of non-existed records")
        {
            REQUIRE_THROWS_AS(ds->find_children_of(L"abc"), api::error);
            REQUIRE_THROWS_AS(
                ds->find_children_of(L"assets/abc")
                , api::error);
        }

        SECTION("illegal Account naming")
        {
            // Invalid name
            REQUIRE_THROWS_AS(assets_ac->set_name(L"abc/xyz"), api::error);

            // Name is not unique at the root
            REQUIRE_THROWS_AS(
                assets_ac->set_name(L"liabilities")
                , api::error);
            
            // Name is not unique as a child
            auto cash_ac = ds->find_account(L"assets/cash");
            REQUIRE_THROWS_AS(cash_ac->set_name(L"bank"), api::error);
        }   // end illegal Account name section

        SECTION("setting illegal parents")
        {
            // Setting cash under expenses
            auto cash_ac = ds->find_account(L"assets/cash");
            REQUIRE_THROWS_AS(cash_ac->set_parent(expenses_ac), api::error);

            // Setting salary under liabilities
            auto salary_ac = ds->find_account(L"income/salary");
            REQUIRE_THROWS_AS(
                salary_ac->set_parent(liabilities_ac), api::error);

            // Attempt to move another "assets" account to the root.
            salary_ac->set_name(L"assets");
            REQUIRE_THROWS_AS(salary_ac->set_parent(nullptr), api::error);
        }   // end setting illegal parents section

        SECTION("illegal running balance settings")
        {
            // Can't remove running balance from "assets" because it has
            // children
            REQUIRE_THROWS_AS(
                assets_ac->set_running_balance_false()
                , api::error);

            // Can't remove running balance from "cash" because it has a
            // parent
            auto cash_ac = ds->find_account(L"assets/cash");
            REQUIRE_THROWS_AS(
                cash_ac->set_running_balance_false()
                , api::error);

            // Can't add running balance to "income" because it has children
            REQUIRE_THROWS_AS(
                income_ac->set_running_balance_true(
                    api::date(2010, 1, 1)
                    , 10.0)
                , api::error);

            // Can't add running balance to "salary" because it has a parent
            auto salary_ac = ds->find_account(L"income/salary");
            REQUIRE_THROWS_AS(
                salary_ac->set_running_balance_true(
                    api::date(2010, 1, 1)
                    , 10.9)
                , api::error);
        }

        SECTION("illegal account destruction")
        {
            // Can't delete an account with children
            REQUIRE_THROWS_AS(ds->destroy_account(L"assets"), api::error);

            // Can't delete an account that doesn't exist
            REQUIRE_THROWS_AS(ds->destroy_account(L"abc"), api::error);
        }   // end illegal account destruction section

    }   // end exception cases section

}   // end Sqlite Account Objects test
