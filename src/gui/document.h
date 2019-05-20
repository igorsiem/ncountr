/**
 * \file document.h
 * Declare the document class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <memory>

#include <QCoreApplication>
#include <QDate>
#include <QException>
#include <QString>

#include <qlib/qlib.h>
///#include <api/api.h>

#include "api.h"
#include "utils/error.h"

#ifndef _gui_document_h_installed
#define _gui_document_h_installed

/**
 * \brief Class for encapsulating a datastore and asssociated types into
 * the concept of an editable 'Document'
 * 
 * A Document is GUI-level concept that acts as a (relatively thin) wrapper
 * for the API-level Datastore. See the \ref datastores page for more
 * information on the Datastore class hierarchy.
 * 
 * \todo Expand documentation
 */
class Document final
{

    // --- External Interface ---

    public:

    /**
     * \brief A unique pointer to a Document object
     */
    using DocumentUpr = std::unique_ptr<Document>;

    /**
     * \brief Exception class for signalling Document-related errors
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
     * \brief Constructor, initialising the underlying datastore
     * 
     * Note that this method would not normally be called directly. Static
     * `make*Document` methods are provided for creating and supplying
     * Datastore objects.
     * 
     * \param datastore A unique pointer to the Datastore object
     */
    explicit Document(Api::datastore_upr datastore);

    DECLARE_NO_MOVE_AND_COPY_SEMANTICS(Document)

    /**
     * \brief Create a new Document object based on a Sqlite Datastore
     * 
     * \param filePath The path of the Datastore object; note that a new
     * Datastore file will be created if it does not already exist.
     * 
     * \return A unique pointer to the new Datastore object
     * 
     * \throw Document::Error There are a problem opening the Datastore, or
     * creating the Document object
     */
    static DocumentUpr makeSqliteDocument(QString filePath);

    /**
     * \brief Suffix (extension) used for Sqlite Datastore Document file
     * names
     */
    static QString sqliteFileNameSuffix(void) { return QString("ncountr"); }

    // -- Document Information --

    /**
     * \brief Retrieve the Document Name
     */
    QString name(void) const
        { return QString::fromStdWString(m_datastore->name()); }

    /**
     *  \brief Set the Document Name
     */
    void setName(QString n)
        { m_datastore->set_name(n.toStdWString()); }

    /**
     * \brief Retrieve the Document Description
     */
    QString description(void) const
        { return QString::fromStdWString(m_datastore->description()); }

    /**
     * \brief Set the Document Description string
     */
    void setDescription(QString d)
        { m_datastore->set_description(d.toStdWString()); }

    // -- Account Management Methods --

    Api::account_spr createAccount(
            QString name
            , Api::account_spr parent
            , QString description
            , QDate openingDate
            , double openingBalance)
    {
        return m_datastore->create_account(
            name.toStdWString()
            , parent
            , description.toStdWString()
            , Utils::toApiDate(openingDate)
            , openingBalance);
    }   // end createAccount method

    Api::account_spr createAccount(
            QString name
            , Api::account_spr parent
            , QString description)
    {
        return m_datastore->create_account(
            name.toStdWString()
            , parent
            , description.toStdWString());
    }   // end createAccount method

    Api::account_spr findAccount(QString fullPath)
    {
        return m_datastore->find_account(fullPath.toStdWString());
    }

    Api::accounts_by_path_map findChildrenOf(Api::const_account_spr parent)
    {
        return m_datastore->find_children_of(parent);
    }

    Api::accounts_by_path_map findChildrenOf(QString fullPath)
    {
        return m_datastore->find_children_of(fullPath.toStdWString());
    }

    void destroyAccount(QString fullPath)
    {
        m_datastore->destroy_account(fullPath.toStdWString());
    }

    // --- Internal Declarations ---

    private:

    /**
     * \brief The underlying Datastore
     */
    Api::datastore_upr m_datastore;

    Q_DECLARE_TR_FUNCTIONS(Document)

};  // end Document class

/**
 * \brief A unique pointer to a Document object
 */
using DocumentUpr = Document::DocumentUpr;

#endif
