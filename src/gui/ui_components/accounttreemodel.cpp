/**
 * \file accounttreemodel.cpp
 * Implement the AccountTreeModel class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include "accounttreemodel.h"

AccountTreeModel::AccountTreeModel(
        Document& document
        , QObject* parent) :
    QAbstractItemModel(parent)
    , m_document(document)
    , m_rootItems()
{
    m_rootItems =
        toItemVec(m_document.findChildrenOf(Api::account_spr(nullptr)));
}   // end constructor

QModelIndex AccountTreeModel::index(
        int row
        , int column
        , const QModelIndex &parent) const
{
    // Make sure the requested item info is valid
    if (!hasIndex(row, column, parent)) return QModelIndex();
    if ((row < 0) || (column < 0) || (column >= 3)) return QModelIndex();

    // Determine what the Item is, depending whether it is at the root (i.e.
    // no valid parent) or not
    ItemSpr childItem = nullptr;
    if (parent.isValid())
    {
        auto parentItem =
            static_cast<Item*>(parent.internalPointer())->shared_from_this();
        if (row < parentItem->children.size())
            childItem = parentItem->children[row];
    }
    else if (row < m_rootItems.size()) childItem = m_rootItems[row];

    // Create a valid index if we have a valid child item
    if (childItem) return createIndex(row, column, childItem.get());
    return QModelIndex();    
}   // end index method

QModelIndex AccountTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) return QModelIndex();

    auto childItem =
        static_cast<Item*>(index.internalPointer())->shared_from_this();
    auto parentItem = childItem->parent.lock();

    if (parentItem) return createIndex(parentItem->row, 0, parentItem.get());
    else return QModelIndex();
}   // end parent method

int AccountTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<Item*>(parent.internalPointer())->columnCount();
    else return 3;
}   // end columnCount method

int AccountTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0) return 0;

    // Return the child count of the parent, if there is one; if not, return
    // the count of root items
    if (parent.isValid())
        return static_cast<Item*>(parent.internalPointer())->children.size();
    else return m_rootItems.size();
}   // end rowCount

QVariant AccountTreeModel::data(const QModelIndex &index, int role) const
{
    // No data if index is invalid
    if (!index.isValid()) return QVariant();

    // Only supporting Display roles for now
    if (role != Qt::DisplayRole) return QVariant();

    return static_cast<Item*>(index.internalPointer())->data(index.column());
}   // end data mehtod

Qt::ItemFlags AccountTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}   // end flags method

QVariant AccountTreeModel::headerData(
        int section
        , Qt::Orientation orientation
        , int role) const
{
    if (orientation != Qt::Horizontal) return QVariant();
    if (role != Qt::DisplayRole) return QVariant();

    switch (section)
    {
        case 0: return tr("Account Name");
        case 1: return tr("Opening Date");
        case 2: return tr("Opening Balance");
        default: return QVariant();
    }
}   // end headerData method

QVariant AccountTreeModel::Item::data(int column) const
{
    switch (column)
    {
        case 0: return QString::fromStdWString(account->name());

        case 1:
            if (account->has_running_balance())
                return Utils::toQDate(
                    std::get<0>(account->opening_data()));
            else return QVariant();

        case 2:
            if (account->has_running_balance())
                return std::get<1>(account->opening_data());
            else return QVariant();

        default: return QVariant();
    }   // end switch on column
}   // end data method

AccountTreeModel::ItemVec AccountTreeModel::toItemVec(
        Api::accounts_by_path_map accounts
        , ItemSpr parent)
{
    ItemVec itemVec;
    int r = 0;
    for (auto itr : accounts)
    {
        auto item = std::make_shared<Item>(
            itr.second
            , ItemVec()
            , parent
            , r++);

        item->children =
            toItemVec(m_document.findChildrenOf(itr.second), item);

        itemVec.push_back(item);
    }   // end accounts loop

    return itemVec;
}   // end toItemVec
