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

account::~account(void)
{
}   // end destructor

void account::initialise(QSqlDatabase& db)
{

    QString queryString = "CREATE TABLE account ("
                                "id INTEGER PRIMARY KEY "
                                ", name TEXT"
                                ", parent_id INTEGER DEFAULT NULL"
                                ", description TEXT"
                                ", has_running_balance INTEGER DEFAULT 0"
                                ", opening_date INTEGER DEFAULT NULL"
                                ", opening_balance REAL DEFAULT NULL"
                                ", FOREIGN KEY (parent_id)"
                                    " REFERENCES account(id)"
                                        " ON DELETE CASCADE"
                            ");";

    // Opening Date is specified as Julian Day integer

    QSqlQuery query(db);
    prepareAndExecute(query, queryString);

    queryString = "CREATE UNIQUE INDEX idx_account_parent_id_name "
                    "ON account(parent_id, name)";
    prepareAndExecute(query, queryString);

}   // end initialise method

std::vector<QString> account::split_path(const QString& p)
{
    std::vector<QString> result;
    auto std_result = base_t::split_path(p.toStdWString());
    for (auto s : std_result) result.push_back(QString::fromStdWString(s));

    return result;
}   // end  method

QString account::concatenate_path(const std::vector<QString>& p)
{
    std::vector<std::wstring> std_p;
    for (auto n : p) std_p.push_back(n.toStdWString());

    return QString::fromStdWString(base_t::concatenate_path(std_p));
}   // end  method

///QString account::to_qstring(type_t t)
///{
///    switch (t)
///    {
///        case type_t::asset: return tr("asset");
///        case type_t::liability: return tr("liability");
///        case type_t::income: return tr("income");
///        case type_t::expense: return tr("expense");
///        default: throw error(tr("unrecognised Account Type enumerator"));
///    }
///}   // end to_qstring method
///
///api::account::type_t account::to_account_type(QString str)
///{
///    if (str == tr("asset")) return type_t::asset;
///    else if (str == tr("liability")) return type_t::liability;
///    else if (str == tr("income")) return type_t::income;
///    else if (str == tr("expense")) return type_t::expense;
///    else throw error(tr("unrecognised Account Type - ") + str);
///}   // end to_account_type method

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

    QSqlQuery query(db);
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
        , QString name
        , boost::optional<int> parent_id
        , QString description
        , QDate opening_date
        , double opening_balance)
{

    if (!db.isOpen())
        throw error(tr("attempt to create a new Account record in a "
            "Database that is not open"));

    // Make sure the name is valid.
    if (!valid_name(name.toStdWString()))
        throw error(tr("invalid account name - ") + name);

    QSqlQuery query(db);
    QString queryString;

    // Need to do some manual business-rule checks here, partly to fill in
    // some of Sqlite's deficiencies
    if (parent_id == boost::none)
    {
        // If the parent is not specified, then we're attempting to create
        // the new account at the root. Name must be unique amongst root
        // account records.
        queryString = "SELECT COUNT(id) AS id_count"
                        " FROM account"
                        " WHERE name = :name AND parent_id IS NULL";

        prepareAndExecute(query, queryString, {{":name", name}});

        query.next();

        auto count = query.value("id_count").toInt();

        if (count != 0)
            throw error(tr("attempt to create new Account with duplicate "
                "name (at root) - ") + name);
    }
    else
    {
        // The parent is not NULL - need to make sure it exists
        queryString = "SELECT COUNT(id) AS id_count"
                        " FROM account"
                        " WHERE id = :id";

        prepareAndExecute(query, queryString, {{":id", *parent_id}});

        query.next();

        auto count = query.value("id_count").toInt();

        if (count == 0)
            throw error(tr("attempt to create new Account with invalid "
                "parent ID"));

        // Parent exists, but we also need to make sure it IS a running
        // balance account (because we are creating a non-running-balance
        // child)
        if (!retrieveSingleRecordFieldValue<bool>(
                db
                , "account"
                , "has_running_balance"
                , QString::fromStdString("id = {}"_format(*parent_id))))
            throw error(tr("attempt to create an Account with a running "
                "balance as a child of an Account that does not have a "
                "Running Balance"));
    }

    queryString =
            "INSERT INTO account ("
                "id"
                ", name"
                ", parent_id"
                ", description"
                ", has_running_balance"
                ", opening_date"
                ", opening_balance"
            ") VALUES ("
                ":id"
                ", :name"
                ", :parent_id"
                ", :description"
                ", 1"
                ", :opening_date"
                ", :opening_balance"
            ");";

    QVariant parentId;
    if (parent_id != boost::none) parentId = *parent_id;

    // Note that Opening Date is converted to a Julian Day integer

    prepareAndExecute(
        query
        , queryString
        , {
            {":id", id}
            , {":name", name}
            , {":parent_id", parentId}
            , {":description", description}
            , {":opening_date", opening_date.toJulianDay() }
            , {":opening_balance", opening_balance }});

}   // end create_record method

