/**
 * \file sqlite_datastore/account.h
 * Declare the `sqlite::account` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <functional>

#include <boost/optional.hpp>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QString>

#include <fmt/format.h>
using namespace fmt::literals;

#include "../api.h"

#ifndef _sqlite_account_h_included
#define _sqlite_account_h_included

namespace ncountr { namespace datastores { namespace sqlite {

/**
 * \brief An Account in the SQLite datatastore
 * 
 * This class implements the `api::account` interface, and encapsulates the
 * `account` table in the Sqlite database. Objects of this class encapsulate
 * individual Accounts, and records in the `account` table.
 */
class account : public api::account
{

    // --- External Definitions ---

    public:

    using base_t = api::account;    ///< Convenient base-class definition

    /**
     * \brief Error exception for signalling errors related to a Sqlite
     * Account
     * 
     * This sub-class modifies the interface slightly to allow initalisation
     * using a `QString`.
     */
    class error : public base_t::error
    {
        public:

        /**
         * \brief Constructor, setting the exception object with a QString
         * 
         * \param msg The human-readable error message
         */
        explicit error(QString msg) : base_t::error(msg.toStdString()) {}

        /**
         * \brief Constructor, setting the exception object with a
         * std::string
         * 
         * \param msg The human-readable error message
         */
        explicit error(std::string msg) : base_t::error(msg) {}

        DECLARE_DEFAULT_VIRTUAL_DESTRUCTOR(error)
        DECLARE_DEFAULT_MOVE_AND_COPY_SEMANTICS(error)

    };  // end error class

    /**
     * \brief A shared pointer to an Account object
     */
    using account_spr = std::shared_ptr<account>;

///    /**
///     * \brief An invocable object for notifying external components that this
///     * object is being deleted; the argument is a reference to self
///     */
///    using DestructNotifierFn = std::function<void(account&)>;

    /**
     * \brief Constructor for an Account that references an existing Record
     * 
     * This is declared non-public, because several different static methods
     * are used to construct account objects
     * 
     * \param db A reference to the database connection
     * 
     * \param id The ID of the account record; this *must* reference a valid
     * database Account record
     */
    explicit account(
        QSqlDatabase& db
        , int id);

    static account_spr make_new(
        QSqlDatabase& db
        , QString name
        , type_t t
        , account_spr parent
        , boost::optional<QString> description);

    static account_spr make_new(
        QSqlDatabase& db
        , QString name
        , type_t t
        , account_spr parent
        , boost::optional<QString> description
        , ncountr::api::date od
        , ncountr::api::currency_t ob);

    static account_spr find_existing(QString path);

    /**
     * \brief Trivial destructor
     */
    virtual ~account(void);

    /**
     * \brief Perform Account table table initialisation for a new Database
     * 
     * \param db The database to initialised; this is assumed *not* to
     * already contain an `account` table
     */
    static void initialise(QSqlDatabase& db);

    // -- Fields --

    /**
     * \brief Retrieve the Name of the Account
     * 
     * The Account Name uniquely identifies the Account within all the
     * Accounts under its parent. 
     */
    virtual std::wstring name(void) const override
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));
    }

    /**
     * \brief Set the Name of the account
     * 
     * The Account Name uniquely identifies the Account within all the
     * Accounts under its parent, and implementations should check this.
     * 
     * This method should validates the Name using the `valid_name` method.
     */
    virtual void set_name(std::wstring n) override
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));
    }

    /**
     * \brief Retrieve the full path of the Account that is the parent of
     * this account
     * 
     * If an empty string is returned, this Account is at the root.
     * 
     * Parent paths are always assumed to begin at the root, and do not start
     * with the `account_path_separator`.
     */
    virtual std::wstring parent_path(void) const override
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));
    }

///    /**
///     *  \brief Set the full path of the parent Account
///     * 
///     * If this is an empty string, the Account is at the root.
///     * 
///     * Parent paths are always assumed to begin at the root, and need not
///     * start with the `account_path_separator`.
///     */
///    virtual void set_parent_path(std::wstring& p) override
///    {
///        throw error(
///            QString(__FUNCTION__) + tr(" function not implemented yet"));
///    }

    virtual void set_parent(account_spr parent)
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));
    }

    /**
     * \brief Retrieve the Full Path of the Account
     * 
     * The Full Path of the Account acts as a unique key for Accounts within
     * a Datastore. Note that if the Account is at the root (i.e. the Parent
     * Path is empty), then the Full Path is the same as the Account Name.
     */
    virtual std::wstring full_path(void) override
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));
    }

    /**
     * \brief Set the Full Path of the Account
     * 
     * The Full Path of the Account acts as a unique key for Accounts within
     * a Datastore. Note that if the Account is at the root (i.e. the Parent
     * Path is empty), then the Full Path is the same as the Account Name.
     */
    virtual void set_full_path(std::wstring p)
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));
    }   // end set_full_path method

    /**
     * \brief Retrieve the Account Description
     */
    virtual std::wstring description(void) const override
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));
    }

    /**
     * \brief Set the Account Description string
     */
    virtual void set_description(std::wstring d) override
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));
    }

    /**
     * \brief Retrieve the account type enumerator (asset, liability, income
     * or expense)
     */
    virtual type_t account_type(void) const override
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));
    }    

    virtual void set_account_type(type_t t) override
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));
    }

    virtual void set_account_type(
            type_t t
            , ncountr::api::date opening_date
            , ncountr::api::currency_t opening_balance) override
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));        
    }

    virtual ncountr::api::date opening_date(void) const override
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));        
    }

    virtual void set_opening_date(ncountr::api::date od) override
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));        
    }

    virtual ncountr::api::currency_t opening_balance(void) const override
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));        
    }

    virtual void set_opening_balance(ncountr::api::currency_t ob) override
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));        
    }

    static QString to_qstring(type_t t);

    static type_t to_account_type(QString str);

    // -- Lower-level Database Services --

    // TODO Note in all documentation that higher-level business rules (e.g.
    // that Opening Info is supplied if and only if the Account Type is Asset
    // Liability) are NOT ENFORCED AT THIS LOW LEVEL
    //
    // All business rule checks are done in the methods that call these
    // lower-level methods

    int max_id(void) const;

    static int max_id(QSqlDatabase& db);

    static void create_record(
        QSqlDatabase& db
        , int id
        , QString full_path
        , boost::optional<QString> description
        , type_t t);

    static void create_record(
        QSqlDatabase& db
        , int id
        , QString full_path
        , boost::optional<QString> description
        , type_t t
        , ncountr::api::date od
        , ncountr::api::currency_t ob);

    static boost::optional<QSqlRecord> find_by_id(
        QSqlDatabase& db
        , int id);

    static boost::optional<QSqlRecord> find_by_full_path(
        QSqlDatabase& db
        , QString full_path);

    static void destroy_record_by_id(QSqlDatabase& db, int id);

    static void destroy_record_by_full_path(
        QSqlDatabase& db, QString full_path);

    // --- Internal Declarations ---

    protected:

    // -- Attributes --

    QSqlDatabase& m_db; ///< The database for our data

    /**
     * \brief The Identifier for the Account Record
     * 
     * Note that this may be absent (`boost::none`) for a new Account Record
     * that has not yet been saved
     */
    boost::optional<int> m_id;

    Q_DECLARE_TR_FUNCTIONS(account)

};  // end account

/**
 * \brief A shared pointer to an Account object
 */
using account_spr = account::account_spr;

}}} // end ncountr::datastores::sqlite namespace

#endif
