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
#include <QVariant>

#include "account.h"
#include "logging.h"

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
    // Make sure the name is valid.
    if (!valid_name(name.toStdWString()))
        throw error(tr("invalid account name - ") + name);

    QString full_path = name;

    assert((t == type_t::income) || (t == type_t::expense));

    // Validate against the parent if we have one, or against the root
    // accounts if not.
    if (parent)
    {
        // Check that the type is compatible
        if (!compatible_parent_child_types(parent->account_type(), t))
        {
            throw error(
                tr("incompatible parent / child account types - ")
                + to_qstring(parent->account_type()) + QString(" - ")
                + to_qstring(t));
        }

        std::wstring parent_path = parent->full_path();
        std::vector<std::wstring> full_path_v = split_path(parent_path);
        full_path_v.push_back(name.toStdWString());

        full_path = QString::fromStdWString(concatenate_path(full_path_v));

    }

    if (find_by_full_path(db, full_path) != boost::none)
        throw error(tr("an account with this path / name already "
            "exists - ") + full_path);

    // Find a new ID
    int new_id = max_id(db) + 1;

    // Now we can create the record, and retrieve it by ID
    create_record(db, new_id, full_path, description, t);

    return std::make_shared<account>(db, new_id);
}   // end make_new method

account_spr account::make_new(
        QSqlDatabase& db
        , QString name
        , type_t t
        , account_spr parent
        , boost::optional<QString> description
        , QDate od
        , double ob)
{
    // Make sure the name is valid.
    if (!valid_name(name.toStdWString()))
        throw error(tr("invalid account name - ") + name);

    assert((t == type_t::asset) || (t == type_t::liability));

    QString full_path = name;

    // Validate against the parent if we have one, or against the root
    // accounts if not.
    if (parent)
    {
        // Check that the type is compatible
        if (!compatible_parent_child_types(parent->account_type(), t))
        {
            throw error(
                tr("incompatible parent / child account types - ")
                + to_qstring(parent->account_type()) + QString(" - ")
                + to_qstring(t));
        }

        std::wstring parent_path = parent->full_path();
        std::vector<std::wstring> full_path_v = split_path(parent_path);
        full_path_v.push_back(name.toStdWString());

        full_path = QString::fromStdWString(concatenate_path(full_path_v));

    }

    if (find_by_full_path(db, full_path) != boost::none)
        throw error(tr("an account with this path / name already "
            "exists - ") + full_path);

    // Find a new ID
    int new_id = max_id(db) + 1;

    // Now we can create the record, and retrieve it by ID
    create_record(db, new_id, full_path, description, t, od, ob);

    return std::make_shared<account>(db, new_id);

}   // end make_new method

account_spr account::find_existing(QSqlDatabase& db, QString full_path)
{
    // Make sure the record exists.
    auto rec = find_by_full_path(db, full_path);
    if (rec == boost::none) return nullptr;
    else return std::make_shared<account>(db, rec->value("id").value<int>());
}   // end find_existing

bool account::is_ready(void) const
{
    return (m_db.isOpen() && (m_id > 0));
}   // end  method

void account::destroy(void)
{
    if (descendants().size() > 0)
        throw error(tr("attempt to destroy an Account that has Child "
            "Accounts - ") + QString::fromStdWString(name()));

    destroy_record_by_id(m_db, m_id);

    m_id = 0;
}   // end  method

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

    // TODO add indices

}   // end initialise method

std::wstring account::name(void) const
{
    // Account Name is recorded as part of the Full Path, so get that, split
    // it, and return the last entry
    std::vector<std::wstring> full_path_v = split_path(full_path());
    return full_path_v.back();
}   // end name method

void account::set_name(std::wstring n)
{

    // Make sure the name is valid.
    if (!valid_name(n))
        throw error(tr("invalid account name - ") +
            QString::fromStdWString(n));

    // Form the new full path with the new name, and check whether it already
    // exists (we know it is not our own name)
    std::vector<std::wstring> full_path_v = split_path(full_path());
    full_path_v.back() = n;

    QString new_full_path =
        QString::fromStdWString(concatenate_path(full_path_v));

    // This method will check to ensure that there are no dups, and that this
    // is not already our existing full path
    set_full_path(new_full_path.toStdWString());

}   // end set_name method

std::wstring account::parent_path(void) const
{
    // Parent path is part of the full path, and remove the last element.
    std::vector<std::wstring> full_path_v = split_path(full_path());
    if (full_path_v.size() == 1) return L"";
    else
    {
        full_path_v.pop_back();
        return concatenate_path(full_path_v);
    }
}   // end parent_path method

api::account_spr account::parent(void) const
{
    return parent_sqlite();
}   // end parent method

