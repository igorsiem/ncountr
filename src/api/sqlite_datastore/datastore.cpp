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
#include "db_utils.h"

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

api::account_spr datastore::create_account(
        std::wstring name
        , api::account_spr parent
        , std::wstring description
        , api::date opening_date
        , api::currency_t opening_balance)
{

    if (name.empty())
        throw error(tr("attempt to create Account with empty Name"));

    auto id = account::max_id(*m_db) + 1;

    boost::optional<int> parent_id = boost::none;
    if (parent)
        parent_id = std::dynamic_pointer_cast<sqlite::account>(parent)->id();

    account::create_record(
        *m_db
        , id
        , QString::fromStdWString(name)
        , parent_id
        , QString::fromStdWString(description)
        , to_qdate(opening_date)
        , opening_balance);
    
    return std::make_shared<sqlite::account>(*m_db, id);
    
}   // end create_account method

api::account_spr datastore::create_account(
        std::wstring name
        , api::account_spr parent
        , std::wstring description)
{
    if (name.empty())
        throw error(tr("attempt to create Account with empty Name"));

    auto id = account::max_id(*m_db) + 1;

    boost::optional<int> parent_id = boost::none;
    if (parent)
        parent_id = std::dynamic_pointer_cast<sqlite::account>(parent)->id();

    account::create_record(
        *m_db
        , id
        , QString::fromStdWString(name)
        , parent_id
        , QString::fromStdWString(description));
    
    return std::make_shared<sqlite::account>(*m_db, id);

}   // end create_account method

api::account_spr datastore::find_account(std::wstring full_path)
{
    // Find the account record, then wrap it in an account object
    auto rec =
        account::find_by_full_path(
            *m_db
            , QString::fromStdWString(full_path));

    if (rec == boost::none) return nullptr;
    else
        return std::make_shared<sqlite::account>(
            *m_db
            , rec->value("id").toInt());
}   // end find_account method

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

    QSqlQuery query(db);
    prepareAndExecute(query, queryString);

    queryString = "INSERT INTO document_info ("
                        "id"
                        ", name"
                        ", description"
                        ", file_format_version"
                    ") VALUES ("
                        "1"
                        ", :name"
                        ", :description"
                        ", 1);";

    prepareAndExecute(
        query
        , queryString
        , {
            {":name", tr("<Document Name>")}
            , { ":description", tr("<Document Description>")}});

    logger().log(level_t::debug, L"new datastore initialised");

    // Now lay out the other tables - handled by other classes
    account::initialise(db);
}   // end initialise method

}}}  // end ncountr::datastores::sqlite namespace
