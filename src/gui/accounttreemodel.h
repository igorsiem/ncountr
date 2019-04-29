/**
 * \file accounttreemodel.h
 * Declare the AccountTreeModel class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <boost/optional.hpp>

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "document.h"

#ifndef _gui_accounttreemodel_h_included
#define _gui_accounttreemodel_h_included

/**
 * \brief An Item Model for the Accounts in a Document
 * 
 * This class exposes an hierarchical tree of Accounts, arranged by Full
 * Path to produce Parent / Child relationships. Data columents are
 * 
 * 0. Name (String)
 * 1. Opening Date (Date, for Assets and Liabilities)
 * 2. Opening Balance (Currency, for Assets and Liabilities)
 * 
 * \todo Expand documentation
 */
class AccountTreeModel : public QAbstractItemModel
{

    Q_OBJECT

    public:

    /**
     * \brief Exception class for signalling error related to the
     * AccountTreeModel
     */
    class Error : public ::Error
    {
        public:
        
        /**
         * \brief Constructor, initialising the Error object
         * 
         * \param msg The human-readable message for the Error
         */
        explicit Error(QString msg) : ::Error(std::move(msg)) {}

        /// \cond
        DECLARE_DEFAULT_VIRTUAL_DESTRUCTOR(Error);
        /// \endcond
    };  // end Error exception class

    /**
     * \brief Constructor for the Account Tree Model
     * 
     * \param document The Document object; this must exist for the life of
     * the Model
     * 
     * \param parent The optional parent object
     */
    explicit AccountTreeModel(
        Document& document
        , QObject* parent = nullptr);

    DECLARE_DEFAULT_VIRTUAL_DESTRUCTOR(AccountTreeModel)

    // -- Reimplemented Model Methods --

    /**
     * \brief Retrieve a QModelIndex object for a given Item in the Tree
     * 
     * Note that Items are referenced in the QModelIndex as *raw pointers*.
     * 
     * \param row The row of the item (under the parent or root)
     * 
     * \param column The column of the data item
     * 
     * \param parent The index of the parent item; this is invalid if the
     * desired item is at the root
     */
    virtual QModelIndex index(
        int row
        , int column
        , const QModelIndex &parent) const override;

    /**
     * \brief Retrieve a parent index for a given item index
     */
    virtual QModelIndex parent(const QModelIndex &index) const override;

    /**
     * \brief Retrieve the number of rows under a given parent (or the root,
     * if the index is invalid)
     */
    virtual int rowCount(const QModelIndex &parent) const override;

    /**
     * \brief Retrieve the column count for an Item (it's always 3 in this
     * iteration)
     */
    virtual int columnCount(const QModelIndex &parent) const override
        { return 3; }

    /**
     * \brief Retrieve the data for a given index and role
     * 
     * This method return data by columns, as described in the class
     * documentation. Only data for the `Qt::DisplayRole` is returned. If the
     * index is invalid or references a column outside the range [0,2], or
     * the role is anything other than `Qt::DisplayRole` a null `QVariant` is
     * returned.
     * 
     * This effectively means that the Tree View can only be used for display
     * in this iteration.
     * 
     * \todo Support other roles, including editing
     */
    virtual QVariant data(
        const QModelIndex &index
        , int role) const override;

    /**
     * \brief Retrieve the flags that govern the behaviour of items in the
     * tree
     * 
     * This default behaviour ensures that all items are read-only.
     * 
     * \todo Support editing of Items
     */
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        if (!index.isValid())
            return 0;