account_spr account::parent_sqlite(void) const
{
    auto pp = parent_path();
    if (pp.empty()) return nullptr;

    return find_existing(m_db, QString::fromStdWString(pp));
}   // and parent_sqlite method

void account::set_parent(api::account_spr parent)
{

    QString new_full_path = QString::fromStdWString(name());

    // Do we set the parent to nullptr?
    if (parent)
    {
        // Determine what our new full path would be
        std::vector<std::wstring> full_path_v =
            split_path(parent->full_path());

        full_path_v.push_back(name());

        new_full_path =
            QString::fromStdWString(concatenate_path(full_path_v));
    }

    // This method checks that this is not already our full path, that
    // the account types are compatible, and that there are no
    // duplicates.
    set_full_path(new_full_path.toStdWString());
}   // end set_parent method

api::accounts_vec_t account::children(void) const
{
    auto c = children_sqlite();
    return api::accounts_vec_t(c.begin(), c.end());
}   // end children method

api::accounts_vec_t account::descendants(void) const
{
    // Transform to a base-class collection of Account objects
    auto d = descendants_sqlite();
    return base_t::accounts_vec_t(d.begin(), d.end());
}   // end descendants method

accounts_vec_t account::children_sqlite(void) const
{
    auto descendants = descendants_sqlite();

    auto fp = full_path();
    accounts_vec_t children;
    for (auto d : descendants)
        if (d->parent_path() == fp) children.push_back(d);

    return children;
}   // end children_sqlite

accounts_vec_t account::descendants_sqlite(void) const
{

    if (!m_db.isOpen())
        throw error(tr("attempt to retrieve Descendants of an Account "
            "in a database that is not open"));

    QSqlQuery query(m_db);
    select(
        query
        , "full_path LIKE '"
            + QString::fromStdWString(
                full_path()
                + account_path_separator)
            + QString("%'"));

    accounts_vec_t result;
    while (query.next())
    {
        auto full_path = query.value("full_path").value<QString>();
        result.push_back(find_existing(m_db, full_path));
    }

    return result;

}   // end descendants_sqlite method

std::wstring account::full_path(void) const
{
    return retrieveSingleRecordFieldValue<QString>(
        m_db
        , "account"
        , "full_path"
        , QString::fromStdString(
            "id = {}"_format(m_id))).toStdWString();
}   // end full_path method

void account::set_full_path(std::wstring p)
{

    if (p.empty())
        throw error(tr("cannot set the Path / Name of an Account to an "
            "empty string"));

    // Check whether this is our own full path, and also whether it
    // exists in another record.
    if (full_path() == p) return;

    if (find_by_full_path(m_db, QString::fromStdWString(p)) !=
            boost::none)
        throw error(tr("Account with this Path / Name already "
            "exists - ") + QString::fromStdWString(p));

    // If the full path includes a parent account, then check that the
    // parent exists, and has a compatible account type
    std::vector<std::wstring> full_path_v = split_path(p);
    std::wstring name = full_path_v.back();
    full_path_v.pop_back();

    if (!full_path_v.empty())
    {
        std::wstring parent_path = concatenate_path(full_path_v);

        auto parent =
            find_existing(m_db, QString::fromStdWString(parent_path));
        if (parent == nullptr)
            throw error(tr("parent Path does not exist - ") +
                QString::fromStdWString(parent_path));

        if (!compatible_parent_child_types(
                parent->account_type()
                , account_type()))
            throw error(
                tr("incompatible parent / child account types - ")
                + to_qstring(parent->account_type()) + QString(" - ")
                + to_qstring(account_type()));                

    }   // end if we have a parent

    // Make sure the name is legit
    if (!valid_name(name))
        throw error(tr("invalid Account Name - ") +
            QString::fromStdWString(name));

    // Get all the children of this account - we'll change their full
    // paths after we change our own
    auto children = children_sqlite();

    // Now we can update the Full Path in our field
    updateSingleRecordFieldValue(
        m_db
        , "account"
        , "full_path"
        , QString::fromStdWString(p)
        , QString::fromStdString("id = {}"_format(m_id)));

    // Update the children - set their full path, rather than their
    // parent item. It's more efficient, and means we don't have to faff
    // around with our own shared pointers.
    for (auto child : children)
    {
        auto child_full_path =
            p + account_path_separator + child->name();
        child->set_full_path(child_full_path);
    }

    // Note that all descendants will be updated appropriately, as this
    // method is called recursively.

}   // end set_full_path method

std::wstring account::description(void) const
{
    return retrieveSingleRecordFieldValue<QString>(
        m_db
        , "account"
        , "description"
        , QString::fromStdString(
            "id = {}"_format(m_id))).toStdWString();
}   // end description method

