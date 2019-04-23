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
 * \todo Document Account hierarchy, names / paths / identifiers
 * 
 * \todo Document account types, along with the rule that asset / liability
 * accounts should not have income / expense accounts as children, and
 * vice-versa; not enforced at the API level; also explain that Asset /
 * Liability accounts *must* have an opening date and balance, while Income
 * and Expense Accounts *must not* have these items. These business rules
 * are enforced in the runtime implementation, rather than an hierarchical
 * Account class structure.
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

    /**
     * \brief Enumerate different types of account
     */
    enum class type_t
    {
        asset           ///< Assets, e.g. bank account, cash
        , liability     ///< Liabilities, e.g. loans, credit cards
        , income        ///< Income, e.g. Salary
        , expense       ///< Expenses, e.g. Rent
    };  // end type_t enumeration

    DECLARE_DEFAULT_VIRTUAL_LIFE_CYCLE(account)

    /**
     * \brief A shared pointer to an Account object
     */
    using account_spr = std::shared_ptr<account>;

    /**
     * \brief A vector of (shared pointers to) Account objects
     */
    using accounts_vec_t = std::vector<account_spr>;

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
     * \brief Retrieve the Parent of an Account (or `nullptr` if the Account
     * is at the root)
     */
    virtual account_spr parent(void) const = 0;

    /**
     * \brief Set the parent account for this account
     * 
     * When setting the parent, the implementation must:
     * 
     * * Allow the parent to be `nullptr` - this means that the account will
     *   have no parent, and be at the root of the Accounts Tree
     * 
     * * Ensure that balance (asset and liability) accounts only have balance
     *   accounts as parents, and with the same condition for non-balance
     *   (income and expense) accounts.
     * 
     * * Ensure that the Account's Name is unique in the set of sibling
     *   Accounts
     */
    virtual void set_parent(account_spr parent) = 0;

    /**
     * \brief Retrieve the direct Child Accounts of this Account
     * 
     * A Child Account is an Account that has this Account as a direct Parent
     */
    virtual accounts_vec_t children(void) const = 0;

    /**
     * \brief Retrieve the descendent Accounts of this Account
     */
    virtual accounts_vec_t descendants(void) const = 0;

    /**
     * \brief Retrieve the Full Path of the Account
     * 
     * The Full Path of the Account acts as a unique key for Accounts within
     * a Datastore. Note that if the Account is at the root (i.e. the Parent
     * Path is empty), then the Full Path is the same as the Account Name.
     */
    virtual std::wstring full_path(void) const = 0;

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

    /**
     * \brief Retrieve the account type enumerator (asset, liability, income
     * or expense)
     */
    virtual type_t account_type(void) const = 0;

    /**
     * Set the Account Type for Income and Expense Accounts
     * 
     * Implementatons of this method must enforce Business Rules associated
     * with Account Types.
     */
    virtual void set_account_type(type_t t) = 0;

    /**
     * \brief Set the Account Type for Income and Expense Accounts, as well
     * as the Opening Date and Balance
     * 
     * Implementatons of this method must enforce Business Rules associated
     * with Account Types.
     */
    virtual void set_account_type(
        type_t t
        , date opening_date
        , currency_t opening_balance) = 0;

    /**
     * \brief Determine whether two Account Types are compatible for
     * parent-child relationships
     */
    static bool compatible_parent_child_types(type_t t1, type_t t2)
    {
        if ((t1 == type_t::asset) || (t1 == type_t::liability))
        {
            if ((t2 == type_t::asset) || (t2 == type_t::liability))
                return true;
            else return false;
        }
        else
        {
            if ((t2 == type_t::asset) || (t2 == type_t::liability))
                return false;
            else return true;

        }
    }   // end compatible_parent_child_types methods

    /**
     * \brief Retrieve the Opening Date of an Asset or Liability Account
     * 
     * This method should not be used with Income or Expense Accounts
     */
    virtual date opening_date(void) const = 0;

    /**
     * \brief Set the Opening Date of an Asset or Liability Account
     * 
     * This method should not be used with Income or Expense Accounts
     */
    virtual void set_opening_date(date od) = 0;

    /**
     * \brief Retrieve the Opening Balance of an Asset or Liability Account
     * 
     * This method should not be used with Income or Expense Accounts
     */
    virtual currency_t opening_balance(void) const = 0;

    /**
     * \brief Set the Opening Balance of an Asset or Liability Account
     * 
     * This method should not be used with Income or Expense Accounts
     */
    virtual void set_opening_balance(currency_t ob) = 0;

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

/**
 * \brief A vector of (shared pointers to) Account objects
 */
using accounts_vec_t = account::accounts_vec_t;

}}  // end ncountr::api namespace

#endif
