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
                , ncountr::api::account::type_t::income));

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

        // Check the account type is 'income', using the appropriate string
        // transformation
        auto accout_type_idx = ar->indexOf("account_type");
        REQUIRE(
            ar->value(accout_type_idx).value<QString>()
            == ncountr::datastores::sqlite::account::to_qstring(
                ncountr::api::account::type_t::income));

        // Attempt to retrieve a non-existent record
        ar =
            ncountr::datastores::sqlite::account::find_by_id(
                ds->db()
                , 2);                

        if (ar != boost::none) FAIL("expected no record to be returned");

        // Add an asset account now
        REQUIRE_NOTHROW(ncountr::datastores::sqlite::account::create_record(
            ds->db()
            , 2
            , "xyz"
            , boost::none
            , ncountr::api::account::type_t::asset
            , ncountr::datastores::sqlite::to_qdate(
                ncountr::api::date(1970, ncountr::api::Oct, 5))
            , 1.1));

        // Retrieve the record by full path
        ar = ncountr::datastores::sqlite::account::find_by_full_path(
            ds->db(), "xyz");
        if (ar == boost::none) FAIL("expected to retrieve a record");

        // Check the account type is 'asset', using the appropriate string
        // transformation
        accout_type_idx = ar->indexOf("account_type");
        REQUIRE(
            ar->value(accout_type_idx).value<QString>()
            == ncountr::datastores::sqlite::account::to_qstring(
                ncountr::api::account::type_t::asset));
        
        // Check the opening balance field value
        auto opening_balance_idx = ar->indexOf("opening_balance");
        REQUIRE(
            ar->value(opening_balance_idx).value<ncountr::api::currency_t>()
            == 1.1);

        // Destroy the first record, and attempt to locate it again.
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

    SECTION("high-level operations")
    {
        // Open the Datastore and initialise it.
        //
        // Can use an in-memory database or a file by uncommenting the
        // appropriate line.
        QString dbFilePath =
            "test-output/sqlite_datastore-test-sqlite_account-2.db";
            // ":memory:";

        auto ds =
            std::make_unique<ncountr::datastores::sqlite::datastore>(
                dbFilePath);
        ds->initialise();

        // Create base accounts of all four types
        auto assets_ac = ncountr::datastores::sqlite::account::make_new(
            ds->db()
            , "assets"
            , ncountr::api::account::type_t::asset
            , nullptr
            , QString("all assets")
            , ncountr::datastores::sqlite::to_qdate(
                ncountr::api::date(2010, 10, 1))
            , 0.0);

        auto liabilities_ac = ncountr::datastores::sqlite::account::make_new(
            ds->db()
            , "liabilities"
            , ncountr::api::account::type_t::liability
            , nullptr
            , QString("all liabilities")
            , ncountr::datastores::sqlite::to_qdate(
                ncountr::api::date(2010, 10, 1))
            , 0.0);
        
        auto income_ac = ncountr::datastores::sqlite::account::make_new(
            ds->db()
            , "income"
            , ncountr::api::account::type_t::income
            , nullptr
            , QString("all income"));

        auto expenses_ac = ncountr::datastores::sqlite::account::make_new(
            ds->db()
            , "expenses"
            , ncountr::api::account::type_t::expense
            , nullptr
            , QString("all expenses"));

        // Make sure we got all the account objects, and that they are
        // valid / ready
        REQUIRE(assets_ac);
        REQUIRE(liabilities_ac);
        REQUIRE(income_ac);
        REQUIRE(expenses_ac);

        REQUIRE(assets_ac->is_ready());
        REQUIRE(liabilities_ac->is_ready());
        REQUIRE(income_ac->is_ready());
        REQUIRE(expenses_ac->is_ready());

        // Check all our fields now
        REQUIRE(assets_ac->name() == L"assets");
        REQUIRE(assets_ac->parent_path().empty());
        REQUIRE(assets_ac->full_path() == L"assets");
        REQUIRE(assets_ac->description()
            == L"all assets");
        REQUIRE(assets_ac->account_type()
            == ncountr::api::account::type_t::asset);
        REQUIRE(assets_ac->opening_date()
            == ncountr::api::date(2010, 10, 1));
        REQUIRE(assets_ac->opening_balance() == 0.0);

        REQUIRE(liabilities_ac->name() == L"liabilities");
        REQUIRE(liabilities_ac->parent_path().empty());
        REQUIRE(liabilities_ac->full_path() == L"liabilities");
        REQUIRE(liabilities_ac->description()
            == L"all liabilities");
        REQUIRE(liabilities_ac->account_type()
            == ncountr::api::account::type_t::liability);
        REQUIRE(liabilities_ac->opening_date()
            == ncountr::api::date(2010, 10, 1));
        REQUIRE(liabilities_ac->opening_balance() == 0.0);

        REQUIRE(income_ac->name() == L"income");
        REQUIRE(income_ac->parent_path().empty());
        REQUIRE(income_ac->full_path() == L"income");
        REQUIRE(income_ac->description()
            == L"all income");
        REQUIRE(income_ac->account_type()
            == ncountr::api::account::type_t::income);

        REQUIRE(expenses_ac->name() == L"expenses");
        REQUIRE(expenses_ac->parent_path().empty());
        REQUIRE(expenses_ac->full_path() == L"expenses");
        REQUIRE(expenses_ac->description()
            == L"all expenses");
        REQUIRE(expenses_ac->account_type()
            == ncountr::api::account::type_t::expense);

        // Attempting to create an Account with a duplicate Full Path
        REQUIRE_THROWS_AS(
            ncountr::datastores::sqlite::account::make_new(
                ds->db()
                , "assets"
                , ncountr::api::account::type_t::asset
                , nullptr
                , QString("description of assets account")
                , ncountr::datastores::sqlite::to_qdate(
                    ncountr::api::date(2010, 10, 1))
                , 0.0)
            , ncountr::datastores::sqlite::account::error);

        REQUIRE_THROWS_AS(
            ncountr::datastores::sqlite::account::make_new(
                ds->db()
                , "liabilities"
                , ncountr::api::account::type_t::liability
                , nullptr
                , QString("description of liabilities account")
                , ncountr::datastores::sqlite::to_qdate(
                    ncountr::api::date(2010, 10, 1))
                , 0.0)
            , ncountr::datastores::sqlite::account::error);

        REQUIRE_THROWS_AS(
            ncountr::datastores::sqlite::account::make_new(
                ds->db()
                , "income"
                , ncountr::api::account::type_t::income
                , nullptr
                , QString("description of income account"))
            , ncountr::datastores::sqlite::account::error);

        REQUIRE_THROWS_AS(
            ncountr::datastores::sqlite::account::make_new(
                ds->db()
                , "expenses"
                , ncountr::api::account::type_t::expense
                , nullptr
                , QString("description of expenses account"))
            , ncountr::datastores::sqlite::account::error);

        // Create some sub-accounts
        auto bank_ac = ncountr::datastores::sqlite::account::make_new(
            ds->db()
            , "bank account"
            , ncountr::api::account::type_t::asset
            , assets_ac
            , QString("my bank account")
            , ncountr::datastores::sqlite::to_qdate(
                ncountr::api::date(2010, 10, 1))
            , 10000.0);

        auto cash_ac = ncountr::datastores::sqlite::account::make_new(
            ds->db()
            , "cash"
            , ncountr::api::account::type_t::asset
            , assets_ac
            , QString("cash on hand")
            , ncountr::datastores::sqlite::to_qdate(
                ncountr::api::date(2010, 10, 1))
            , 250.0);

        auto credit_card_ac = ncountr::datastores::sqlite::account::make_new(
            ds->db()
            , "credit card"
            , ncountr::api::account::type_t::liability
            , liabilities_ac
            , QString("my credit card")
            , ncountr::datastores::sqlite::to_qdate(
                ncountr::api::date(2010, 10, 1))
            , -200.0);

        auto car_loan_ac = ncountr::datastores::sqlite::account::make_new(
            ds->db()
            , "car loan"
            , ncountr::api::account::type_t::liability
            , liabilities_ac
            , QString("loan for my car")
            , ncountr::datastores::sqlite::to_qdate(
                ncountr::api::date(2010, 10, 1))
            , -5500.00);

        auto salary_ac = ncountr::datastores::sqlite::account::make_new(
            ds->db()
            , "salary"
            , ncountr::api::account::type_t::income
            , income_ac
            , QString("my regular salary"));

        auto contract_work_ac =
            ncountr::datastores::sqlite::account::make_new(
                ds->db()
                , "contract work"
                , ncountr::api::account::type_t::income
                , income_ac
                , QString("income from contract jobs"));

        auto rent_ac = ncountr::datastores::sqlite::account::make_new(
            ds->db()
            , "rent"
            , ncountr::api::account::type_t::expense
            , expenses_ac
            , QString("rent"));

        auto interest_ac = ncountr::datastores::sqlite::account::make_new(
            ds->db()
            , "interest"
            , ncountr::api::account::type_t::expense
            , expenses_ac
            , QString("interest on loans and credit card"));

        // All account objects were created successfully, and that they are
        // ready for further operations
        REQUIRE(bank_ac);
        REQUIRE(cash_ac);
        REQUIRE(credit_card_ac);
        REQUIRE(car_loan_ac);
        REQUIRE(salary_ac);
        REQUIRE(contract_work_ac);
        REQUIRE(rent_ac);
        REQUIRE(interest_ac);

        REQUIRE(bank_ac->is_ready());
        REQUIRE(cash_ac->is_ready());
        REQUIRE(credit_card_ac->is_ready());
        REQUIRE(car_loan_ac->is_ready());
        REQUIRE(salary_ac->is_ready());
        REQUIRE(contract_work_ac->is_ready());
        REQUIRE(rent_ac->is_ready());
        REQUIRE(interest_ac->is_ready());

        // At this point, we can query for the root Account Records, and
        // receive the 4 top-level Accounts.
        REQUIRE(ds->root_accounts().size() == 4);

        // Check children method - top-level accounts have two children
        // each; lower-level accounts have none
        //
        // TODO check descendents as distinct from children
        REQUIRE(assets_ac->children().size() == 2);
        REQUIRE(liabilities_ac->children().size() == 2);
        REQUIRE(income_ac->children().size() == 2);
        REQUIRE(expenses_ac->children().size() == 2);

        REQUIRE(bank_ac->children().size() == 0);        
        REQUIRE(cash_ac->children().size() == 0);
        REQUIRE(credit_card_ac->children().size() == 0);
        REQUIRE(car_loan_ac->children().size() == 0);
        REQUIRE(salary_ac->children().size() == 0);
        REQUIRE(contract_work_ac->children().size() == 0);
        REQUIRE(rent_ac->children().size() == 0);
        REQUIRE(interest_ac->children().size() == 0);

        // Check full paths
        REQUIRE(bank_ac->full_path() == L"assets/bank account");
        REQUIRE(cash_ac->full_path() == L"assets/cash");
        REQUIRE(credit_card_ac->full_path() == L"liabilities/credit card");
        REQUIRE(car_loan_ac->full_path() == L"liabilities/car loan");
        REQUIRE(salary_ac->full_path() == L"income/salary");
        REQUIRE(contract_work_ac->full_path() == L"income/contract work");
        REQUIRE(rent_ac->full_path() == L"expenses/rent");
        REQUIRE(interest_ac->full_path() == L"expenses/interest");

        // Exception cases
        //
        // - Attempt to create an Income or Expense Account as a child of
        //   a Liability or Asset Account
        //
        // - Attempt to create an Asset or Liability Account as a child of
        //   an Income or Expense Account

        REQUIRE_THROWS_AS(
            ncountr::datastores::sqlite::account::make_new(
                ds->db()
                , "some more income"
                , ncountr::api::account::type_t::income
                , assets_ac
                , QString(""))
            , ncountr::datastores::sqlite::account::error);

        REQUIRE_THROWS_AS(
            ncountr::datastores::sqlite::account::make_new(
                ds->db()
                , "another expense"
                , ncountr::api::account::type_t::expense
                , liabilities_ac
                , QString(""))
            , ncountr::datastores::sqlite::account::error);

        REQUIRE_THROWS_AS(
            ncountr::datastores::sqlite::account::make_new(
            ds->db()
                , "another asset"
                , ncountr::api::account::type_t::asset
                , income_ac
                , QString("")
                , ncountr::datastores::sqlite::to_qdate(
                    ncountr::api::date(2010, 10, 1))
                , 0.0)
            , ncountr::datastores::sqlite::account::error);

        REQUIRE_THROWS_AS(
            ncountr::datastores::sqlite::account::make_new(
            ds->db()
                , "more liability"
                , ncountr::api::account::type_t::liability
                , expenses_ac
                , QString("")
                , ncountr::datastores::sqlite::to_qdate(
                    ncountr::api::date(2010, 10, 1))
                , 0.0)
            , ncountr::datastores::sqlite::account::error);

        // Setting account name - affects the Full Path of Child Accounts as 
        // well.
        assets_ac->set_name(L"asset accounts");
        REQUIRE(assets_ac->name() == L"asset accounts");
        REQUIRE(bank_ac->full_path() ==
            std::wstring(L"asset accounts/bank account"));
        REQUIRE(cash_ac->full_path() == L"asset accounts/cash");

        assets_ac->set_name(L"assets");
        REQUIRE(bank_ac->full_path() == L"assets/bank account");
        REQUIRE(cash_ac->full_path() == L"assets/cash");

        expenses_ac->set_name(L"my expenses");
        REQUIRE(expenses_ac->name() == L"my expenses");
        REQUIRE(rent_ac->full_path() == L"my expenses/rent");
        REQUIRE(interest_ac->full_path() == L"my expenses/interest");

        expenses_ac->set_name(L"expenses");
        REQUIRE(rent_ac->full_path() == L"expenses/rent");
        REQUIRE(interest_ac->full_path() == L"expenses/interest");

        // Change account types (then change them back again)
        REQUIRE_NOTHROW(
            assets_ac->set_account_type(
                ncountr::api::account::type_t::liability
                , assets_ac->opening_date()
                , assets_ac->opening_balance()));
        REQUIRE(assets_ac->account_type()
            == ncountr::api::account::type_t::liability);
        REQUIRE_NOTHROW(
            assets_ac->set_account_type(
                ncountr::api::account::type_t::asset
                , assets_ac->opening_date()
                , assets_ac->opening_balance()));
        REQUIRE(assets_ac->account_type()
            == ncountr::api::account::type_t::asset);

        REQUIRE_NOTHROW(
            income_ac->set_account_type(
                ncountr::api::account::type_t::expense));
        REQUIRE(income_ac->account_type()
            == ncountr::api::account::type_t::expense);
        REQUIRE_NOTHROW(
            income_ac->set_account_type(
                ncountr::api::account::type_t::income));
        REQUIRE(income_ac->account_type()
            == ncountr::api::account::type_t::income);

        // Move accounts around in the tree
        REQUIRE_NOTHROW(bank_ac->set_parent(liabilities_ac));
        REQUIRE(bank_ac->full_path() == L"liabilities/bank account");
        REQUIRE_NOTHROW(bank_ac->set_parent(assets_ac));
        REQUIRE(bank_ac->full_path() == L"assets/bank account");

        REQUIRE_NOTHROW(rent_ac->set_parent(income_ac));
        REQUIRE(rent_ac->full_path() == L"income/rent");
        REQUIRE_NOTHROW(rent_ac->set_parent(expenses_ac));
        REQUIRE(rent_ac->full_path() == L"expenses/rent");

        // More exception cases...
        //
        // - Attempt to set an Income or Expense Account that is a parent or
        //   child of another Income or Expense Account to being an Asset or
        //   Liability Account
        //
        // - Attempt to set an Asset or Liability Account that is a parent or
        //   child of another Asset or Liability Account to being an Income
        //   or Expense Account.
        //
        // - Attempt to move an Asset or Liability Account to being a child
        //   of an Income or Expense Account
        //
        // - Attempt to move an Income or Expense Account to being a child of
        //   an Asset or Liability Account
        REQUIRE_THROWS_AS(
            assets_ac->set_account_type(
                ncountr::api::account::type_t::income)
            , ncountr::datastores::sqlite::account::error);
        REQUIRE_THROWS_AS(
            bank_ac->set_account_type(
                ncountr::api::account::type_t::income)
            , ncountr::datastores::sqlite::account::error);

        REQUIRE_THROWS_AS(
            expenses_ac->set_account_type(
                ncountr::api::account::type_t::liability
                , ncountr::api::date(2000, 1, 1)
                , 0.0)
            , ncountr::datastores::sqlite::account::error);
        REQUIRE_THROWS_AS(
            interest_ac->set_account_type(
                ncountr::api::account::type_t::liability
                , ncountr::api::date(2000, 1, 1)
                , 0.0)
            , ncountr::datastores::sqlite::account::error);

        REQUIRE_THROWS_AS(
            bank_ac->set_parent(income_ac)
            , ncountr::datastores::sqlite::account::error);

        REQUIRE_THROWS_AS(
            rent_ac->set_parent(liabilities_ac)
            , ncountr::datastores::sqlite::account::error);

        // Account Deletion
        REQUIRE(cash_ac->is_ready() == true);
        REQUIRE_NOTHROW(cash_ac->destroy());
        REQUIRE(cash_ac->is_ready() == false);
        if (ncountr::datastores::sqlite::account::find_by_full_path(
                ds->db()
                , "assets/cash") != boost::none)
            FAIL("retrieved a valid record when none was expected");

        // Except case - can't destroy an Account with Children
        REQUIRE_THROWS_AS(
            liabilities_ac->destroy()
            , ncountr::datastores::sqlite::account::error);

        // Liabilities Account is still ready after failed destruction
        REQUIRE(liabilities_ac->is_ready());

    }   // end high-level section

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
