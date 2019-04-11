/**
 * \file textfieldlineedit.cpp
 * Implement the `TextFieldLineEdit` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <QHBoxLayout>
#include "error_handling.h"
#include "textfieldlineedit.h"

TextFieldLineEdit::TextFieldLineEdit(
        RetrieveFieldValueFn rFn
        , SaveFieldValueFn sFn
        , QWidget* parent) :
    QWidget(parent)
    , m_lineEdit(new QLineEdit(this))
    , m_editButton(new QPushButton(tr("Edit"), this))
    , m_saveButton(new QPushButton(tr("Save"), this))
    , m_cancelButton(new QPushButton(tr("Cancel"), this))
    , retrieve(rFn)
    , save(sFn)
    , m_currentMode(mode_t::display)
{
    setLayout(new QHBoxLayout(this));

    layout()->addWidget(m_lineEdit);
    m_lineEdit->installEventFilter(this);
    connect(
        m_lineEdit
        , &QLineEdit::returnPressed
        , [this](void) { onLinedEditReturnPressed(); });

    layout()->addWidget(m_editButton);
    connect(
        m_editButton
        , &QPushButton::clicked
        , [this](void) { onEditButtonClicked(); });

    layout()->addWidget(m_saveButton);
    connect(
        m_saveButton
        , &QPushButton::clicked
        , [this](void) { onSaveButtonClicked(); });

    layout()->addWidget(m_cancelButton);
    connect(
        m_cancelButton
        , &QPushButton::clicked
        , [this](void) { onCancelButtonClicked(); });

    setMode(mode_t::display);

    updateFromField();
}   // end constructor

void TextFieldLineEdit::updateFromField(void)
{
    if (retrieve != nullptr) m_lineEdit->setText(retrieve());
}   // end updateFromField method

void TextFieldLineEdit::saveToField(void)
{
    if (save != nullptr) save(m_lineEdit->text());
}   // end saveToField method

void TextFieldLineEdit::setMode(mode_t m)
{
    switch (m)
    {
        case mode_t::display:
            m_editButton->show();
            m_saveButton->hide();
            m_cancelButton->hide();
            m_lineEdit->setReadOnly(true);
            m_lineEdit->setFrame(false);

            m_currentMode = mode_t::display;
            
            break;

        case mode_t::edit:
            if (canEdit())
            {
                m_editButton->hide();
                m_saveButton->show();
                m_cancelButton->show();
                m_lineEdit->setReadOnly(false);
                m_lineEdit->setFrame(true);

                m_currentMode = mode_t::edit;
            }
            else logging::logger().log(
                    logging::level_t::warning
                    , L"attempt to place a TextFieldLineEdit in editing mode "
                        "when editing is disabled");
            break;

        default:
            Error(tr("unrecognised operating mode for "
                "TextFieldLineEdit")).raise();
    }

}   // end setMode method

bool TextFieldLineEdit::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_lineEdit && event->type() == QEvent::MouseButtonDblClick)
    {
        onLineEditDoubleClicked();
    }
    else QWidget::eventFilter(obj, event);
}   // end eventFilter

void TextFieldLineEdit::onLineEditDoubleClicked(void)
{
    executeStartEditAction();
}   // end onLineEditDoubleClick method

void TextFieldLineEdit::onLinedEditReturnPressed(void)
{
    executeSaveEditAction();
}   // end onLinedEditReturnPressed method

void TextFieldLineEdit::onEditButtonClicked(void)
{
    executeStartEditAction();
}   // end onEditButtonClicked method

void TextFieldLineEdit::onSaveButtonClicked(void)
{
    executeSaveEditAction();
}   // end onSaveButtonClick method

void TextFieldLineEdit::onCancelButtonClicked(void)
{
    executeCancelEditAction();
}   // end onCancelButtonClick method

void TextFieldLineEdit::executeStartEditAction(void)
{
    ACTION_TRY
    {
        if (canEdit())
        {
            // Make sure our field content is up to date, then move into edit
            // mode
            updateFromField();
            setMode(mode_t::edit);
        }
    }
    ACTION_CATCH_DURING(tr("Begin Field Editing"));
}   // end executeStartEditAction method

void TextFieldLineEdit::executeCancelEditAction(void)
{
    ACTION_TRY
    {
        // Move into display mode without updating the document, and retrieve
        // the old value back.
        updateFromField();
        setMode(mode_t::display);
    }
    ACTION_CATCH_DURING(tr("Cancel Field Editing"));
}   // end executeCancelEditAction method

void TextFieldLineEdit::executeSaveEditAction(void)
{
    ACTION_TRY
    {
        // Save the data, and move back into display mode.
        saveToField();
        setMode(mode_t::display);
    }
    ACTION_CATCH_DURING(tr("Save Edited Field"));
}   // end executeSaveEditAction method
