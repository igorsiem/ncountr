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
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>

#include <fmt/format.h>
using namespace fmt::literals;

#include "../api.h"
#include "db_utils.h"

#ifndef _sqlite_account_h_included
#define _sqlite_account_h_included

namespace ncountr { namespace datastores { namespace sqlite {

/**
 * \brief An Account in the SQLite datatastore
 * 
 * This class implements the `api::account` interface, and encapsulates the
 * `account` table in the Sqlite database. Objects of this class encapsulate
 * individual Accounts, and records in the `account` table.
 * 
 * This class includes low-level database implementations of basic CRUD
 * operations. These *do not* impose high-level business rules from the API
 * (e.g. Asset / Liability Accounts with Income / Expense Accounts). These
 * rules are imposed by the higher-level implementations of the base-class
 * methods.
 * 
 * \todo Expand documentation explaining ephemeral treatment of Accounts, and
 * how Sqlite Account objects are interfaces to DB records
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

    /**
     * \brief A vector of (shared pointers to) Account objects
     */
    using accounts_vec_t = std::vector<account_spr>;

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

    /**
     * \brief Create a new Income or Expense Account Record in the database,
     * returning the `account` object
     * 
     * Note that if an Asset or Liability Account is required, the other
     * overload of this method (which takes Opening Date and Balance) must be
     * used.
     * 
     * \param db The database object in which to create the new Account
     * Record
     * 
     * \param name The name of the Account; this must be unique amongst the
     * children of the `parent` Account, or unique at the root of the
     * collection of Accouts if no Parent is set
     * 
     * \param t The type of Account; this *must* be `income` or `expense`,
     * or an exception is thrown
     * 
     * \param parent The Parent Account; this may be set to `nullptr` if the
     * new Account is to be created at the root
     * 
     * \param description An optional human-readable description of the
     * Account
     * 
     * \return A shared pointer to the new Account object that can be used
     * to access the new Record
     * 
     * \throws ncountr::datastores::sqlite::account::error Some problem
     * occurred while attempting to create the new Account
     */
    static account_spr make_new(
        QSqlDatabase& db
        , QString name
        , type_t t
        , account_spr parent
        , boost::optional<QString> description);

    /**
     * \brief Create a new Asset or Liability Account Record in the database,
     * returning the `account` object
     * 
     * Note that if an Income or Expense Account is required, the other
     * overload of this method (not taking an Opening Date or Balance) must
     * be used.
     * 
     * \param db The database object in which to create the new Account
     * Record
     * 
     * \param name The name of the Account; this must be unique amongst the
     * children of the `parent` Account, or unique at the root of the
     * collection of Accouts if no Parent is set
     * 
     * \param t The type of Account; this *must* be `income` or `expense`,
     * or an exception is thrown
     * 
     * \param parent The Parent Account; this may be set to `nullptr` if the
     * new Account is to be created at the root
     * 
     * \param description An optional human-readable description of the
     * Account
     * 
     * \param od The opening date for the Account
     * 
     * \param ob The opening balance for the Account
     * 
     * \return A shared pointer to the new Account object that can be used
     * to access the new Record
     * 
     * \throws ncountr::datastores::sqlite::account::error Some problem
     * occurred while attempting to create the new Account
     */
    static account_spr make_new(
        QSqlDatabase& db
        , QString name
        , type_t t
        , account_spr parent
        , boost::optional<QString> description
        , QDate od
        , double ob);

    /**
     * \brief Locate an existing Account record by its Full Path, and create
     * an `account` object to access it (as a shared pointer)
     */
    static account_spr find_existing(QSqlDatabase& db, QString full_path);

    /**
     * \brief Check whether the Account object can be used
     * 
     * This method checks that the database is open, and that the record
     * ID is greater than zero. It does *not* check that the Record is
     * actually present in the Database.
     */
    virtual bool is_ready(void) const override;

    /**
     * \brief Destroy the underlying storage associated with this record
     * 
     * This method checks to see that the Account has no Descendants, and
     * then deletes the Record. It also sets the record ID to zero, so that
     * `is_ready` will return `false.
     * 
     * \throws error An attempt was made to destroy an Account that has
     * Descendants
     */
    virtual void destroy(void);

    /**
     * \brief Trivial destructor
     * 
     * Note that this method destroys the Account object that is used to
     * access the Account Record, but does not destroy the Record itself.
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
     * \brief Retrieve the record ID of the Account
     */
    int id(void) const { return m_id; }

