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
#include <QDate>
#include <QSqlDatabase>
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
 * \todo Expand documentation
 * 
 * \todo Notes on record structure, including non-normalised running balance
 * info
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
     * \brief Retrieve the ID of the underlying Account record
     */
    int id(void) const { return m_id; }

    /**
     * \brief Retrieve the Name of the Account
     * 
     * The Account Name uniquely identifies the Account within all the
     * Accounts under its parent. 
     */
    virtual std::wstring name(void) const override
    {
        return retrieveFieldValue<QString>("name").toStdWString();
    }

    /**
     * \brief Set the Name of the account
     * 
     * This method checks the following Business Rules:
     * 
     * 1. The new Account Name is valid
     * 
     * 2. The new Account Name is unique amongst the children of the Parent
     *    Account (or at the root if the Account has no Parent)
     * 
     * This method should validates the Name using the `valid_name` method.
     */
    virtual void set_name(std::wstring n) override
    {
        if (n == name()) return;    // No change

        if (!valid_name(n))
            throw error(tr("invalid Account name - ")
                + QString::fromStdWString(n));

        // Make sure that the new name will not violate uniqueness in the
        // parent (or at the root, if there is no parent)
        if (find_by_parent_id_and_name(
                m_db, parent_id()
                , QString::fromStdWString(n)) != boost::none)
            throw error(tr("attempt to change Account Name to a Name that "
                "is already taken"));

        updateFieldValue("name", QString::fromStdWString(n));
    }   // end set_name

    /**
     * \brief Split an account path string into an array of account names
     * 
     * This is a version of the base-class method that uses `QString` instead
     * of `std::wstring`.
     */
    static std::vector<QString> split_path(const QString& p);

    /**
     * \brief Concatenate a vector of account names into an account path
     * string
     * 
     * This is a version of the base-class method that uses `QString` instead
     * of `std::wstring`.
     */
    static QString concatenate_path(const std::vector<QString>& p);

    /**
     * \brief Retrieve the ID of the Parent Account record, or `boost::none`
     * if the Account is at the root
     */
    boost::optional<int> parent_id(void) const
    {
        auto pid = retrieveFieldValue<QVariant>("parent_id");
        if (pid.isNull()) return boost::none;
        else return pid.toInt();
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

        std::vector<std::wstring> path_names;
        auto parent_id = retrieveFieldValue<QVariant>("parent_id");
        while (parent_id.isNull() == false)
        {
            auto parent_rec = find_by_id(m_db, parent_id.toInt());
            if (parent_rec == boost::none) parent_id = QVariant();
            else
            {
                path_names.push_back(
                    parent_rec->value("name").toString().toStdWString());
                parent_id = parent_rec->value("parent_id");
            }
        }

        return api::account::concatenate_path(path_names);
    }   // end parent_path method

    /**
     * \brief Set the Parent Account of the Account
     * 
     * This method checks the following Business Rules
     * 
     * 1. IF a non-root Parent is being set THEN the new Parent must have
     *    the same `has_running_balance` as self
     * 
     * 2. There must be no other Account with the same name as self under
     *    the new Parent (or at the root, if moving to the root)
     * 
     * \param parent The new Parent account; this may be `nullptr` to put the
     * Account at the root
     */
    virtual void set_parent(api::account_spr parent) override
    {

        auto sql_parent = std::dynamic_pointer_cast<sqlite::account>(parent);
        boost::optional<int> parent_id = boost::none;
        if (sql_parent)
        {
            // Don't bother if the parent isn't changing
            if (parent->full_path() == parent_path()) return;

            // Ensure that the new parent has the same values for
            // `has_running_balance` as we do.
            if (has_running_balance() != parent->has_running_balance())
                throw error(tr("an attempt was made to add a child with a "
                    "Running Balance to a parent that does not have a "
                    "Running Balance, or vice-versa"));

            parent_id = sql_parent->id();   
        }
        else
        {
            // No parent - if there was no parent before, do nothing
            if (parent_path().empty()) return;
        }

        // Whether we're setting a parent or moving the account to root, we
        // want to make sure that there is not already something with the
        // same name there.
        if (find_by_parent_id_and_name(
                m_db
                , parent_id
                , QString::fromStdWString(name())))
            throw error(
                tr("attempt to add set a Parent Account to an Account "
                    "with a duplicate name, or move an Account to the root "
                    "when another root Account has the same name"));

        if (parent_id == boost::none)
            updateFieldValue("parent_id", QVariant());
        else updateFieldValue("parent_id", *parent_id);

    }   // end set_parent method

    /**
     * \brief Retrieve the Full Path of the Account
     * 
     * The Full Path of the Account acts as a unique key for Accounts within
     * a Datastore. Note that if the Account is at the root (i.e. the Parent
     * Path is empty), then the Full Path is the same as the Account Name.
     */
    virtual std::wstring full_path(void) const override
    {
        auto pp = parent_path();
        if (pp.empty()) return name();
        else return pp + account_path_separator + name();
    }

    /**
     * \brief Retrieve the Account Description
     */
    virtual std::wstring description(void) const override
    {
        return retrieveFieldValue<QString>("description").toStdWString();
    }

    /**
     * \brief Set the Account Description string
     */
    virtual void set_description(std::wstring d) override
    {
        updateFieldValue("description", QString::fromStdWString(d));
    }

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
    virtual bool has_running_balance(void) const override
    {
        return retrieveFieldValue<bool>("has_running_balance");
    }

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
    virtual void set_running_balance_true(
            ncountr::api::date od
            , ncountr::api::currency_t ob) override
    {
///        // Can only do this if there are are no children or parent.
///        if (parent_id() != boost::none)
///            throw error(tr("attempt to add a running balance to an Account "
///                "that is not at the root"));
///
///        
///
///        updateFieldValue("has_running_balance", true);
///        updateFieldValue("opening_date", to_qdate(od).toJulianDay());
///        updateFieldValue("opening_balance", ob);

        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));

    }

    /**
     * \brief Set the Account to have no Running Balance (i.e. to be an
     * Income or Expense Account)
     * 
     * \todo Document business rules
     */
    virtual void set_running_balance_false(void) override
    {
        throw error(
            QString(__FUNCTION__) + tr(" function not implemented yet"));
    }

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
    virtual std::tuple<ncountr::api::date, ncountr::api::currency_t>
    opening_data(void) const override
    {
        if (has_running_balance() == false)
            throw error(tr("attempt to retrieve opening data for an account "
                "that has no running balance - ") +
                    QString::fromStdWString(full_path()));


        return std::make_tuple(
            to_api_date(
                QDate::fromJulianDay(
                    retrieveFieldValue<int>("opening_date")))
            , retrieveFieldValue<double>("opening_balance"));

///        throw error(
///            QString(__FUNCTION__) + tr(" function not implemented yet"));
    }   // end opening_data

