/**
 * \file error.h
 * Exception base-class, just for use in the GUI
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <QException>

#ifndef _gui_error_h_included
#define _gui_error_h_included

/**
 * \brief An exception base-class for use as an error base-class at the GUI
 * level
 */
class Error : public QException
{
    public:

    /**
     * \brief Constructor, initialising the error message
     * 
     * \param msg Human-readable error message
     */
    explicit Error(QString msg) : m_message(std::move(msg)) {}

    /// \cond
    DECLARE_DEFAULT_VIRTUAL_DESTRUCTOR(Error);
    /// \endcond

    /**
     * \brief Throw the error object as an exception
     */
    virtual void raise(void) const override { throw *this; }

    /**
     * \brief Create a cloned copy of the Error object
     * 
     * \return A pointer to a new copy of self, created using `new`
     */
    virtual Error* clone(void) const override
        { return new Error(*this); }

    /**
     * \brief Retrieve the human-readable message for the error
     */
    QString message(void)  const { return m_message; }

    protected:

    QString m_message;  ///< Human-readable message for the error

};  // end Error class

#endif
