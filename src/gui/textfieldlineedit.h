/**
 * Declare the `TextFieldLineEdit` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <functional>

#include <QEvent>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QVariant>
#include <QWidget>

#include "error.h"

#ifndef _gui_textfieldlineedit_h_installed
#define _gui_textfieldlineedit_h_installed

/**
 * \brief A widget for displaying and editing some data field as text
 * 
 * This widget can display a value as read-only 'display' text, or make it
 * editable. Data is retrieved and saved via externally-supplied invokable
 * objects (e.g. lambda functions)
 * 
 * \todo Expand documentation here
 */
class TextFieldLineEdit : public QWidget
{

    Q_OBJECT

    // --- External Interface ---

    public:

    /**
     * \brief Error exception for signalling errors related to this Widget
     * 
     * This sub-class modifies the interface slightly to allow initalisation
     * using a `QString`.
     */
    class Error : public ::Error
    {
        public:

        /**
         * \brief Constructor, setting the exception object with a QString
         * 
         * \param msg The human-readable error message
         */
        explicit Error(QString msg) : ::Error(msg) {}

        DECLARE_DEFAULT_VIRTUAL_DESTRUCTOR(Error)

        /**
         * \brief Throw the error object as an exception
         */
        virtual void raise(void) const override { throw *this; }

        /**
         * \`brief Create a cloned copy of the Error object
         * 
         * \return A pointer to a new copy of self, created using `new`
         */
        virtual Error* clone(void) const override
            { return new Error(*this); }

    };  // end Error class

    /**
     * \brief Enumerate the modes in which the field editor may operate
     */
    enum class mode_t
    {
        display         ///< Read-only display mode
        , edit          ///< Edit mode
    };

    /**
     * \brief Callable object, function or lambda for retrieving the value
     * of the Field from the source document when necessary
     * 
     * The return value is the field value, expressed as a string for editing
     */
    using RetrieveFieldValueFn = std::function<QString(void)>;

    /**
     * \brief Callable object for saving the vlaue of the Field
     * 
     * The argument is the field data, expressed as a string after editing
     */
    using SaveFieldValueFn = std::function<void(QString)>;

    /**
     * \brief Constructor - initialising all attributes
     * 
     * \param rFn Invokable object / lambda for retrieving the editable value
     * from its source
     * 
     * \param sFn Invokable object / lambda for saving the editable value
     * 
     * \param parent The parent widget
     */
    explicit TextFieldLineEdit(
        RetrieveFieldValueFn rFn
        , SaveFieldValueFn sFn
        , QWidget* parent = nullptr);

    /**
     * \brief Call the `RetrieveFieldValueFn` object to retrieve the value of
     * the field and make it visible in the widget
     */
    void updateFromField(void);

    /**
     * \brief Call the `SaveFieldValueFn` object to save the value of the
     * edited field back to its source
     */
    void saveToField(void);

    /**
     * \brief Set the operating mode of the widget - editing or displaying
     * 
     * Note: if an attempt is made to set the mode to `edit`, the request is
     * ignored, and a warning is issued.
     * 
     * \param m The mode to set
     */
    void setMode(mode_t m);

    /**
     * \brief Check whether the widget may be placed in Edit mode
     */
    bool canEdit(void) const { return m_editButton->isEnabled(); }

    /**
     * \brief Set the flag for whether or not the widget may be placed in
     * edit mode or not
     * 
     * Note that -- if editing is disabled -- the edit button will be
     * disabled, and the double-click behaviour of the Line Edit control will
     * not put the control into edit mode.
     */
    void setCanEdit(bool c) { m_editButton->setEnabled(c); }

    // --- Internal Declarations ---

    protected:

    // -- Event Methods --

    /**
     * \brief Perform event filtering for the control
     * 
     * This method is overridden from the base class to catch double-click
     * events in the Line Edit control (which does not trap its own
     * double-click) events. If a double-click event is detected for the
     * Line Edit, the `onLineEditDoubleClicked` method is called.
     * 
     * All other events are filtered using the base-class method.
     * 
     * \param obj The object for which the event occurred
     * 
     * \param event The event object being checked
     */
    virtual bool eventFilter(QObject* obj, QEvent* event);

    /**
     * \brief Invoke editing mode for the widget when the Line Edit control
     * is double-clicked
     */
    void onLineEditDoubleClicked(void);

    /**
     * \brief Invoke the 'Save' action when the User presses Return after
     * editing the control text
     */
    void onLinedEditReturnPressed(void);

    /**
     * \brief Invoke editing mode for the widget when the 'Edit' button is
     * clicked
     */
    void onEditButtonClicked(void);

    /**
     * \brief Invoke the 'Save' action when the 'Save' button is clicked
     */
    void onSaveButtonClicked(void);

    /**
     * \brief Invoke the 'Cancel' action when the User clicks on the 'Cancel'
     * button
     */
    void onCancelButtonClicked(void);

    // -- Action Methods --

    /**
     * \brief Begin editing the field value, placing the control in 'Edit'
     * mode
     * 
     * The field value is retrieved from the source, to make sure it is
     * up to date.
     */
    void executeStartEditAction(void);

    /**
     * \brief Cancel the editing action, retrieving the original field
     * value from the source, and placing the control in display mode 
     */
    void executeCancelEditAction(void);

    /**
     * \brief Save the edited value to the source and place the control in
     * display mode.
     */
    void executeSaveEditAction(void);

    // -- Attributes --

    QLineEdit* m_lineEdit;      ///< The line editor box

    QPushButton* m_editButton;  ///< Button to begin editing

    QPushButton* m_saveButton;  ///< The 'Save' button

    QPushButton* m_cancelButton;    ///< The 'Cancel' button

    /**
     * \brief Invocable object for retreiving the field value
     * 
     * Note that this *may* be `nullptr`, and this should be checked before
     * invoking.
     */
    RetrieveFieldValueFn retrieve;

    /**
     * \brief Invocable object for saving the field value
     * 
     * Note that this *may* be `nullptr`, and this should be checked before
     * invoking.
     */
    SaveFieldValueFn save;

    mode_t m_currentMode;   ///< What mode we are currently operating in

};  // end TextFieldLineEdit class

#endif
