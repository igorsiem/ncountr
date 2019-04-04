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

#include <QHBoxLayout>
#include <QLabel>

#include <fmt/format.h>
using namespace fmt::literals;

#include <api/api.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Add a label for the API version
    ui->centralWidget->setLayout(new QHBoxLayout(ui->centralWidget));
    ui->centralWidget->layout()->addWidget(
        new QLabel(
            QString::fromStdString(
                "API version: {}"_format(api::version()))));
}   // end constructor

MainWindow::~MainWindow()
{
    delete ui;
}   // end destructor