void account::set_description(std::wstring d)
{
    updateSingleRecordFieldValue(
        m_db
        , "account"
        , "description"
        , QString::fromStdWString(d)
        , QString::fromStdString("id = {}"_format(m_id)));
}

account::type_t account::account_type(void) const
{
    return to_account_type(retrieveSingleRecordFieldValue<QString>(
        m_db
        , "account"
        , "account_type"
        , QString::fromStdString("id = {}"_format(m_id))));
}   // end account_type method

void account::set_account_type(type_t t)
{

    // This overload is only for Income or Expense Accounts; use the other
    // one for Asset or Liability Accounts
    assert((t == type_t::income) || (t == type_t::expense));

    // If we have a parent, then check its type against ours
    auto parent = find_existing(
        m_db
        , QString::fromStdWString(parent_path()));
    if (parent)
    {
        if (!compatible_parent_child_types(parent->account_type(), t))
            throw error(
                tr("incompatible parent / child account types - ")
                + to_qstring(parent->account_type()) + QString(" - ")
                + to_qstring(t));
    }

    // If we have children - check their types against ours
    auto children = children_sqlite();
    for (auto child : children)
    {
        if (!compatible_parent_child_types(t, child->account_type()))
            throw error(
                tr("incompatible parent / child account types - ")
                + to_qstring(t) + QString(" - ")
                + to_qstring(child->account_type()));
    }   // end child loop

    updateSingleRecordFieldValue(
        m_db
        , "account"
        , "account_type"
        , to_qstring(t)
        , QString::fromStdString("id = {}"_format(m_id)));

}   // end set_account_type

void account::set_account_type(
        type_t t
        , ncountr::api::date opening_date
        , ncountr::api::currency_t opening_balance)
{

    // This overload is only for Asset or Liability accounts. Use the
    // other one for Income or Expense Accounts
    assert((t == type_t::asset) || (t == type_t::liability));

    // If we have a parent, then check its type against ours
    auto parent = find_existing(
        m_db
        , QString::fromStdWString(parent_path()));
    if (parent)
    {
        if (!compatible_parent_child_types(parent->account_type(), t))
            throw error(
                tr("incompatible parent / child account types - ")
                + to_qstring(parent->account_type()) + QString(" - ")
                + to_qstring(t));
    }

    // If we have children - check their types against ours
    auto children = children_sqlite();
    for (auto child : children)
    {
        if (!compatible_parent_child_types(t, child->account_type()))
            throw error(
                tr("incompatible parent / child account types - ")
                + to_qstring(t) + QString(" - ")
                + to_qstring(child->account_type()));
    }   // end child loop

    updateSingleRecordFieldValue(
        m_db
        , "account"
        , "account_type"
        , to_qstring(t)
        , QString::fromStdString("id = {}"_format(m_id)));

    set_opening_date(opening_date);
    set_opening_balance(opening_balance);

}   // end set_account_type method

ncountr::api::date account::opening_date(void) const
{
    // this method is only for assets or liabilities
    assert((account_type() == type_t::asset) ||
        (account_type() == type_t::liability));

    return to_api_date(
        QDate::fromJulianDay(retrieveSingleRecordFieldValue<qint64>(
            m_db
            , "account"
            , "opening_date"
            , QString::fromStdString("id = {}"_format(m_id)))));
}   // end opening_date

void account::set_opening_date(ncountr::api::date od)
{
    // this method is only for assets or liabilities
    assert((account_type() == type_t::asset) ||
        (account_type() == type_t::liability));

    updateSingleRecordFieldValue(
        m_db
        , "account"
        , "opening_date"
        , to_qdate(od).toJulianDay()
        , QString::fromStdString("id = {}"_format(m_id)));
}   // end set_opening_date

ncountr::api::currency_t account::opening_balance(void) const
{
    // this method is only for assets or liabilities
    assert((account_type() == type_t::asset) ||
        (account_type() == type_t::liability));

    return retrieveSingleRecordFieldValue<double>(
        m_db
        , "account"
        , "opening_balance"
        , QString::fromStdString("id = {}"_format(m_id)));
}   // end opening_balance

