/**
 * \file accounttreemodel.h
 * Implement the AccountTreeModel class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include "../../api/sqlite_datastore/db_utils.h"
#include "accounttreemodel.h"
#include "../utils/logging.h"

AccountTreeModel::AccountTreeModel(
        Document& document
        , QObject* parent) :
    QAbstractItemModel(parent)
    , m_document(document)
{
    m_items = Item::generateFullTree(document);
}   // end constructor

QModelIndex AccountTreeModel::index(
        int row
        , int column
        , const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) return QModelIndex();

    // If we have a valid parent, grab the child item from there; otherwise
    // take it from the root
    ItemSpr parentItem = nullptr, childItem = nullptr;
    if (parent.isValid())
    {
        parentItem =
            static_cast<Item*>(parent.internalPointer())->shared_from_this();

        if ((row >= 0) && (row < parentItem->childItems.size()))
            childItem = parentItem->childItems[row];
    }
    else
    {
        if ((row >= 0) && (row < m_items.size()))
            childItem = m_items[row];
    }

    if (childItem)
    {
        if ((column >= 0) && (column < childItem->columnCount()))
            return createIndex(row, column, childItem.get());
    }

    return QModelIndex();

}   // end index method

QModelIndex AccountTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) return QModelIndex();

    auto childItem =
        static_cast<Item*>(index.internalPointer())->shared_from_this();

    // If we have a parent, return an item for it; otherwise, we're at the
    // root, so there is no parent
    auto parentItem = childItem->parentItem.lock();
    if (parentItem)
        return createIndex(parentItem->rowNumber, 0, parentItem.get());
    else return QModelIndex();
}   // end parent method

int AccountTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0) return 0;

    if (parent.isValid())
        return static_cast<Item*>(
            parent.internalPointer())->childItems.size();
    else return m_items.size();
}   // end rowCount

QVariant AccountTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    auto item =
        static_cast<Item*>(index.internalPointer())->shared_from_this();
    auto account = item->account();

    switch (index.column())
    {
        case 0: return QVariant(QString::fromStdWString(account->name()));
        case 1:
            switch (account->account_type())
            {
                case Api::AccountType::asset:
                case Api::AccountType::liability:
                    return QVariant(
                        ncountr::datastores::sqlite::to_qdate(
                            account->opening_date()));

                default: return QVariant();
            }
        case 2:
            switch (account->account_type())
            {
                case Api::AccountType::asset:
                case Api::AccountType::liability:
                    return QVariant(account->opening_balance());

                default: return QVariant();
            }

        default: return QVariant();

    }   // end switch on column type

}   // end data method

QString AccountTreeModel::Item::fullPath(void) const
{
    // If we have a parent Item, get its full path, and add a trailing
    // separator
    QString parentPath;

    auto parent = parentItem.lock();

    if (parent)
        parentPath = parent->fullPath()
                    + QString::fromStdWString(
                        ncountr::api::account::account_path_separator);

    return parentPath + name;
}   // end fullPath

Api::AccountSpr AccountTreeModel::Item::account(void)
{
    return document.findAccountByFullPath(fullPath());
}   // end account method

Api::ConstAccountSpr AccountTreeModel::Item::account(void) const
{
    return document.findAccountByFullPath(fullPath());
}   // end account method

AccountTreeModel::Item::ItemsVec AccountTreeModel::Item::generateItemsVec(
        const Api::AccountsVec accounts
        , ItemSpr parent
        , Document& doc)
{

    ENC_LOG_DEBUG(L"accounts argument has size {}"_format(accounts.size()));

    ItemsVec items;

    for (auto ac : accounts)
    {
        items.push_back(
            std::make_shared<Item>(
                QString::fromStdWString(ac->name())
                , parent
                , doc
                , items.size()));
    }
    

    // Sort the items by Account Name
    std::sort(
        items.begin()
        , items.end()
        , [](ItemSpr a, ItemSpr b) { return a->name < b->name; });

    return items;

}   // end generateItemsVec method

AccountTreeModel::ItemsVec AccountTreeModel::Item::generateFullTree(
        Document& doc
        , ItemSpr parent)
{
    // Generate the initial list of Items from the root of the document or
    // from the children of the parent
    ItemsVec result;
    if (parent)
        result =
            generateItemsVec(parent->account()->children(), parent, doc);
    else result = generateItemsVec(doc.rootAccounts(), nullptr, doc);

    // For each item generated, call recursively for the descendant accounts
    for (auto it : result) it->childItems = generateFullTree(doc, it);

    return result;
}   // end generateFullTree

void AccountTreeModel::Item::setChildItems(
        const Api::AccountsVec childAccounts)
{
    childItems =
        generateItemsVec(childAccounts, shared_from_this(), document);
}   // end setChildItems

QVariant AccountTreeModel::Item::data(int column) const
{
    switch (column)
    {
        case 0: return QVariant(name);
        case 1:
        {
            auto ac = account();
            if ((ac->account_type() == Api::AccountType::asset)
                    || (ac->account_type() == Api::AccountType::liability))
                return QVariant(
                    ncountr::datastores::sqlite::to_qdate(
                        ac->opening_date()));
            else return QVariant();
        }

        case 2:
        {
            auto ac = account();
            if ((ac->account_type() == Api::AccountType::asset)
                    || (ac->account_type() == Api::AccountType::liability))
                return QVariant(ac->opening_balance());
            else return QVariant();
        }

        default: return QVariant();
    }
}   // end data method
