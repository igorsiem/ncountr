/**
 * \file api/account.h
 * Declare the `account` interface
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <map>
#include <memory>
#include <tuple>
#include <vector>

#include <boost/optional/optional.hpp>

#include <qlib/qlib.h>

#include "currency.h"
#include "date.h"
#include "error.h"

#ifndef _api_account_h_included
#define _api_account_h_included

namespace ncountr { namespace api {

/**
 * \brief Class encapsulating a bookkeeping Account, which is a source and /
 * or destination of funds
 * 
 * \todo Expand documentation
 * 
 * \todo Document account names / paths / identifiers
 * 
 * \todo Document account types and business rules
 */
class account
{

    // --- External Interface ---

    public:

    /**
     * \brief Error exception for signalling issues related to the Datastore
     */
    class error : public api::error
    {

        public:

        /**
         * \brief Constructor - setting human-readable error message
         * 
         * \param msg The human-readable error message
         */
        explicit error(std::string msg) : api::error(msg) {}

        DECLARE_DEFAULT_VIRTUAL_DESTRUCTOR(error)
        DECLARE_DEFAULT_MOVE_AND_COPY_SEMANTICS(error)

    };  // end error class

    DECLARE_DEFAULT_VIRTUAL_LIFE_CYCLE(account)

    /**
     * \brief A shared pointer to an Account object
     */

    using account_spr = std::shared_ptr<account>;

    /**
     * \brief A collection of (shared pointers to) Accounts, indexed by their
     * fully qualified path names
     */
    using accounts_by_path_map = std::map<std::wstring, account_spr>;

    /**
     * \brief Retrieve the Name of the Account
     * 
     * The Account Name uniquely identifies the Account within all the
     * Accounts under its parent. 
     */
    virtual std::wstring name(void) const = 0;

    /**
     * \brief Set the Name of the account
     * 
     * The Account Name uniquely identifies the Account within all the
     * Accounts under its parent, and implementations should check this.
     * 
     * Implementations of this method should validate the Name using the
     * `valid_name` method, and signal an error if when attempting to set
     * the Name to an invalid value.
     */
    virtual void set_name(std::wstring n) = 0;

    /**
     * \brief The character(s) used to separate the names in a 'path' of
     * accounts in an account hierarchy
     */
    static const std::wstring account_path_separator;

    /**
     * \brief Verify that a given string is valid for use as an account name
     * 
     * This method should be called when an account name is set. It checks
     * that a path separator is not included in the name, and may verify
     * other logical inconsistencies in the name.
     */
    static bool valid_name(const std::wstring& n);

    /**
     * \brief Split an account path string into an array of account names
     */
    static std::vector<std::wstring> split_path(const std::wstring& p);

    /**
     * \brief Concatenate a vector of account names into an account path
     * string
     */
    static std::wstring concatenate_path(const std::vector<std::wstring>& p);

    /**
     * \brief Retrieve the full path of the Account that is the parent of
     * this account
     * 
     * If an empty string is returned, this Account is at the root.
     * 
     * Parent paths are always assumed to begin at the root, and do not start
     * with the `account_path_separator`.
     */
    virtual std::wstring parent_path(void) const = 0;

    /**
     * \brief Set the parent account for this account
     * 
     * \todo Document business rules
     */
    virtual void set_parent(account_spr parent) = 0;

    /**
     * \brief Retrieve the Full Path of the Account
     * 
     * The Full Path of the Account acts as a unique key for Accounts within
     * a Datastore. Note that if the Account is at the root (i.e. the Parent
     * Path is empty), then the Full Path is the same as the Account Name.
     */
    virtual std::wstring full_path(void) = 0;

    /**
     * \brief Set the Full Path of the Account
     * 
     * The Full Path of the Account acts as a unique key for Accounts within
     * a Datastore. Note that if the Account is at the root (i.e. the Parent
     * Path is empty), then the Full Path is the same as the Account Name.
     */
    virtual void set_full_path(std::wstring p) = 0;

    /**
     * \brief Retrieve the Account Description
     */
    virtual std::wstring description(void) const = 0;

    /**
     * \brief Set the Account Description string
     */
    virtual void set_description(std::wstring d) = 0;

///    /**
///     * \brief Retrieve the account type enumerator (asset, liability, income
///     * or expense)
///     */
///    virtual type_t account_type(void) const = 0;
///
///    /**
///     * \brief Set the account type (for income and expense accounts)
///     * 
///     * \todo Expand documentation about enforcing business rules
///     */
///    virtual void set_account_type(type_t t) = 0;
///
///    /**
///     * \brief Set the account type (for asset and liability accounts)
///     * 
///     * \todo Expand documentation about enforcing business rules
///     */
///    virtual void set_account_type(
///        type_t t
///        , date opening_date
///        , currency_t opening_balance) = 0;
///
///    /**
///     * \brief Retrieve the opening date (for asset and liability accounts)
///     */
///    virtual date opening_date(void) const = 0;
///
///    /**
///     * \brief Set the opening date (for asset and liability accounts)
///     * 
///     * \todo Expand documentation about enforcing business rules
///     */
///    virtual void set_opening_date(date od) = 0;
///
///    /**
///     * \brief Retrieve the opening balance (for asset and liability accounts)
///     */
///    virtual currency_t opening_balance(void) const = 0;
///
///    /**
///     * \brief Set the opening balance (for asset and liability accounts)
///     * 
///     * \todo Expand documentation about enforcing business rules
///     */
///    virtual void set_opening_balance(currency_t ob) = 0;

    /**
     * \brief Whether or not the Account has a Running Balance
     * 
     * Accounts with a running balance are Assets or Liabilities, and have a
     * positive or negative values at any point in time that contributes to
     * the Total Net Worth. They have an Opening Date and an Opening Balance.
     * 
     * Accounts without a running balance are Income or Expenses. They depict
     * funds going in or out over a set period of time.
     * 
     * \return `true` if the Account has a Running Balance
     * 
     * \todo Document business rules
     */
    virtual bool has_running_balance(void) const = 0;

    /**
     * \brief Set up the Account to have a Running Balance, with an Opening
     * Date and Opening Balance
     * 
     * \param od The Opening Date of the Account
     * 
     * \param ob The Opening Balance of the Account
     * 
     * \todo Document business rules
     */
    virtual void set_running_balance_true(date od, currency_t ob) = 0;

    /**
     * \brief Set the Account to have no Running Balance (i.e. to be an
     * Income or Expense Account)
     * 
     * \todo Document business rules
     */
    virtual void set_running_balance_false(void) = 0;

    /**
     * \brief Retrieve the Opening Data (Opening Date and Opening Balance)
     * for an Account; this method should only be called for Running Balance
     * Accounts
     * 
     * \return A tuple with the Opening Date and Opening Balance
     * 
     * \throw account::error The Account does not have a Running Balance
     * 
     * \todo Document business rules
     */
    virtual std::tuple<date, currency_t> opening_data(void) const = 0;

};  // end account class

/**
 * \brief A shared pointer to an Account object
 */
using account_spr = account::account_spr;

/**
 * \brief A collection (shared pointers to) Accounts, indexed by the full
 * path
 */
using accounts_by_path_map = account::accounts_by_path_map;

}}  // end ncountr::api namespace

#endif
