/**
 * \file mainwindow.h
 * Declare the main window class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#ifndef _gui_mainwindow_h_installed
#define _gui_mainwindow_h_installed

#include <QMainWindow>

/**
 * Qt framework generated user interface classes
 */
namespace Ui {
    class MainWindow;   ///< The Qt-generated version of the MainWindow class
}

/**
 * \brief The main window of the application
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:

    /**
     * \brief Trivial constructor
     * 
     * This method sets up all user interface elements for the window.
     * 
     * \param parent The parent UI object (usually `nullptr`)
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * \brief Destructor - destroys all user interface elements for this
     * window
     */
    ~MainWindow();

    private:

    /**
     * \brief Qt-generated framework for the main window
     */
    Ui::MainWindow *ui;

};  // end MainWindow class

#endif
