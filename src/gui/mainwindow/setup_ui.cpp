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

    setWindowTitleMessage();

}   // end setupUi method
