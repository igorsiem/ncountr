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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

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
        , widget);

    m_nameFew->setCanEdit(false);
    widget->layout()->addWidget(m_nameFew);

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
        , widget);

    m_descriptionFew->setCanEdit(false);
    widget->layout()->addWidget(m_descriptionFew);

}   // end setupCentralWidget method