///    /**
///     * \brief Convert an Account Type enumerator to a human-readable QString
///     */
///    static QString to_qstring(type_t t);
///
///    /**
///     * \brief Convert a human-readable QString to an Account Type enumerator
///     */
///    static type_t to_account_type(QString str);

    // -- Lower-level Database Services --

    // TODO Note in all documentation that higher-level business rules (e.g.
    // that Opening Info is supplied if and only if the Account Type is Asset
    // Liability) are NOT ENFORCED AT THIS LOW LEVEL
    //
    // All business rule checks are done in the methods that call these
    // lower-level methods

    /**
     * \brief Retrieve the maximum ID of all Account records
     */
    int max_id(void) const;

    /**
     * \brief Retrieve the maximum ID of all Account records in a given
     * database
     */
    static int max_id(QSqlDatabase& db);

    /**
     * \brief Create a Running Balance Account record
     * 
     * This is a low-level method for actually creating the database record.
     * It does not generate an `account` object, but should probably be
     * called by some method that does this next.
     * 
     * Before creating the record, this method checks the following Business
     * Rules.
     * 
     * 1. The Name is a valid Account Name
     * 
     * 2. IF a Parent is NOT specified:
     * 
     *    a. There must be no other Account Record with the same Name
     * 
     * 3. ELSE (i.e. a Parent IS specified)
     * 
     *    a. The Parent must exist
     * 
     *    b. The Parent must also be a Running Balance Account
     * 
     * \param db A reference to the database object
     * 
     * \param id The ID of the record; this should always be one larger than
     * the ID returned by `max_id`
     * 
     * \param name The Name of the new Account; this must conform to the
     * Business Rules
     * 
     * \param parent_id If this is `boost::none`, then the Account is placed
     * at the root; otherwise, this must be the ID of the Parent record
     * 
     * \param description Human-readable description of the Account
     * 
     * \param opening_data The opening date of the Account
     * 
     * \param opening_balance The opening balance of the Account
     */
    static void create_record(
        QSqlDatabase& db
        , int id
        , QString name
        , boost::optional<int> parent_id
        , QString description
        , QDate opening_date
        , double opening_balance);

    /**
     * \brief Create an Incoming / Outgoing Funds (Non-Running Balance)
     * Account record
     * 
     * This is a low-level method for actually creating the database record.
     * It does not generate an `account` object, but should probably be
     * called by some method that does this next.
     * 
     * Before creating the record, this method checks the following Business
     * Rules.
     * 
     * 1. The Name is a valid Account Name
     * 
     * 2. IF a Parent is NOT specified:
     * 
     *    a. There must be no other Account record with the same Name
     * 
     * 3. ELSE (i.e. a Parent IS specified)
     * 
     *    a. The Parent must exist
     * 
     *    b. The Parent must also be a Non-Running Balance Account
     * 
     * \param db A reference to the database object
     * 
     * \param id The ID of the record; this should always be one larger than
     * the ID returned by `max_id`
     * 
     * \param name The Name of the new Account; this must conform to the
     * Business Rules
     * 
     * \param parent_id If this is `boost::none`, then the Account is placed
     * at the root; otherwise, this must be the ID of the Parent record
     * 
     * \param description Human-readable description of the Account
     */
    static void create_record(
        QSqlDatabase& db
        , int id
        , QString name
        , boost::optional<int> parent_id
        , QString description);

    /**
     * \brief Find an Account record by its ID
     */
    static boost::optional<QSqlRecord> find_by_id(
        QSqlDatabase& db
        , int id);

    /**
     * \brief Find an Account record by its full path
     */
    static boost::optional<QSqlRecord> find_by_full_path(
        QSqlDatabase& db
        , QString full_path);

    /**
     * \brief Retrieve an Account record given its parent ID (which may be
     * `boost::none`) and its Name
     */
    static boost::optional<QSqlRecord> find_by_parent_id_and_name(
        QSqlDatabase& db
        ,   boost::optional<int> parent_id
        , QString name);

    static void select(
        QSqlQuery& query
        , const QString& whereClause
        , const std::map<QString, QVariant> bindings = {});

    /**
     * \brief Retrieve the value of a field in the record
     * 
     * \tparam T The type of the value
     * 
     * \param fieldName The name of the file in the `account` table
     * 
     * \return The field value
     */
    template <typename T>
    T retrieveFieldValue(QString fieldName) const
    {
        return retrieveSingleRecordFieldValue<T>(
            m_db
            , "account"
            , fieldName
            , QString::fromStdString("id = {}"_format(m_id)));
    }   // end retrieveFieldValue method

    template <typename T>
    void updateFieldValue(QString fieldName, T value)
    {
        updateSingleRecordFieldValue(
            m_db
            , "account"
            , fieldName
            , value
            , QString::fromStdString("id = {}"_format(m_id)));
    }

    /**
     * \brief Destroy an Account record by its ID
     */
    static void destroy_record_by_id(QSqlDatabase& db, int id);

    /**
     * \brief Destroy an Account record by its Full Path
     */
    static void destroy_record_by_full_path(
        QSqlDatabase& db, QString full_path);

    // --- Internal Declarations ---

    protected:

    // -- Attributes --

    QSqlDatabase& m_db; ///< The database for our data

    /**
     * \brief The Identifier for the Account Record
     */
    int m_id;

    Q_DECLARE_TR_FUNCTIONS(account)

};  // end account

/**
 * \brief A shared pointer to an Account object
 */
using account_spr = account::account_spr;

}}} // end ncountr::datastores::sqlite namespace

#endif
