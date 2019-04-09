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
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QStandardPaths>

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
    , ui(new Ui::MainWindow)
    , m_command_line_options(clo)
    , m_settings(settings)
    , m_document(nullptr)
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

    setupCommandTree();

    setWindowTitleMessage();

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

        QString filePath = QFileDialog::getSaveFileName(
            this
            , tr("New nCountr File")
            , lastDocumentDirectoryUsed()
            , tr("nCountr Files (*.ncountr);;All files (*.*)"));

        if (!filePath.isEmpty())
        {

            // Check our file name suffix, and also make sure we save the
            // document location for next time.
            //
            // If no file name suffix has been supplied, add our standard
            // one.
            QString suffix = QFileInfo(filePath).suffix();
            if (suffix.isEmpty())
                filePath += "." + Document::sqliteFileNameSuffix();

            QFileInfo fi(filePath);
            setLastDocumentDirectoryUsed(fi.absolutePath());

            // Can create the file now - if there is was a Document open
            // before, we assume it will be closed cleanly by destructors.
            m_document = std::move(Document::makeSqliteDocument(filePath));

            // Remember the path of the file for next time
            setLastDocumentFileUsed(filePath);

            QString msg = QString::fromStdWString(
                L"created \"{}.{}\" in \"{}\""_format(
                    fi.baseName().toStdWString()
                    , fi.suffix().toStdWString()
                    , fi.absolutePath().toStdWString()));

            logging::logger().log(
                logging::level_t::debug
                , msg.toStdWString());

            ui->statusBar->showMessage(msg, 5000);

            setWindowTitleMessage(fi.baseName());

        }   // end if the chosen file path is not empty
        
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
