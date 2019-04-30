/**
 * \file mainwindow/notifications.cpp
 * Notification methods in the `MainWindow` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include "../mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::notifyDocumentOpened(void)
{

    m_nameFew->updateFromField();
    m_nameFew->setCanEdit(true);
    m_descriptionFew->updateFromField();
    m_descriptionFew->setCanEdit(true);

    if (m_accountTreeModel) delete m_accountTreeModel;
    m_accountTreeModel = new AccountTreeModel(*m_document, this);
    m_accountTreeView->setModel(m_accountTreeModel);

    // TODO set up other UI elements

}   // end notifyDocumentOpened method

void MainWindow::notifyDocumentAboutToClose(void)
{
    m_accountTreeView->setModel(nullptr);
    delete m_accountTreeModel;
    m_accountTreeModel = nullptr;
}   // end notifyDocumentAboutToClose

void MainWindow::notifyDocumentClosed(void)
{

    setWindowTitleMessage();
    m_nameFew->updateFromField();
    m_nameFew->setCanEdit(false);
    m_descriptionFew->updateFromField();
    m_descriptionFew->setCanEdit(false);

    // TODO clear other UI elements

}   // end notifyDocumentClosed method