void  account::set_opening_balance(ncountr::api::currency_t ob)
{
    // this method is only for assets or liabilities
    assert((account_type() == type_t::asset) ||
        (account_type() == type_t::liability));

    updateSingleRecordFieldValue(
        m_db
        , "account"
        , "opening_balance"
        , ob
        , QString::fromStdString("id = {}"_format(m_id)));    
}   // end set_opening_balance method

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

    logger().log(
        level_t::debug
        , L"query: {}"_format(queryString.toStdWString()));

    QSqlQuery query(db);
    if (!query.prepare(queryString))
        throw error(
            tr("query preparation error: ")
            + query.lastError().text());

    if (!query.exec())
        throw error(
            tr("query execution error: ")
            + query.lastError().text());

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

    logger().log(
        level_t::debug
        , L"query: {}"_format(queryString.toStdWString()));
    
    QSqlQuery query(db);
    if (!query.prepare(queryString))
        throw error(tr("query preparation error: ") +
            query.lastError().text());

    // Value-binding
    query.bindValue(":id", QVariant(id), QSql::In);
    query.bindValue(":full_path", QVariant(full_path), QSql::In);

    if (description == boost::none)
        query.bindValue(
            ":description"
            , QVariant(QVariant::String)
            , QSql::In);
    else query.bindValue(":description", QVariant(*description), QSql::In);

    query.bindValue(":type", QVariant(to_qstring(t)), QSql::In);

     if (!query.exec())
         throw error(tr("query execution error: ") +
            query.lastError().text());
}   // end create_record method

void account::create_record(
        QSqlDatabase& db
        , int id
        , QString full_path
        , boost::optional<QString> description
        , type_t t
        , QDate od
        , double ob)
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
            ", opening_date"
            ", opening_balance"
        ") VALUES ("
            ":id"
            ", :full_path"
            ", :description"
            ", :type"
            ", :opening_date"
            ", :opening_balance"
        ");";

    logger().log(
        level_t::debug
        , L"query: {}"_format(queryString.toStdWString()));
    
    QSqlQuery query(db);
    if (!query.prepare(queryString))
        throw error(tr("query preparation error: ") +
            query.lastError().text());

    // Value-binding
    query.bindValue(":id", QVariant(id), QSql::In);
    query.bindValue(":full_path", QVariant(full_path), QSql::In);

    if (description == boost::none)
        query.bindValue(
            ":description"
            , QVariant(QVariant::String)
            , QSql::In);
    else query.bindValue(":description", QVariant(*description), QSql::In);

    query.bindValue(":type", QVariant(to_qstring(t)), QSql::In);

    query.bindValue(":opening_date", od.toJulianDay());
    query.bindValue(":opening_balance", ob);

     if (!query.exec())
         throw error(tr("query execution error: ") +
            query.lastError().text());

}

boost::optional<QSqlRecord> account::find_by_id(
        QSqlDatabase& db
        , int id)
{
    
    if (!db.isOpen())
        throw error(tr("attempt to find an Account record in a Database "
            "that is not open"));

    QSqlQuery query(db);

    select(query, "id = :id", {{":id", QVariant(id)}});

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

    QSqlQuery query(db);

    select(
        query
        , "full_path = :full_path"
        , {{":full_path", QVariant(full_path)}});

    if (query.next()) return query.record();
    else return boost::none;

}   //  end find_by_full_path method

void account::select(
        QSqlQuery& query
        , QString where_clause
        , std::map<QString, QVariant> bind_list)
{
    QString queryString = "SELECT * FROM account WHERE " + where_clause;

    logger().log(
        level_t::debug
        , L"query: {}"_format(queryString.toStdWString()));

    if (!query.prepare(queryString))
        throw error(
            tr("query preparation error: ")
            + query.lastError().text());

    for (auto bv : bind_list)
        query.bindValue(bv.first, bv.second, QSql::In);

    if (!query.exec())
        throw error(
            tr("query execution error: ")
            + query.lastError().text());

}   // end find_by_where_clause

void account::destroy_record_by_id(QSqlDatabase& db, int id)
{
    if (!db.isOpen())
        throw error(tr("attempt to destroy an Account record in a Database "
            "that is not open"));

    QString queryString = "DELETE FROM account WHERE id = :id";

    logger().log(
        level_t::debug
        , L"query: {}"_format(queryString.toStdWString()));

    QSqlQuery query(db);
    if (!query.prepare(queryString))
        throw error(
            tr("query preparation error: ")
            + query.lastError().text());

    query.bindValue(":id", QVariant(id), QSql::In);

    if (!query.exec())
        throw error(
            tr("query execution error: ")
            + query.lastError().text());

}   // end destroy_record_by_id

void account::destroy_record_by_full_path(
        QSqlDatabase& db, QString full_path)
{
    if (!db.isOpen())
        throw error(tr("attempt to destroy an Account record in a Database "
            "that is not open"));

    QString queryString = "DELETE FROM account WHERE full_path = :full_path";

    logger().log(
        level_t::debug
        , L"query: {}"_format(queryString.toStdWString()));

    QSqlQuery query(db);
    if (!query.prepare(queryString))
        throw error(
            tr("query preparation error: ")
            + query.lastError().text());

    query.bindValue(":full_path", QVariant(full_path), QSql::In);

    if (!query.exec())
        throw error(
            tr("query execution error: ")
            + query.lastError().text());

}   // end destroy_record_by_full_path method

}}} // end ncountr::datastores::sqlite
