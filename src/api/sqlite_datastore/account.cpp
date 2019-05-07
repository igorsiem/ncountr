/**
 * \file sqlite_datastore/account.cpp
 * Implement the `sqlite::account` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "account.h"
#include "logging.h"
#include "db_utils.h"

namespace ncountr { namespace datastores { namespace sqlite {

account::account(
        QSqlDatabase& db
        , int id) :
    api::account::account()
    , m_db(db)
    , m_id(id)
{

}   // end constructor

account_spr account::make_new(
        QSqlDatabase& db
        , QString name
        , type_t t
        , account_spr parent
        , boost::optional<QString> description)
{
    throw error(QString(__FUNCTION__) + tr(" function not implemented yet"));
}   // end make_shared

account_spr account::make_new(
        QSqlDatabase& db
        , QString name
        , type_t t
        , account_spr parent
        , boost::optional<QString> description
        , ncountr::api::date od
        , ncountr::api::currency_t ob)
{
    throw error(QString(__FUNCTION__) + tr(" function not implemented yet"));
}

account_spr account::find_existing(QString path)
{
    throw error(QString(__FUNCTION__) + tr(" function not implemented yet"));
}

account::~account(void)
{
}   // end destructor

void account::initialise(QSqlDatabase& db)
{
    QString queryString = "CREATE TABLE account ("
                                "id INTEGER PRIMARY KEY"
                                ", full_path TEXT UNIQUE NOT NULL"
                                ", description TEXT"
                                ", account_type TEXT NOT NULL"
                                ", opening_date INTEGER DEFAULT NULL"
                                ", opening_balance REAL DEFAULT NULL"
                            ");";

///    logger().log(
///        level_t::debug
///        , L"query: {}"_format(queryString.toStdWString()));

    QSqlQuery query(db);
///    if (!query.prepare(queryString))
///        throw error(tr("query preparation error: ") +
///            query.lastError().text());
///
///    if (!query.exec())
///        throw error(tr("query execution error: ") +
///            query.lastError().text());

    prepareAndExecute(query, queryString);

    // TODO add indices

}   // end initialise method

QString account::to_qstring(type_t t)
{
    switch (t)
    {
        case type_t::asset: return tr("asset");
        case type_t::liability: return tr("liability");
        case type_t::income: return tr("income");
        case type_t::expense: return tr("expense");
        default: throw error(tr("unrecognised Account Type enumerator"));
    }
}   // end to_qstring method

api::account::type_t account::to_account_type(QString str)
{
    if (str == tr("asset")) return type_t::asset;
    else if (str == tr("liability")) return type_t::liability;
    else if (str == tr("income")) return type_t::income;
    else if (str == tr("expense")) return type_t::expense;
    else throw error(tr("unrecognised Account Type - ") + str);
}   // end to_account_type method

int account::max_id(void) const
{
    return max_id(m_db);
}   // end max_id

int account::max_id(QSqlDatabase& db)
{
    if (!db.isOpen())
        throw error(tr("attempt to retrieve max Account Record ID from "
            "Database that is not open"));

    QString queryString = "SELECT MAX(id) AS max_id FROM account";

///    logger().log(
///        level_t::debug
///        , L"query: {}"_format(queryString.toStdWString()));

    QSqlQuery query(db);
///    if (!query.prepare(queryString))
///        throw error(
///            tr("query preparation error: ")
///            + query.lastError().text());
///
///    if (!query.exec())
///        throw error(
///            tr("query execution error: ")
///            + query.lastError().text());

    prepareAndExecute(query, queryString);

    // We should have at least one record
    if (!query.next())
        throw error(tr("could not access retrieve record"));

    int idx = query.record().indexOf("max_id");

    if (query.isNull(idx)) return 0;
    else return query.value(idx).value<int>();
}   // end max_id

void account::create_record(
        QSqlDatabase& db
        , int id
        , QString full_path
        , boost::optional<QString> description
        , type_t t)
{
    if (!db.isOpen())
        throw error(tr("attempt to create a new Account record for a "
            "Database that is not open"));

    // Set up the query
    QString queryString =
        "INSERT INTO account ("
            "id"
            ", full_path"
            ", description"
            ", account_type"
        ") VALUES ("
            ":id"
            ", :full_path"
            ", :description"
            ", :type"
        ");";

    QSqlQuery query(db);

    QVariant desc(QVariant::String);
    if (description != boost::none)
        desc = *description;

    prepareAndExecute(
        query
        , queryString
        , {
            { ":id", id }
            , { ":full_path" , full_path }
            , { ":description", desc}
            , { ":type", to_qstring(t) } });

}   // end create_record method

void account::create_record(
        QSqlDatabase& db
        , int id
        , QString full_path
        , boost::optional<QString> description
        , type_t t
        , ncountr::api::date od
        , ncountr::api::currency_t ob)
{
    if (!db.isOpen())
        throw error(tr("attempt to create a new Account record for a "
            "Database that is not open"));

    throw error(QString(__FUNCTION__) + tr(" function not implemented yet"));
}

boost::optional<QSqlRecord> account::find_by_id(
        QSqlDatabase& db
        , int id)
{
    
    if (!db.isOpen())
        throw error(tr("attempt to find an Account record in a Database "
            "that is not open"));

    QString queryString = "SELECT * FROM account WHERE id = :id";

    QSqlQuery query(db);
    prepareAndExecute(query, queryString, {{":id", id}});

    if (query.next()) return query.record();
    else return boost::none;

}   // end find_by_id method

boost::optional<QSqlRecord> account::find_by_full_path(
        QSqlDatabase& db
        , QString full_path)
{
    if (!db.isOpen())
        throw error(tr("attempt to find an Account record in a Database "
            "that is not open"));

    QString queryString =
        "SELECT * FROM account WHERE full_path = :full_path";


    QSqlQuery query(db);
    prepareAndExecute(query, queryString, {{":full_path", full_path}});

    if (query.next()) return query.record();
    else return boost::none;
}

void account::destroy_record_by_id(QSqlDatabase& db, int id)
{
    if (!db.isOpen())
        throw error(tr("attempt to destroy an Account record in a Database "
            "that is not open"));

    QString queryString = "DELETE FROM account WHERE id = :id";

    QSqlQuery query(db);
    prepareAndExecute(query, queryString, {{":id", id}});

}   // end destroy_record_by_id

void account::destroy_record_by_full_path(
        QSqlDatabase& db, QString full_path)
{
    if (!db.isOpen())
        throw error(tr("attempt to destroy an Account record in a Database "
            "that is not open"));

    QString queryString = "DELETE FROM account WHERE full_path = :full_path";

    QSqlQuery query(db);
    prepareAndExecute(query, queryString, {{":full_path", full_path}});

}   // end destroy_record_by_full_path method

}}} // end ncountr::datastores::sqlite
