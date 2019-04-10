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

#include <QMainWindow>
#include <QSettings>

#include "config.h"
#include "document.h"

#ifndef _gui_mainwindow_h_installed
#define _gui_mainwindow_h_installed

/**
 * Qt framework generated user interface classes
 */
namespace Ui {
    class MainWindow;   ///< The Qt-generated version of the MainWindow class
}

/**
 * \brief The main window of the application
 * 
 * \todo Additional overview information
 * 
 * \todo Doc page on nCountr document files (somewhere)
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

    private:

    // -- UI Setup --

    /**
     * \brief Set up all user interface elements
     * 
     * This method is called from the constructor.
     */
    void setupUi(void);

    // - Actions and Commands -
    //
    // These methods are implemented in the
    // `mainwindow/setup_commands_and_actions.cpp` file.

    /**
     * \brief Set up User actions for the application, and connect them to
     * the GUI
     * 
     * User actions are best visualised as a menu tree, even though many of
     * them will be accessed via the Tool Bar.
     * 
     * * File menu:
     *   - New File action
     *   - Open File action
     *   - Close File action
     *   - Exit Application action
     * 
     * \todo Add more actions
     */
    void setupCommandTree(void);

    /**
     * \brief Set up the User actions associated with file-related
     * functionality, along with associated menu items and tool bar buttons
     */
    void setupFileActions(void);

    // -- User Command Execution --
    //
    // Note: these are implemented in the `mainwindow/file_actions.cpp
    // file

    /**
     * \brief Allow the User to create a new nCountr file, closing any file
     * that may currently be open
     * 
     * This method is called when the "New File" action is invoked.
     * 
     * Note that the logic of this involves choosing a file name / path
     * for the document immediately. This is because Documents are based on
     * Sqlite databases (at the moment), and so need a file straight away.
     * This means that a "Save File" dialog is opened, and we need logic for
     * checking whether chosen file paths already exist, and need to be
     * overwritten.
     * 
     * On the plus side, there's not need to save Documents before closing.
     */
    void executeFileNewFile(void);

    /**
     * \brief Allow the User to choose and open an nCountr file, closing
     * any file that may currently be open
     * 
     * This method is called when the "Open File" action is invoked.
     */
    void executeFileOpenFile(void);

    /**
     * \brief Close the currently open nCountr file
     * 
     * This method is called when the User invoked the "Close File" action.
     */
    void executeFileCloseFile(void);

    /**
     * \brief Close the main window and exit the application
     * 
     * This method is called when the User chooses "Exit Application" action
     * is invoked by the User.
     */
    void executeFileExitApplication(void);

    // -- Internal Helper Methods --

    // - Document and Path Settings -
    //
    // Note - these methods are implemented in the
    // `mainwindow/doc_and_path_settings.cpp` file

    /**
     * \brief Retrieve the directory last used with a Document file from
     * persistent settings storage
     * 
     * Note that if a directory has not previously been set, the standard
     * (system dependent), location for Documents is used.
     */
    QString lastDocumentDirectoryUsed(void);

    /**
     * \brief Save the path to the last directory used with Document file to
     * persistent settings storage
     * 
     * \param The directory path to set and save
     */
    void setLastDocumentDirectoryUsed(const QString& lastDirUsed);

    /**
     * \brief Retrieve the path of the last Document file created or opened
     * from persistent settings storage
     * 
     * \return The path of the last file used, or an empty string if the
     * info is not recorded; note that the actual file may -- or may not --
     * exist
     */
    QString lastDocumentFileUsed(void);

    /**
     * \brief Save the path of the last Document file used in persistent
     * settings storage.
     * 
     * This method is called when a Document file is opened or created.
     * 
     * \param filePath The path of the Document file
     */
    void setLastDocumentFileUsed(const QString& filePath);

    /**
     * \brief Add a message (possibly blank) to the Window title
     * 
     * This method is intended for adding something long-term and basic to
     * the title of the Main Window (like the name of the file that is
     * currently open)
     * 
     * \param msg The message to add to the window title
     */
    void setWindowTitleMessage(const QString& msg = "")
    {
        QString title = "nCountr";
        if (!msg.isEmpty()) title += " - " + msg;
        setWindowTitle(title);
    }

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

    /**
     * \brief The currently open document (when non-null)
     */
    DocumentUpr m_document;

};  // end MainWindow class

#endif
