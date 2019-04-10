/**
 * \file sqlite_datastore/datastore.h
 * Declare the `sqlite::datastore` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <boost/optional.hpp>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

#include <fmt/format.h>
using namespace fmt::literals;

#include "../api/api.h"

#ifndef _sqlite_datastore_h_included
#define _sqlite_datastore_h_included

namespace ncountr { namespace datastores { namespace sqlite {

/**
 * \brief Convenient alias for the logger level enumerator type
 * 
 * \todo Move this to a single logging header for the library
 */
using level_t = qlib::logger::level_t;

/**
 * \brief Convenience method for retrieving a reference to the single logger
 * instance
 * 
 * \return A reference to the logging singleton
 * 
 * \todo Move this to a single logging header for the library
 */
inline qlib::logger& logger(void) { return qlib::logger::instance(); }

/**
 * \brief SQLite implementation of the `api::datastore` interface
 * 
 * This Datastore implementation uses a Sqlite database to record its data.
 * Note that one (very minor) constraint on this implementation is that two
 * Sqlite Datastores cannot access the same file at the same time within the
 * same process, because the underlying implementation uses the file path as
 * a unique ID for the database connection). We could probably get around
 * this constraint with some clever ID work, but there doesn't seem to be
 * much point in going to the effort, because -- in this application -- we're
 * only opening one datastore at a time in any case.
 */
class datastore : public api::datastore
{

    // --- External Definitions ---

    public:

    using base_t = api::datastore;  ///< Our base class

    /**
     * \brief Error exception for signalling errors related to the Sqlite
     * Datastore
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
     * \brief Constructor - attempts to open the database
     * 
     * Note that if the database is being created 'from scratch', then the
     * `initialise` method needs to be called as well.
     * 
     * If opening the database is succcessful, `is_ready` will return `true`.
     * 
     * \param filePath The path of the Sqlite file
     */
    explicit datastore(QString filePath);

    /**
     * \brief Destructor - shuts down the database connection
     */
    virtual ~datastore(void);

    // Default virtual destructor, and disable copy / move semantics
    DECLARE_NO_MOVE_AND_COPY_SEMANTICS(datastore)

    /**
     * \brief Determine whether or not the storage is ready for use
     * 
     * For a Sqlite Datastore, this means checking to see whether the
     * database object is present.
     */
    virtual bool is_ready(void) const
    {
        return (m_db != boost::none);
    }   // end is_ready

    /**
     * \brief Initialise the database
     * 
     * This method must be called for new Sqlite Datastore Documents, after
     * they are opened.
     * 
     * \todo Consider running this automatically in the constructor,
     * automatically checking for previous initialisation
     */
    void initialise(void);

    // -- Document Information --

    /**
     * \brief Retrieve the Document Name
     */
    virtual std::wstring name(void) const;

    /**
     *  \brief Set the Document Name
     */
    virtual void set_name(std::wstring n);

    /**
     * \brief Retrieve the Document Description
     */
    virtual std::wstring description(void) const;

    /**
     * \brief Set the Document Description string
     */
    virtual void set_description(std::wstring d);

    /**
     * \brief Retrieve the version number for the underlying file format
     * 
     * Note that this information is not available at the generic
     * `api::datastore` level. All format version issues must be resolved
     * at the Sqlite Datastore level.
     */
    int file_format_version(void) const;

    // -- Inernal Helper Functions --
    
    /**
     * \brief Initialise the given database for nCountr data
     * 
     * This method assumes that `db` is completely new and empty. It runs
     * all the necessary table creation and data initialisation queries
     * necessary to use the database for nCountr data.
     * 
     * \param db The database object
     */
    static void initialise(QSqlDatabase& db);

    /**
     * \brief Retrieve a single field value from a single record in a table
     * 
     * This is a convenience method for accessing a single field value. It
     * prepares and executes a whole query, so should not be used when
     * accessing lots of values at once.
     * 
     * \tparam T The type of the value; this should be convertible from the
     * `QVariant` object that is retrieved from the database
     * 
     * \param tableName The name of the table
     * 
     * \param fieldName The name of the field in the table
     * 
     * \param whereClause The search condition by which the record is to be
     * located; this should be something that will return exactly one record
     */
    template <typename T>
    T retrieveSingleRecordFieldValue(
            const QString& tableName
            , const QString& fieldName
            , const QString& whereClause) const
    {
        if (!is_ready())
            throw error(tr("attempted to retrieve a field value from a "
                "Datastore that is not open"));

        QString queryString =
            "SELECT " + fieldName
            + " FROM " + tableName
            + " WHERE " + whereClause;

        logger().log(
            level_t::debug
            , L"query: {}"_format(queryString.toStdWString()));

        QSqlQuery query(*m_db);
        if (!query.prepare(queryString))
            throw error(tr("query preparation error: ") +
                query.lastError().text());

        if (!query.exec())
            throw error(tr("query execution error: ") +
                query.lastError().text());

        // We should have at least one record
        if (!query.next())
            throw error(tr("could not locate record"));

        int idx = query.record().indexOf(fieldName);

        return query.value(idx).value<T>();
    }   // end retrieveSingleRecordFieldValue template method

    /**
     * \brief Update a single field value in a single record
     * 
     * This is a convenience method for doing a quick value updated. It
     * should not be called repeatedly to update several values at once, as
     * it is not very efficient (constructs and executes a whole query).
     * 
     * \tparam T The type of the value to set; this should be a type that is
     * convertible to `QVariant`
     * 
     * \param tableName The name of the table in the database
     * 
     * \param fieldName The name of the field to update
     * 
     * \param value The value to set
     * 
     * \param whereClause Condition identifying the record to update; this
     * should be something like "id = <n>", identifying just one record
     */
    template <typename T>
    void updateSingleRecordFieldValue(
            const QString& tableName
            , const QString& fieldName
            , const T& value
            , const QString& whereClause)
    {
        if (!is_ready())
            throw error(tr("attempted to set a field value in a Datastore "
                "that is not open"));

        QString queryString = "UPDATE " + tableName
                                + " SET " + fieldName + " = :v"
                                " WHERE " + whereClause;

        logger().log(
            level_t::debug
            , L"query: {}"_format(queryString.toStdWString()));
        
        QSqlQuery query(*m_db);
        if (!query.prepare(queryString))
            throw error(tr("query preparation error: ") +
                query.lastError().text());

        query.bindValue(":v", QVariant(value), QSql::In);

        if (!query.exec())
            throw error(tr("query execution error: ") +
                query.lastError().text());        
    }   // end updateSingleRecordFieldValue template method

    // --- Internal Attributes ---

    private:

    QString m_filePath;                 ///< Path of the database file

    mutable boost::optional<QSqlDatabase> m_db; ///< Database connection

    Q_DECLARE_TR_FUNCTIONS(Document)

};  // end datastore class

}}}  // end ncountr::datastores::sqlite namespace

#endif
