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
    , m_accountTreeView(nullptr)
    , m_accountTreeModel(nullptr)
{
    setupUi();
}   // end constructor

MainWindow::~MainWindow()
{
    delete ui;
}   // end destructor

void MainWindow::closeEvent(QCloseEvent *event)
{

    // Record window geometry in persistent settings
    m_settings.beginGroup("MainWindow");
    m_settings.setValue("geometry", saveGeometry());
    m_settings.setValue("windowState", saveState());
    m_settings.endGroup();    

    QMainWindow::closeEvent(event);

}   // end closeEvent
