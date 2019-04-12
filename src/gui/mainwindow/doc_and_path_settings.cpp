/**
 * \file doc_and_path_settings.cpp
 * Implement methods related to document and path settings in persistent
 * storage for the `MainWindow` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <QStandardPaths>
#include "../mainwindow.h"
#include "ui_mainwindow.h"

QString MainWindow::lastDocumentDirectoryUsed(void)
{
    QString p;
    m_settings.beginGroup("fileLocations");
    p = m_settings.value(
        "lastDocumentDirectoryUsed"
        , QStandardPaths::writableLocation(
            QStandardPaths::DocumentsLocation)).toString();
    m_settings.endGroup();

    return p;
}   // end getLastDocumentDirectoryUsed method

void MainWindow::setLastDocumentDirectoryUsed(const QString& lastDirUsed)
{
    m_settings.beginGroup("fileLocations");
    m_settings.setValue("lastDocumentDirectoryUsed", lastDirUsed);
    m_settings.endGroup();    
}   // end setLastDocumentDirectoryUsed method

QString MainWindow::lastDocumentFileUsed(void)
{
    QString p;
    m_settings.beginGroup("fileLocations");
    p = m_settings.value("lastDocumentFileUsed").toString();
    m_settings.endGroup();

    return p;
}   // end lastDocumentFileUsed method

void MainWindow::setLastDocumentFileUsed(const QString& filePath)
{
    m_settings.beginGroup("fileLocations");
    m_settings.setValue("lastDocumentFileUsed", filePath);
    m_settings.endGroup();    
}   // end setLastDocumentFileUsed method
