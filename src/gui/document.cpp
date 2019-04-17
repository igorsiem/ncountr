/**
 * \file document.cpp
 * Implement the document class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <QCoreApplication>
#include <QFileInfo>
#include "api/sqlite_datastore/datastore.h"
#include "document.h"

Document::Document(ncountr::api::datastore_upr datastore) :
    m_datastore(std::move(datastore))
{
}   // end constructor

DocumentUpr Document::makeSqliteDocument(QString filePath)
{
    // If the file does not exist, it will be created, and we need to run the
    // initialisation method after creation.
    bool init = false;
    if (!QFileInfo(filePath).exists()) init = true;

    auto ds = std::make_unique<ncountr::datastores::sqlite::datastore>(
        filePath);
    if (!ds->is_ready())
        Document::Error(tr("could not open datastore at \"") + filePath +
            "\"").raise();

    if (init) ds->initialise();

    return std::make_unique<Document>(std::move(ds));
}   // end makeSqliteDocument
