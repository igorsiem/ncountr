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

void account::set_name(std::wstring n)
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
}   // end set_name method

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
}   // end concatenate_path method

std::wstring account::parent_path(void) const
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

void account::set_parent(api::account_spr parent)
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

void account::set_running_balance_true(
            ncountr::api::date od
            , ncountr::api::currency_t ob)
{
    // Can only do this if there are are no children or parent.
    if (parent_id() != boost::none)
        throw error(tr("attempt to add a running balance to an Account "
            "that is not at the root"));

    QSqlQuery query(m_db);
    select(
        query
        , "COUNT(id) AS id_count"
        , "parent_id = :parent_id"
        , {{":parent_id", id()}});
    if (query.next() == false)
        throw(tr("attempt to count child Accounts failed"));

    int child_count = query.value("id_count").toInt();

    if (child_count > 0)
        throw error(tr("attempt to add a running balance to an Account "
            "that has children"));

    updateFieldValue("has_running_balance", true);
    updateFieldValue("opening_date", to_qdate(od).toJulianDay());
    updateFieldValue("opening_balance", ob);
}   // end set_running_balance_true method

void account::set_running_balance_false(void)
{
    // Can only do this if there are are no children or parent.
    if (parent_id() != boost::none)
        throw error(tr("attempt to remove a running balance from an "
            "Account that is not at the root"));

    QSqlQuery query(m_db);
    select(
        query
        , "COUNT(id) AS id_count"
        , "parent_id = :parent_id"
        , {{":parent_id", id()}});
    if (query.next() == false)
        throw(tr("attempt to count child Accounts failed"));

    int child_count = query.value("id_count").toInt();

    if (child_count > 0)
        throw error(tr("attempt to remove a running balance from an "
            "Account that has children"));

    updateFieldValue("has_running_balance", false);
    updateFieldValue("opening_date", QVariant());
    updateFieldValue("opening_balance", QVariant());
}   // end set_running_balance_false method

std::tuple<ncountr::api::date, ncountr::api::currency_t>
account::opening_data(void) const
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

}   // end opening_data

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
        , const QString& selectClause
        , const QString& whereClause
        , const std::map<QString, QVariant> bindings)
{
    QString queryString =
        "SELECT "
        + selectClause
        + " FROM account WHERE "
        + whereClause;
    prepareAndExecute(query, queryString, bindings);
}   // end select method

void account::select(
        QSqlQuery& query
        , const QString& whereClause
        , const std::map<QString, QVariant> bindings)
{
    select(query, "*", whereClause, bindings);
}   // end select method

void account::destroy_record_by_id(QSqlDatabase& db, int id)
{

    if (!db.isOpen())
        throw error(tr("attempt to destroy an Account record in a Database "
            "that is not open"));

    // Make sure account has no children
    QSqlQuery query(db);
    select(
        query
        , "COUNT(id) AS id_count"
        , "parent_id = :parent_id"
        , {{":parent_id", id}});
    if (query.next() == false)
        throw(tr("attempt to count child Accounts failed"));

    int child_count = query.value("id_count").toInt();

    if (child_count > 0)
        throw error(tr("attempt to destroy an Account that has Children"));

    QString queryString = "DELETE FROM account WHERE id = :id";

///    QSqlQuery query(db);
    prepareAndExecute(query, queryString, {{":id", id}});

}   // end destroy_record_by_id

void account::destroy_record_by_full_path(
        QSqlDatabase& db, QString full_path)
{

    // Get the account object for its ID
    auto rec = find_by_full_path(db, full_path);
    if (rec == boost::none)
        throw error(
            tr("attempt to destroy an Account record that does not "
                "exist - ") + full_path);

    destroy_record_by_id(db, rec->value("id").toInt());
    
}   // end destroy_record_by_full_path method

}}} // end ncountr::datastores::sqlite
