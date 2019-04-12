/**
 * \file api/error.h
 * Declare the `apu::error` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <stdexcept>
#include <qlib/qlib.h>

#ifndef _api_error_h_included
#define _api_error_h_included

namespace ncountr { namespace api {

/**
 * \brief Base class for all error exceptions in the API
 */
class error : public std::exception
{

    public:

    /**
     * \brief Constructor, initialising the User message
     * 
     * \param msg Human-readable error message
     */
    explicit error(std::string msg) :
        std::exception()
        , m_message(std::move(msg)) {}

    /**
     * \brief Retrieve the human-readable error message
     */
    virtual const char* what() const noexcept override
        { return m_message.c_str(); }

    DECLARE_DEFAULT_MOVE_AND_COPY_SEMANTICS(error)

    protected:

    std::string m_message;  ///< Human-readable description of the error

};  // end error class

}}  // end ncountr::api namespace

#endif