    /**
     * \brief Retrieve the Name of the Account
     * 
     * The Account Name uniquely identifies the Account within all the
     * Accounts under its parent.
     */
    virtual std::wstring name(void) const override;

    /**
     * \brief Set the Name of the account
     * 
     * The Account Name uniquely identifies the Account within all the
     * Accounts under its parent, and implementations should check this.
     * 
     * This method should validates the Name using the `valid_name` method.
     */
    virtual void set_name(std::wstring n) override;

    /**
     * \brief Retrieve the full path of the Account that is the parent of
     * this account
     * 
     * If an empty string is returned, this Account is at the root.
     * 
     * Parent paths are always assumed to begin at the root, and do not start
     * with the `account_path_separator`.
     */
    virtual std::wstring parent_path(void) const override;

    /**
     * \brief Retrieve the Parent Account of this Account (or `nullptr` if
     * this Account is at the root)
     */
    virtual base_t::account_spr parent(void) const override;

    /**
     * \brief Retrieve the Parent Account of this Account (or `nullptr` if
     * this Account is at the root)
     */
    account_spr parent_sqlite(void) const;

    /**
     * \brief Set the Parent Account of this Account
     * 
     * If the Parent is set to `nullptr`, then this Account is placed at the
     * root. Note that the Parent Account must have a compatible Account
     * Type.
     */
    virtual void set_parent(api::account_spr parent) override;

    /**
     * \brief Retrieve all Accounts that are direct children of this Account
     */
    virtual base_t::accounts_vec_t children(void) const override;

    /**
     * \brief Retrieve all Accounts that are Descendants of this Account
     */
    virtual base_t::accounts_vec_t descendants(void) const override;

    /**
     * \brief Retrieve all the direct child accounts of this accounts
     * 
     * A Child is a Descendant whose Parent Path is the same as our Full Path
     * 
     * \todo This method is implemented somewhat inefficiently - it
     * retrieves all the descendants, and then filters out descendants that
     * are not direct children
     */
    accounts_vec_t children_sqlite(void) const;

    /**
     * \brief Retrieve all the descendent Accounts of this Account
     * 
     * A descendent is any Account that has a Full Path that begins with the
     * Full Path of this Account.
     */
    accounts_vec_t descendants_sqlite(void) const;

    /**
     * \brief Retrieve the Full Path of the Account
     * 
     * The Full Path of the Account includes the names of all parent and
     * ancestor accounts, and acts as a unique key for Accounts within a
     * Datastore. Note that if the Account is at the root (i.e. the Parent
     * Path is empty), then the Full Path is the same as the Account Name.
     */
    virtual std::wstring full_path(void) const override;

    /**
     * \brief Set the Full Path of the Account
     * 
     * It is important to note that the Full Path is directly recorded in the
     * underlying Sqlite Database, as a unique key for Accounts. It
     * implicitly encodes the parent-child relationship, as well as the Name
     * of the Account.
     * 
     * Changing the Full Path potentially involves changing the parent
     * Account, this method checks that the Parent implied by the Full Path
     * exists, and has a compatible Account Type. If the Account is at the
     * root (i.e. the Parent Path is empty), then the Full Path is the same
     * as the Account Name.
     * 
     * \todo Add checks for circular parent / child / descendent
     * relationships
     */
    virtual void set_full_path(std::wstring p);

    /**
     * \brief Retrieve the Account Description
     */
    virtual std::wstring description(void) const override;

    /**
     * \brief Set the Account Description string
     */
    virtual void set_description(std::wstring d) override;

    /**
     * \brief Retrieve the account type enumerator (asset, liability, income
     * or expense)
     */
    virtual type_t account_type(void) const override;

    /**
     * \brief Set the Account Type for Income and Expense Accounts
     * 
     * This method ensures that the Type being set it compatible with the
     * Parent (if it exists) and any children.
     */
    virtual void set_account_type(type_t t) override;

    /**
     * \brief Set the Account Type for Asset and Liability Accounts, along
     * with the Opening Date and Balance
     * 
     * This method ensures that the Type being set it compatible with the
     * Parent (if it exists) and any children.
     */
    virtual void set_account_type(
        type_t t
        , ncountr::api::date opening_date
        , ncountr::api::currency_t opening_balance) override;

    /**
     * \brief Retrieve the Opening Date of an Asset or Liability Account
     * 
     * This method should not be called for Income or Expense Accounts.
     */
    virtual ncountr::api::date opening_date(void) const override;

    /**
     * \brief Set the Opening Date of an Asset of Liability Accouont
     * 
     * This method should not be called for Income or Expense Accounts.
     */
    virtual void set_opening_date(ncountr::api::date od) override;

