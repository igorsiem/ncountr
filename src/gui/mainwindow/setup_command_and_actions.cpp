/**
 * \file doc_and_path_settings.cpp
 * Implement methods related to setting up User commands and actions in the
 * `MainWindow` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include "../mainwindow.h"
#include "ui_mainwindow.h"

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
