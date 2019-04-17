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
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>

#include "../api.h"
#include "logging.h"

#ifndef _sqlite_db_utils_h_included
#define _sqlite_db_utils_h_included

namespace ncountr { namespace datastores { namespace sqlite {

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

    logger().log(
        level_t::debug
        , L"query: {}"_format(queryString.toStdWString()));

    QSqlQuery query(db);
    if (!query.prepare(queryString))
        throw DbUtilsError(
            DB_UTILS_TR("query preparation error: ")
            + query.lastError().text());

    if (!query.exec())
        throw DbUtilsError(
            DB_UTILS_TR("query execution error: ")
            + query.lastError().text());

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

    logger().log(
        level_t::debug
        , L"query: {}"_format(queryString.toStdWString()));
    
    QSqlQuery query(db);
    if (!query.prepare(queryString))
        throw DbUtilsError(DB_UTILS_TR("query preparation error: ") +
            query.lastError().text());
 
     query.bindValue(":v", QVariant(value), QSql::In);
 
     if (!query.exec())
         throw DbUtilsError(DB_UTILS_TR("query execution error: ") +
             query.lastError().text());

}   // end updateSingleRecordFieldValue template function

}}} // end ncountr::datastores::sqlite namespace

#endif
