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
#include <QSplitter>

#include "../logging.h"
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
    QWidget* widget = ui->centralWidget;

    widget->setLayout(new QVBoxLayout(ui->centralWidget));

    setupTopLevelSplitters(widget);

}   // end setupCentralWidget method

void MainWindow::setupTopLevelSplitters(QWidget* parent)
{
    auto mainTopLevelSplitter = new QSplitter(Qt::Vertical, parent);
    parent->layout()->addWidget(mainTopLevelSplitter);

    // Top area
    auto mainTopFrame = new QFrame(mainTopLevelSplitter);
    mainTopFrame->setFrameStyle(QFrame::Box);
    mainTopFrame->setLayout(new QHBoxLayout(mainTopFrame));

    mainTopFrame->layout()->addWidget(createDocumentInfoWidget());
    mainTopFrame->layout()->addWidget(createViewInfoWidget());

    mainTopLevelSplitter->addWidget(mainTopFrame);

    // Left/right area beneath the top area
    auto leftRightSplitter =
        new QSplitter(Qt::Horizontal, mainTopLevelSplitter);

    auto leftFrame = new QFrame(leftRightSplitter);
    leftFrame->setFrameStyle(QFrame::Box);
    leftRightSplitter->addWidget(leftFrame);

    auto rightFrame = new QFrame(leftRightSplitter);
    rightFrame->setFrameStyle(QFrame::Box);
    leftRightSplitter->addWidget(rightFrame);
    
    // Set the left-right splitter geometry from persistent settings, and
    // make sure they get saved when they change
    m_settings.beginGroup("MainWindow");
    leftRightSplitter->setSizes(
        QList<int>({
            m_settings.value("leftRightSplitterLeft", 200).value<int>()
            , m_settings.value("leftRightSplitterRight", 1000).value<int>()
            }));
    m_settings.endGroup();

    connect(
        leftRightSplitter
        , &QSplitter::splitterMoved
        , [this, leftRightSplitter](int, int)
        {
            auto sizes = leftRightSplitter->sizes();
            m_settings.beginGroup("MainWindow");
            m_settings.setValue("leftRightSplitterLeft", sizes[0]);
            m_settings.setValue("leftRightSplitterRight", sizes[1]);
            m_settings.endGroup();
        });    

    leftRightSplitter->setChildrenCollapsible(false);

    mainTopLevelSplitter->addWidget(leftRightSplitter);

    // Set the main splitter geometry from persistent settings, and make
    // sure that they get saved when they change.
    m_settings.beginGroup("MainWindow");
    mainTopLevelSplitter->setSizes(
        QList<int>({
            m_settings.value("mainTopLevelSplitterTop", 50).value<int>()
            , m_settings.value(
                "mainTopLevelSplitterBottom"
                , 1000).value<int>()}));
    m_settings.endGroup();

    connect(
        mainTopLevelSplitter
        , &QSplitter::splitterMoved
        , [this, mainTopLevelSplitter](int, int)
        {
            auto sizes = mainTopLevelSplitter->sizes();
            m_settings.beginGroup("MainWindow");
            m_settings.setValue("mainTopLevelSplitterTop", sizes[0]);
            m_settings.setValue("mainTopLevelSplitterBottom", sizes[1]);
            m_settings.endGroup();
        });

    mainTopLevelSplitter->setChildrenCollapsible(false);
    
}   // end setupTopLevelSplitters

QFrame* MainWindow::createDocumentInfoWidget(void)
{
    auto docInfoWgt = new QFrame(this);
    docInfoWgt->setLayout(new QVBoxLayout(docInfoWgt));

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
        , docInfoWgt);

    m_nameFew->setCanEdit(false);
    docInfoWgt->layout()->addWidget(m_nameFew);

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
        , docInfoWgt);

    m_descriptionFew->setCanEdit(false);
    docInfoWgt->layout()->addWidget(m_descriptionFew);
    
    return docInfoWgt;
}   // end setupDocumentInfoArea method

QFrame* MainWindow::createViewInfoWidget(void)
{
    auto viewInfoWgt = new QFrame(this);
    viewInfoWgt->setLayout(new QVBoxLayout(viewInfoWgt));

    viewInfoWgt->layout()->addWidget(
        new QLabel(
            tr("some info about what we're looking at right now")
            , viewInfoWgt));

    return viewInfoWgt;
}   // end createViewInfoWidget
