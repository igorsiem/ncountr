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

#include <vector>
#include <QAbstractItemModel>
#include "../api.h"
#include "../document.h"

#ifndef _gui_accounttreemodel_h_installed
#define _gui_accounttreemodel_h_installed

class AccountTreeModel : public QAbstractItemModel
{

    Q_OBJECT

    // --- External Interface ---

    public:

    /**
     * \brief Error exception for signalling errors related to this class
     * 
     * This sub-class modifies the interface slightly to allow initalisation
     * using a `QString`.
     */
    class Error : public ::Error
    {
        public:

        /**
         * \brief Constructor, setting the exception object with a QString
         * 
         * \param msg The human-readable error message
         */
        explicit Error(QString msg) : ::Error(msg) {}

        DECLARE_DEFAULT_VIRTUAL_DESTRUCTOR(Error)

        /**
         * \brief Throw the error object as an exception
         */
        virtual void raise(void) const override { throw *this; }

        /**
         * \`brief Create a cloned copy of the Error object
         * 
         * \return A pointer to a new copy of self, created using `new`
         */
        virtual Error* clone(void) const override
            { return new Error(*this); }

    };  // end Error class

    explicit AccountTreeModel(Document& document, QObject* parent = nullptr);

    virtual QModelIndex index(
        int row
        , int column
        , const QModelIndex &parent) const override;

    virtual QModelIndex parent(const QModelIndex &index) const override;

    virtual int rowCount(const QModelIndex &parent) const override;

    virtual int columnCount(const QModelIndex &parent) const override;

    virtual QVariant data(const QModelIndex &index, int role) const override;

    /**
     * \brief Retrieve various flags describing model items
     * 
     * This override essentially ensures that Views 'know' that the model
     * is read-only.
     * 
     * \todo Support an editable Accounts Tree
     */
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

    virtual QVariant headerData(
        int section
        , Qt::Orientation orientation
        , int role) const override;

    // --- Internal Declarations ---

    protected:

    struct Item : public std::enable_shared_from_this<Item>
    {

        using ItemSpr = std::shared_ptr<Item>;

        using ItemVec = std::vector<ItemSpr>;

        Api::account_spr account;

        ItemVec children;

        std::weak_ptr<Item> parent;

        int row;

        Item(
                Api::account_spr ac
                , ItemVec c
                , ItemSpr p
                , int r) :
            std::enable_shared_from_this<Item>()
            , account(ac)
            , children(c)
            , parent(p)
            , row(r)
        {}

        int columnCount() { return 3; }

        QVariant data(int column) const;

    };  // end Item class

    /**
     * \brief A shared pointer to an Item object
     */
    using ItemSpr = Item::ItemSpr;

    /**
     * \brief A vector of (shared pointers to) Item objects
     */
    using ItemVec = Item::ItemVec;

    // -- Internal Helper Methods --

    /**
     * \brief Convert a collection of Account objects *and their children*
     * to a collection of (shared pointers to) Item objects
     * 
     * This algorithm works recursively to convert *all* the descendant
     * Accounts under the collection of Accounts. Note that the order of the
     * Items is the same as the order of the accounts collection, so will
     * be in sorted order of Full Path.
     */
    ItemVec toItemVec(
        Api::accounts_by_path_map accounts
        , ItemSpr parent = nullptr);

    // -- Attributes --

    /**
     * \brief A reference to the Document object
     */
    Document& m_document;

    /**
     * \brief The root Items in the Account Items Tree
     */
    ItemVec m_rootItems;

};  // end AccountTreeModel class

#endif