        return QAbstractItemModel::flags(index);
    }   // end flags method

    /**
     * \brief Retrieve column header labels
     */
    QVariant headerData(
        int section
        , Qt::Orientation orientation
        , int role) const override
    {
        if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
            return QVariant();

        switch (section)
        {
            case 0: return tr("Account Name");
            case 1: return tr("Opening Date");
            case 2: return tr("Opening Balance");

            default: return QVariant();
        }

    }   // end headerDate

    // --- Internal Declarations ---

    private:

    /**
     * \brief An Item in the Tree structure, corresponding to an Account
     * in the Document
     * 
     * This is close to a POD structure - it just pulls together the various
     * data that a node in the Tree needs to manage its information.
     * Attributes are public, and accessed directly in most cases, except
     * where some helpful functionality has been added (e.g. setting up
     * the list of child items).
     */
    struct Item : public std::enable_shared_from_this<Item>
    {

        /**
         * \brief A weak pointer to an Item object
         */
        using ItemWpr = std::weak_ptr<Item>;

        /**
         * \brief A shared pointer to an Item object
         */
        using ItemSpr = std::shared_ptr<Item>;

        /**
         * \brief A shared pointer to a const Item
         */
        using ConstItemSpr = std::shared_ptr<const Item>;

        /**
         * \brief A collection of (shared pointer to) Item objects
         */
        using ItemsVec = std::vector<ItemSpr>;

        QString name;   ///< Name (*not* Path) of this Account

        ItemWpr parentItem; ///< Pointer to parent Item

        ItemsVec childItems;    ///< Collection of child Items

        int rowNumber;  ///< Row number within our parent item (or root)

        Document& document; ///< Reference to the Document object

        /**
         * \brief Constructor, initialising the Account Name (*not* the
         * Path), the Parent Item (as a weak pointer), and thr row number
         */
        explicit Item(QString n, ItemWpr p, Document& doc, int rn) :
                std::enable_shared_from_this<Item>()
                , name(n)
                , parentItem(p)
                , childItems()
                , rowNumber(rn)
                , document(doc)
            {}

        /**
         * \brief Derive the Full Path of the Account for this Item
         */
        QString fullPath(void) const;

        /**
         * \brief Retrieve the Account object for this Item
         */
        Api::AccountSpr account(void);

        /**
         * \brief Retrieve the (const) Account object for this Item
         */
        Api::ConstAccountSpr account(void) const;

        /**
         * \brief Generate a collection of Name-indexed Item object from a
         * list of accounts
         * 
         * Items in the returned collection will be sorted by Name
         * 
         * \param accounts The collection of Account objects from which to
         * generate the Items
         * 
         * \param parent The (possibly `nullptr`) Item to use as the parent
         * of the generated Items
         * 
         * \param doc Reference to the document object
         */
        static ItemsVec generateItemsVec(
            const Api::AccountsVec accounts
            , ItemSpr parent
            , Document& doc);

        /**
         * \brief Generate a full tree of Items from the correspodning
         * Accounts tree
         * 
         * This method is called recursively, starting with the root Accounts
         * of the document (parent set to `nullptr`), and then for each
         * child account.
         */
        static ItemsVec generateFullTree(
            Document& doc
            , ItemSpr parent = nullptr);

        /**
         * \brief Generate child Items from the given set of Accounts
         */
        void setChildItems(const Api::AccountsVec childAccounts);

        /**
         * \brief Retrieve the number of data columns in this model
         * 
         * In the current implementation, this is always three
         */
        int columnCount(void) const { return 3; }

        /**
         * \brief Retrieve a datum from the Item, given the column number
         * 
         * This method returns the appropriate field from the Account pointed
         * to by the Item. Fields are specified by columns as:
         * 
         * 0. Account Name (string)
         * 1. Opening Data (date, Assets and Liabilities only)
         * 2. Opening Balance (currency, Assets and Liabilities only)
         */
        QVariant data(int column) const;

    };  // end Item class

    /**
     * \brief A shared pointer to an Item
     */
    using ItemSpr = Item::ItemSpr;

    /**
     * \brief A pointer to a const Item object
     */
    using ConstItemSpr = Item::ConstItemSpr;

    /**
     * \brief A map of (shared pointers to) Item objects, indexed by Name
     */
    using ItemsVec = Item::ItemsVec;

    /**
     * \brief The Document object containing the Accounts
     * 
     * This must exist for the life of the Model
     */
    Document& m_document;

    // The list of Items at the root
    ItemsVec m_items;

};  // end AccountTreeModel class

#endif
