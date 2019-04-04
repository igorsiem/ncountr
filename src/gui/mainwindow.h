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

#include <QSettings>
#include <QMainWindow>

#include "config.h"

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

    // --- External Interface ---

    public:

    /**
     * \brief Trivial constructor
     * 
     * This method sets up all user interface elements for the window.
     * 
     * \param clo The Boost Variables Map of options parsed from the
     * command-line
     * 
     * \param settings Persistent user settings object
     * 
     * \param parent The parent UI object (usually `nullptr`)
     */
    explicit MainWindow(
        const bst::po::variables_map& clo
        , QSettings& settings
        , QWidget *parent = nullptr);

    /**
     * \brief Destructor - destroys all user interface elements for this
     * window
     */
    ~MainWindow();

    // --- Internal Declarations ---

    // -- Event-handler Overrides --

    protected:

    /**
     * \brief Write window state and geometry to persistent storage
     * 
     * \param event The event object (passed to base-class implementation) 
     */
    virtual void closeEvent(QCloseEvent *event) override;

    // -- Internal Helper Methods --

    private:

    /**
     * \brief Set up all user interface elements
     * 
     * This method is called from the constructor.
     */
    void setupUi(void);

    // -- Attributes --

    private:

    /**
     * \brief Qt-generated framework for the main window
     */
    Ui::MainWindow *ui;

    /**
     * \brief The variables map of options parsed from the command-line
     */
    const bst::po::variables_map& m_command_line_options;

    /**
     * \brief Persistent user settings for the application
     */
    QSettings& m_settings;

};  // end MainWindow class

#endif
