/**
 * \file file_actions.cpp
 * Implement file-related actions in the MainWindow class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <QFileDialog>
#include <QStandardPaths>

#include <fmt/format.h>
using namespace fmt::literals;

#include <qlib/qlib.h>

#include "../utils/error_handling.h"
#include "../mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::executeFileNewFile(void)
{
    ACTION_TRY
    {

        // Need to use a QFileDialog object instead of an ordinary static
        // method to give us finer control over file selection.
        QFileDialog dlg(
            this
            , tr("New nCountr File")
            , lastDocumentDirectoryUsed()
            , tr("nCountr Files (*.ncountr);;All files (*.*)"));

        dlg.setAcceptMode(QFileDialog::AcceptSave);
        dlg.setDefaultSuffix(Document::sqliteFileNameSuffix());

        if (dlg.exec() == QDialog::Accepted)
        {

            auto selectedFiles = dlg.selectedFiles();
            QString filePath = dlg.selectedFiles()[0];
            
            // Make sure we save the document location for next time.
            QFileInfo fi(filePath);
            setLastDocumentDirectoryUsed(fi.absolutePath());

            // If the file exists (User will have already confirmed the
            // overwrite), we need to delete the existing file so that
            // Sqlite doesn't just open the existing file.
            if (fi.exists())
            {
                if (QFile(filePath).remove())
                    logging::logger().log(
                        logging::level_t::debug
                        , L"file \"{}\" exists, so it was deleted before "
                            "creating the new doucment"_format(
                                filePath.toStdWString()));
                else Document::Error(tr("file already exists, and could "
                    "not be deleted - ") + filePath).raise();
            }

            // If there is a document open already, close it now.
            if (m_document != nullptr)
            {
                documentBeforeClose();
                m_document = nullptr;
                documentAfterClose();
            }

            // Now we can create our new document
            m_document = std::move(Document::makeSqliteDocument(filePath));
            documentAfterOpen();

            // Remember the path of the file for next time
            setLastDocumentFileUsed(filePath);

            QString msg = tr("created document ") + filePath;

            logging::logger().log(
                logging::level_t::debug
                , msg.toStdWString());

            // Update UI
            ui->statusBar->showMessage(msg, 5000);
            setWindowTitleMessage(fi.baseName());
///            m_nameFew->updateFromField();
///            m_nameFew->setCanEdit(true);
///            m_descriptionFew->updateFromField();
///            m_descriptionFew->setCanEdit(true);

            // TODO set up other UI elements

        }   // end if the chosen file path is not empty
        
    }
    ACTION_CATCH_DURING("New File")
}   // end executeFileNewFile

void MainWindow::executeFileOpenFile(void)
{
    ACTION_TRY
    {

        QString filePath = QFileDialog::getOpenFileName(
            this
            , tr("Open nCountr File")
            , lastDocumentDirectoryUsed()
            , tr("nCountr Files (*.ncountr);;All files (*.*)"));

        if (!filePath.isEmpty())
        {

            // Save the folder location for next time
            QFileInfo fi(filePath);
            setLastDocumentDirectoryUsed(fi.absolutePath());

            // Check that the file exists, and is a file.
            if (!fi.exists())
                Document::Error(tr("file does not exist - ") +
                    filePath).raise();

            if (!fi.isFile())
                Document::Error(tr("selected path is not a file (probably a "
                    "directory) - ")  + filePath).raise();

///            // Open the file - any existing document will be closed cleanly
///            // by destructors (we assume)
///            //
///            // Note that we have to explicitly release the database before
///            // recreating it, so that connections are cleaned up before new
///            // ones are created.
///            m_document = nullptr;

            // If there is a document open already, close it now.
            if (m_document != nullptr)
            {
                documentBeforeClose();
                m_document = nullptr;
                documentAfterClose();
            }

            m_document = std::move(Document::makeSqliteDocument(filePath));
            documentAfterOpen();

            // Remember the path of the file for next time
            setLastDocumentFileUsed(filePath);

            QString msg = tr("opened document ") + filePath;
            logging::logger().log(
                logging::level_t::debug
                , msg.toStdWString());

            ui->statusBar->showMessage(msg, 5000);
            setWindowTitleMessage(fi.baseName());
            
        }   // end if a file name was selected

    }
    ACTION_CATCH_DURING("Open File")
}   // end executeFileOpenFile

void MainWindow::executeFileCloseFile(void)
{
    ACTION_TRY
    {

        if (m_document != nullptr)
        {
            documentBeforeClose();
            m_document = nullptr;
            documentAfterClose();
        }

        QString msg = tr("document closed");

        logging::logger().log(
            logging::level_t::debug
            , msg.toStdWString());

        ui->statusBar->showMessage(msg, 5000);
        setWindowTitleMessage();

    }
    ACTION_CATCH_DURING("Close File")
}   // end executeFileCloseFile

void MainWindow::executeFileExitApplication(void)
{
    ACTION_TRY
    {
        close();
    }
    ACTION_CATCH_DURING("Exit Application")
}   // end executeFileExitApplication
