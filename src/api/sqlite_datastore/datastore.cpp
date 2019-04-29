/**
 * \file datastore.cpp
 * Implement the `sqlite::datastore` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <qlib/qlib.h>

#include "account.h"
#include "datastore.h"

namespace ncountr { namespace datastores { namespace sqlite {

datastore::datastore(QString filePath) :
    base_t()
    , m_filePath(std::move(filePath))
    , m_db(boost::none)
{
    // Use the file path as a connection name, which we can remove when the
    // object goes down
    m_db = QSqlDatabase::addDatabase("QSQLITE", m_filePath);
    m_db->setDatabaseName(m_filePath);

    if (m_db->open())
    {
        logger().log(level_t::debug, L"database {} created / opened"_format(
            m_filePath.toStdWString()));
    }
    else
    {
        m_db = boost::none;
        logger().log(level_t::error, L"could not open database {}"_format(
            m_filePath.toStdWString()));
    }
}   // end constructor

datastore::~datastore(void)
{
    m_db = boost::none;
    QSqlDatabase::removeDatabase(m_filePath);

    logger().log(level_t::debug, L"closed database and connection");
}   // end destructor

void datastore::initialise(void)
{
    if (m_db == boost::none)
        throw error(tr("attempted to initialise a Datastore that has not "
            "been opened successfully"));

    initialise(*m_db);
}   // end initialise method

std::wstring datastore::name(void) const
{
    return retrieveSingleRecordFieldValue<QString>(
        "document_info"
        , "name"
        , "id = 1").toStdWString();
}   // end name method

void datastore::set_name(std::wstring n)
{
    updateSingleRecordFieldValue(
        "document_info"
        , "name"
        , QString::fromStdWString(n)
        , "id = 1");
}   // end set_name method

std::wstring datastore::description(void) const
{
    return retrieveSingleRecordFieldValue<QString>(
        "document_info"
        , "description"
        , "id = 1").toStdWString();
}   // end description method

void datastore::set_description(std::wstring d)
{    
    updateSingleRecordFieldValue(
        "document_info"
        , "description"
        , QString::fromStdWString(d)
        , "id = 1");
}   // end set_description method

api::accounts_vec_t datastore::root_accounts(void)
{

    if (!is_ready())
        throw error(tr("attempt to retrieve root Accounts from a Datastore "
            "that is not open / ready"));

    QSqlQuery query(*m_db);
    account::select(
        query
        , QString("full_path NOT LIKE '%")
            + QString::fromStdWString(account::account_path_separator)
            + QString("%'"));

    api::accounts_vec_t results;
    while (query.next())
        results.push_back(
            std::make_shared<account>(
                *m_db
                , query.value("id").value<int>()));

    return results;
}   // end root_accounts

api::account_spr datastore::find_by_full_path(
        std::wstring full_path)
{
    if (!is_ready())
        throw error(tr("attempt to find an Account in a Datastore that is "
            "not open / ready"));

    return account::find_existing(*m_db, QString::fromStdWString(full_path));
}   // end find_by_root_path method

int datastore::file_format_version(void) const
{
    return retrieveSingleRecordFieldValue<int>(
        "document_info"
        , "file_format_version"
        , "id = 1");
}   // end file_format_version

void datastore::initialise(QSqlDatabase& db)
{
    // First, lay out the document info table (which we handle directly)
    // Note: assuming database is already totally empty
    QString queryString = "CREATE TABLE document_info ("
            "id INTEGER PRIMARY KEY"
            ", name TEXT"
            ", description TEXT"
            ", file_format_version INTEGER"
        ");";

    logger().log(
        level_t::debug
        , L"query: {}"_format(queryString.toStdWString()));

    QSqlQuery query(db);
    if (!query.prepare(queryString))
        throw error(tr("query preparation error: ") +
            query.lastError().text());

    if (!query.exec())
        throw error(tr("query execution error: ") +
            query.lastError().text());

    queryString = "INSERT INTO document_info ("
                        "id"
                        ", name"
                        ", description"
                        ", file_format_version"
                    ") VALUES ("
                        "1"
                        ", '<Document Name>'"
                        ", '<Document Description>'"
                        ", 1);";

    logger().log(
        level_t::debug
        , L"query: {}"_format(queryString.toStdWString()));
    
    if (!query.prepare(queryString))
        throw error(tr("query preparation error: ") +
            query.lastError().text());

    if (!query.exec())
        throw error(tr("query execution error: ") +
            query.lastError().text());

    logger().log(level_t::debug, L"new datastore initialised");

    // Now lay out the other tables - handled by other classes
    account::initialise(db);
}   // end initialise method

}}}  // end ncountr::datastores::sqlite namespace