void account::create_record(
        QSqlDatabase& db
        , int id
        , QString name
        , boost::optional<int> parent_id
        , QString description)
{

    if (!db.isOpen())
        throw error(tr("attempt to create a new Account record in a "
            "Database that is not open"));

    // Make sure the name is valid.
    if (!valid_name(name.toStdWString()))
        throw error(tr("invalid account name - ") + name);

    QSqlQuery query(db);
    QString queryString;

    // Need to do some manual business-rule checks here, partly to fill in
    // some of Sqlite's deficiencies
    if (parent_id == boost::none)
    {
        // If the parent is not specified, then we're attempting to create
        // the new account at the root. Name must be unique amongst root
        // account records.
        queryString = "SELECT COUNT(id) AS id_count"
                        " FROM account"
                        " WHERE name = :name AND parent_id IS NULL";

        prepareAndExecute(query, queryString, {{":name", name}});

        query.next();

        auto count = query.value("id_count").toInt();

        if (count != 0)
            throw error(tr("attempt to create new Account with duplicate "
                "name (at root) - ") + name);

    }
    else
    {
        // The parent is not NULL - need to make sure it exists
        queryString = "SELECT COUNT(id) AS id_count"
                        " FROM account"
                        " WHERE id = :id";

        prepareAndExecute(query, queryString, {{":id", *parent_id}});

        query.next();

        auto count = query.value("id_count").toInt();

        if (count == 0)
            throw error(tr("attempt to create new Account with invalid "
                "parent ID"));

        // Parent exists, but we also need to make sure it is not a running
        // balance account (because we are creating a non-running-balance
        // child)
        if (retrieveSingleRecordFieldValue<bool>(
                db
                , "account"
                , "has_running_balance"
                , QString::fromStdString("id = {}"_format(*parent_id))))
            throw error(tr("attempt to create an Account with no running "
                "balance as a child of an Account that does have a running "
                "balance"));
    }

    queryString =
            "INSERT INTO account ("
                "id"
                ", name"
                ", parent_id"
                ", description"
            ") VALUES ("
                ":id"
                ", :name"
                ", :parent_id"
                ", :description"
            ");";

    QVariant parentId;
    if (parent_id != boost::none) parentId = *parent_id;

    prepareAndExecute(
        query
        , queryString
        , {
            {":id", id}
            , {":name", name}
            , {":parent_id", parentId}
            , {":description", description}});

}   // end create_record

///void account::create_record(
///        QSqlDatabase& db
///        , int id
///        , QString full_path
///        , boost::optional<QString> description
///        , type_t t
///        , ncountr::api::date od
///        , ncountr::api::currency_t ob)
///{
//////    if (!db.isOpen())
//////        throw error(tr("attempt to create a new Account record for a "
//////            "Database that is not open"));
///
///    throw error(QString(__FUNCTION__) + tr(" function not implemented yet"));
///}

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
    
    // Break down the full path and work our way down, finding parent and
    // child records until we reach the end.
    auto path_names = split_path(full_path);

    boost::optional<QSqlRecord> rec = boost::none;
    for (auto n : path_names)
    {
        if (rec == boost::none)
        {
            rec = find_by_parent_id_and_name(db, boost::none, n);
            if (rec == boost::none) return boost::none;
        }
        else
            rec = find_by_parent_id_and_name(
                db, rec->value("id").toInt(), n);
    }

    return rec;

}   // end find_by_full_path method

boost::optional<QSqlRecord> account::find_by_parent_id_and_name(
        QSqlDatabase& db
        , boost::optional<int> parent_id
        , QString name)
{
    if (!db.isOpen())
        throw error(tr("attempt to find an Account record in a Database "
            "that is not open"));

    // Different query if we don't have a parent
    QString queryString;
    std::map<QString, QVariant> bindList;
    if (parent_id == boost::none)
    {
        queryString = "SELECT * FROM account "
            "WHERE name = :name AND parent_id IS NULL";
        bindList = {{ ":name", name }};
    }
    else
    {
        queryString = "SELECT * FROM account "
            "WHERE name = :name AND parent_id = :parent_id";
        bindList = {{ ":name", name }, {":parent_id", *parent_id }};
    }

    QSqlQuery query(db);
    prepareAndExecute(query, queryString, bindList);

    if (query.next()) return query.record();
    else return boost::none;

}   // end find_by_parent_id_and_name method

void account::select(
        QSqlQuery& query
        , const QString& whereClause
        , const std::map<QString, QVariant> bindings)
{
    QString queryString = "SELECT * FROM account WHERE " + whereClause;
    prepareAndExecute(query, queryString, bindings);
}

void account::destroy_record_by_id(QSqlDatabase& db, int id)
{
    throw error(QString(__FUNCTION__) + tr(" function not implemented yet"));

///    if (!db.isOpen())
///        throw error(tr("attempt to destroy an Account record in a Database "
///            "that is not open"));
///
///    QString queryString = "DELETE FROM account WHERE id = :id";
///
///    QSqlQuery query(db);
///    prepareAndExecute(query, queryString, {{":id", id}});
///
}   // end destroy_record_by_id

void account::destroy_record_by_full_path(
        QSqlDatabase& db, QString full_path)
{
///    if (!db.isOpen())
///        throw error(tr("attempt to destroy an Account record in a Database "
///            "that is not open"));
///
///    QString queryString = "DELETE FROM account WHERE full_path = :full_path";
///
///    QSqlQuery query(db);
///    prepareAndExecute(query, queryString, {{":full_path", full_path}});

    throw error(QString(__FUNCTION__) + tr(" function not implemented yet"));

}   // end destroy_record_by_full_path method

}}} // end ncountr::datastores::sqlite
