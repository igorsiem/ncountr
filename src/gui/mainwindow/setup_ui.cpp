/**
 * \file setup_ui.cpp
 * Implement methods related to setting up the User Interface in the
 * `MainWindow` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSizePolicy>

#include "../utils/logging.h"
#include "../mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::setupUi(void)
{
    ui->setupUi(this);

    // Restore window geometry from persistent settings
    m_settings.beginGroup("MainWindow");
    restoreGeometry(m_settings.value("geometry").toByteArray());
    restoreState(m_settings.value("state").toByteArray());
    m_settings.endGroup();

    setupCommandTree();
    setupCentralWidget();

    setWindowTitleMessage();

}   // end setupUi method

void MainWindow::setupCentralWidget(void)
{
    ui->centralWidget->setLayout(new QVBoxLayout());

    auto docInfo =  createDocumentInformationWidget();
    docInfo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    ui->centralWidget->layout()->addWidget(docInfo);

    auto leftRightSplitter = createLeftRightSplitter();
    leftRightSplitter->setSizePolicy(
        QSizePolicy::Preferred
        , QSizePolicy::Expanding);
    ui->centralWidget->layout()->addWidget(leftRightSplitter);

}   // end setupCentralWidget method

QWidget* MainWindow::createDocumentInformationWidget(void)
{
    auto docInfo = new QFrame(this);

    docInfo->setLayout(new QVBoxLayout());

    m_nameFew = new TextFieldLineEdit(
        [this](void)
        {
            if (m_document) return m_document->name();
            else return QString("");
        }
        , [this](QString name)
        {
            if (m_document) m_document->setName(name);
            else logging::logger().log(
                logging::level_t::warning
                , tr("attempted to set the Document Name, but no "
                    "document is open").toStdWString());
        }
        , docInfo);

    m_nameFew->setCanEdit(false);
    docInfo->layout()->addWidget(m_nameFew);

    m_descriptionFew = new TextFieldLineEdit(
        [this](void)
        {
            if (m_document) return m_document->description();
            else return QString("");
        }
        , [this](QString description)
        {
            if (m_document) m_document->setDescription(description);
            else logging::logger().log(
                logging::level_t::warning
                , tr("attempted to set the Document Description, but no "
                    "document is open").toStdWString());
        }
        , docInfo);

    m_descriptionFew->setCanEdit(false);
    docInfo->layout()->addWidget(m_descriptionFew);

    return docInfo;
}   // end createDocumentInformationWidget method

QSplitter* MainWindow::createLeftRightSplitter(void)
{
    auto leftFrame = new QFrame(this), rightFrame = new QFrame(this);
    leftFrame->setFrameStyle(QFrame::Box);
    rightFrame->setFrameStyle(QFrame::Box);

    auto splitter = new QSplitter(this);
    splitter->addWidget(leftFrame);
    splitter->addWidget(rightFrame);

    // Get widgt proportions set up, and make sure they are saved as well.
    m_settings.beginGroup("MainWindow");
    auto leftSize = m_settings.value("leftRightSplitterLeft", 50).toInt();
    auto rightSize =
        m_settings.value("leftRightSplitterRight", 1000).toInt();
    m_settings.endGroup();
    splitter->setSizes(QList<int>({leftSize, rightSize}));

    connect(
        splitter,
        &QSplitter::splitterMoved,
        [this, splitter](int, int)
        {
            // Splitter has moved - write its sizes to persistent storage
            auto sizes = splitter->sizes();
            m_settings.beginGroup("MainWindow");
            m_settings.setValue("leftRightSplitterLeft", sizes[0]);
            m_settings.setValue("leftRightSplitterRight", sizes[1]);
            m_settings.endGroup();
        });

    // Set up the contents of the frames
    setupAccountTreeView();
    leftFrame->setLayout(new QVBoxLayout(this));
    leftFrame->layout()->addWidget(m_accountsTreeView);

    return splitter;
}   // end createLeftRightSplitter method

void MainWindow::setupAccountTreeView(void)
{
    m_accountsTreeView = new QTreeView(this);
    // TODO look and feel stuff
}