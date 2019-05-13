/**
 * \file db_utils.h
 * Low-level database utilities for the Sqlite Datastore
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <QCoreApplication>
#include <QDate>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QVariant>

#include <fmt/format.h>
using namespace fmt::literals;

#include "../api.h"
#include "logging.h"

#ifndef _sqlite_db_utils_h_included
#define _sqlite_db_utils_h_included

namespace ncountr { namespace datastores { namespace sqlite {

/**
 * \brief An exception class for signalling an error condition in the
 * database utilities functionality
 */
class DbUtilsError : public api::error
{
    public:

    /**
     * \brief Constructor, setting the exception object with a QString
     * 
     * \param msg The human-readable error message
     */
    explicit DbUtilsError(QString msg) :
        api::error::error(msg.toStdString()) {}

    /**
     * \brief Constructor, setting the exception object with a
     * std::string
     * 
     * \param msg The human-readable error message
     */
    explicit DbUtilsError(std::string msg) : api::error::error(msg) {}

    DECLARE_DEFAULT_VIRTUAL_DESTRUCTOR(DbUtilsError)
    DECLARE_DEFAULT_MOVE_AND_COPY_SEMANTICS(DbUtilsError)

};  // end DbUtilsError class

/**
 * Call the text-translation function for the Database Utililities library
 */
#define DB_UTILS_TR( msg ) QCoreApplication::translate("dbutils", msg)

/**
 * \brief Generic query execution with bind values
 * 
 * \param query The query object to use, which should already be associated
 * with a database
 * 
 * \param queryString The full query string
 * 
 * \param bindings A map of bind symbol names and their values; these must
 * correspond to the bindings specified in `queryString`
 */
void prepareAndExecute(
        QSqlQuery& query
        , const QString& queryString
        , const std::map<QString, QVariant>& bindings = {});

/**
 * \brief Retrieve a single field value from a single record in a database
 * table
 * 
 * This is a convenience method for accessing a single field value. It
 * prepares and executes a whole query, so should not be used when
 * accessing lots of values at once.
 * 
 * \tparam T The type of the value; this should be convertible from the
 * `QVariant` object that is retrieved from the database
 * 
 * \param db A reference to the database object
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
    QSqlDatabase& db
    , const QString& tableName
    , const QString& fieldName
    , const QString& whereClause)
{
    if (!db.isOpen())
        throw DbUtilsError(DB_UTILS_TR("attempted to retrieve a field value "
            "from a Database that is not open"));

    QString queryString =
        "SELECT " + fieldName
        + " FROM " + tableName
        + " WHERE " + whereClause;

    QSqlQuery query(db);
    prepareAndExecute(query, queryString);

    // We should have at least one record
    if (!query.next())
        throw DbUtilsError(DB_UTILS_TR("could not access retrieved record"));

    int idx = query.record().indexOf(fieldName);

    return query.value(idx).value<T>();
}   // end retrieveSingleRecordFieldValue template function

/**
 * \brief Update a single field value in a single record in a single table
 * 
 * This is a convenience method for doing a quick value update. It should not
 * be called repeatedly to update several values at once, as it is not very
 * efficient (constructs and executes a whole query).
 * 
 * Note: *Business Rules are not checked by this method, so it is possible to
 * use this method to make records that break the rules. Use this method with
 * care!*
 * 
 * \tparam T The type of the value to set; this should be a type that is
 * convertible to `QVariant`
 * 
 * \param db The database object on which to perform the operation
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
        QSqlDatabase& db
        , const QString& tableName
        , const QString& fieldName
        , const T& value
        , const QString& whereClause)
{
    if (!db.isOpen())
        throw DbUtilsError(DB_UTILS_TR("attempted to set a field value in a "
            "Database that is not open"));

    QString queryString = "UPDATE " + tableName
                            + " SET " + fieldName + " = :v"
                            " WHERE " + whereClause;

    QSqlQuery query(db);
    prepareAndExecute(query, queryString, {{":v", value}});

}   // end updateSingleRecordFieldValue template function

/**
 * \brief Convenuence function for converting an API date object (from Boost)
 * to a QDate
 */
QDate to_qdate(ncountr::api::date d);

/**
 * \brief Convenience function for converting a QDate to an API date (from
 * Boost)
 */
ncountr::api::date to_api_date(QDate d);

}}} // end ncountr::datastores::sqlite namespace

#endif
