/**
 * \file mainwindow.cpp
 * Implement the main window class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_components/accounttreemodel.h"

MainWindow::MainWindow(
        const bst::po::variables_map& clo
        , QSettings& settings
        , QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_command_line_options(clo)
    , m_settings(settings)
    , m_document(nullptr)
    , m_nameFew(nullptr)
    , m_descriptionFew(nullptr)
    , m_accountsTreeView(nullptr)
{
    setupUi();
}   // end constructor

MainWindow::~MainWindow()
{
    delete ui;
}   // end destructor

void MainWindow::closeEvent(QCloseEvent *event)
{

    // Close the document if we have one
    if (m_document != nullptr)
    {
        documentBeforeClose();
        m_document = nullptr;
        documentAfterClose();
    }

    // Record window geometry in persistent settings
    m_settings.beginGroup("MainWindow");
    m_settings.setValue("geometry", saveGeometry());
    m_settings.setValue("windowState", saveState());
    m_settings.endGroup();    

    QMainWindow::closeEvent(event);

}   // end closeEvent

void MainWindow::documentAfterOpen(void)
{
    // Set the main Document Info fields up
    m_nameFew->updateFromField();
    m_nameFew->setCanEdit(true);
    m_descriptionFew->updateFromField();
    m_descriptionFew->setCanEdit(true);

    // Accounts Tree / Model
    //
    // Note: according to Qt docs, it is best to explicitly delete the
    // selection model when we change the item model, because the selection
    // model will be changed explicitly as well.
    auto selModel = m_accountsTreeView->selectionModel();
    m_accountsTreeView->setModel(new AccountTreeModel(*m_document, this));
    delete selModel;

    // TODO set up other UI elements
}   // end documentAfterOpen method

void MainWindow::documentBeforeClose(void)
{
    // Accounts Tree / Model
    auto selModel = m_accountsTreeView->selectionModel();
    auto model = m_accountsTreeView->model();
    m_accountsTreeView->setModel(nullptr);
    delete model;
    delete selModel;
}   // end documentBeforeClose method

void MainWindow::documentAfterClose(void)
{
        // Update UI
        m_nameFew->updateFromField();
        m_nameFew->setCanEdit(false);
        m_descriptionFew->updateFromField();
        m_descriptionFew->setCanEdit(false);

        // TODO clear other UI elements
}   // end documentAfterClose method
