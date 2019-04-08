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

#include <QAction>
#include <QHBoxLayout>
#include <QLabel>

#include <fmt/format.h>
using namespace fmt::literals;

#include <api/api.h>

#include "error_handling.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(
        const bst::po::variables_map& clo
        , QSettings& settings
        , QWidget *parent) :
    QMainWindow(parent)
    , m_command_line_options(clo)
    , m_settings(settings)
    , ui(new Ui::MainWindow)
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

void MainWindow::setupUi(void)
{
    ui->setupUi(this);

    // Restore window geometry from persistent settings
    m_settings.beginGroup("MainWindow");
    restoreGeometry(m_settings.value("geometry").toByteArray());
    restoreState(m_settings.value("state").toByteArray());
    m_settings.endGroup();

///    // Add a label for the API version
///    ui->centralWidget->setLayout(new QHBoxLayout(ui->centralWidget));
///    ui->centralWidget->layout()->addWidget(
///        new QLabel(
///            QString::fromStdString(
///                "API version: {}"_format(ncountr::api::version()))));

    setupCommandTree();

}   // end setupUi method

void MainWindow::setupCommandTree(void)
{

    // Some tool bar setup
    ui->mainToolBar->setFloatable(false);
    ui->mainToolBar->setMovable(false);
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            
    setupFileActions();

}   // end setupActions

void MainWindow::setupFileActions(void)
{
    auto newFileAction =
        new QAction(QIcon(":/new_file"), tr("&New File..."), this);
    connect(
        newFileAction
        , &QAction::triggered
        , [this](void) { executeFileNewFile(); });

    auto fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newFileAction);

    auto openFileAction =
        new QAction(QIcon(":/open_file"), tr("&Open File..."), this);
    connect(
        openFileAction
        , &QAction::triggered
        , [this](void) { executeFileOpenFile(); });

    fileMenu->addAction(openFileAction);
    ui->mainToolBar->addAction(openFileAction);

    auto closeFileAction = new QAction(tr("Close File"), this);
    connect(
        closeFileAction
        , &QAction::triggered
        , [this](void) { executeFileCloseFile(); });
    fileMenu->addAction(closeFileAction);
    
    auto exitApplicationAction = new QAction(tr("E&xit Application"), this);
    connect(
        exitApplicationAction
        , &QAction::triggered
        , [this](void) { executeFileExitApplication(); });
    fileMenu->addSeparator();
    fileMenu->addAction(exitApplicationAction);
}   // end setupFileActions

void MainWindow::executeFileNewFile(void)
{
    ACTION_TRY
    {
        throw std::runtime_error("this action is not implemented yet");
    }
    ACTION_CATCH_DURING("New File")
}   // end executeFileNewFile

void MainWindow::executeFileOpenFile(void)
{
    ACTION_TRY
    {
        throw std::runtime_error("this action is not implemented yet");
    }
    ACTION_CATCH_DURING("Open File")
}   // end executeFileOpenFile

void MainWindow::executeFileCloseFile(void)
{
    ACTION_TRY
    {
        throw std::runtime_error("this action is not implemented yet");
    }
    ACTION_CATCH_DURING("Close File")
}   // end executeFileCloseFile

void MainWindow::executeFileExitApplication(void)
{
    ACTION_TRY
    {
        close();
        // throw std::runtime_error("this action is not implemented yet");
    }
    ACTION_CATCH_DURING("Exit Application")
}   // end executeFileExitApplication