    /**
     * \brief Retrieve the Opening Balance of an Asset or Liability Account
     * 
     * This method should not be called for Income or Expense Accounts.
     */
    virtual ncountr::api::currency_t opening_balance(void) const override;

    /**
     * \brief Set the Opening Balance of an Asset or Liability Account
     * 
     * This method should not be called for Income or Expense Accounts.
     */
    virtual void set_opening_balance(ncountr::api::currency_t ob) override;

    /**
     * \brief Convert an Account Type enumerator to a human-readable string
     */
    static QString to_qstring(type_t t);

    /**
     * \brief Convert a human-readable string for an Account Type back to
     * an Account Type enumerator
     */
    static type_t to_account_type(QString str);

    // -- Lower-level Database Services --

    // Higher-level business rules (e.g. that Opening Info is supplied if and
    // only if the Account Type is Asset Liability) are NOT ENFORCED AT THIS
    // LOW LEVEL
    //
    // All business rule checks are done in the methods that call these
    // lower-level methods

    /**
     * \brief Retrieve the highest ID in the `account` table
     * 
     * This method is useful when creating a new record (just add 1 to this
     * to get a new ID), since Sqlite doesn't seem to have an AUTOINCREMENT
     * field.
     */
    int max_id(void) const;

    /**
     * \brief Retrieve the highest ID in the `account` table
     * 
     * This method is useful when creating a new record (just add 1 to this
     * to get a new ID), since Sqlite doesn't seem to have an AUTOINCREMENT
     * field.
     */
    static int max_id(QSqlDatabase& db);

    /**
     * \brief Create a new Income or Expense Account record
     * 
     * Note that this overload should be used for creating `income` and
     * `expense` accounts (because opening date and balance are not given),
     * but this is not enforced. Higher-level methods enforce this business
     * rule.
     */
    static void create_record(
        QSqlDatabase& db
        , int id
        , QString full_path
        , boost::optional<QString> description
        , type_t t);

    /**
     * \brief Create a new Asset or Liability Account record
     * 
     * Note that this overload should be used for creating `asset` and
     * `liability` accounts (because opening date and balance are taken), but
     * this is not enforced. Higher-level methods enforce this business rule.
     */
    static void create_record(
        QSqlDatabase& db
        , int id
        , QString full_path
        , boost::optional<QString> description
        , type_t t
        , QDate od
        , double ob);

    /**
     * \brief Find an Account Record by its ID
     * 
     * \param db A reference to the Database object (must be open and ready
     * to run queries)
     * 
     * \param id The Record ID for which to search
     * 
     * \return The Account Record or `boost::none` if no Record with this ID
     * is found
     */
    static boost::optional<QSqlRecord> find_by_id(
        QSqlDatabase& db
        , int id);

    /**
     * \brief Find an Account Record by its Full Path string
     * 
     * \param db A reference to the Database object (must be open and ready
     * to run queries)
     * 
     * \param full_path The Path of the Account whose record is to be
     * retrieved
     * 
     * \return The Account Record or `boost::none` if no Record with this 
     * Path is found
     */
    static boost::optional<QSqlRecord> find_by_full_path(
        QSqlDatabase& db
        , QString full_path);

    /**
     * \brief Prepare, execute and bind a generic SQL `SELECT` query
     * 
     * \param query A new query object created with the relevant Database;
     * when this method returns, `query` can be used to access results
     * 
     * \param where_clause The criteria on which to search (excluding the
     * `WHERE` keyword)
     * 
     * \param bind_list A name-value map of parameters to bind
     */
    static void select(
        QSqlQuery& query
        , QString where_clause
        , std::map<QString, QVariant> bind_list = {});

    /**
     * \brief Destroy an Account record by ID
     * 
     * \param db The database object to use
     * 
     * \param id The ID of the record to destroy
     */
    static void destroy_record_by_id(QSqlDatabase& db, int id);

    /**
     * \brief Destroy an Account Record by its Full Path
     * 
     * \param db The Database object
     * 
     * \param full_path The Path of the Record
     */
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
    int m_id;

    Q_DECLARE_TR_FUNCTIONS(account)

};  // end account

/**
 * \brief A shared pointer to an Account object
 */
using account_spr = account::account_spr;

/**
 * \brief A vector of (shared pointers to) Account objects
 */
using accounts_vec_t = account::accounts_vec_t;

}}} // end ncountr::datastores::sqlite namespace

#endif
