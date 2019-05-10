/**
 * \file api/datastore.h
 * Declare the `datastore` interface
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <memory>
#include <qlib/qlib.h>

#include "account.h"
#include "error.h"

#ifndef _api_datastore_h_included
#define _api_datastore_h_included

namespace ncountr { namespace api {

/**
 * \brief An abstract interface for storing all application data for a single
 * set of accounts
 */
class datastore
{

    // --- External Interface ---

    public:

    /**
     * \brief Error exception for signalling issues related to the Datastore
     */
    class error : public api::error
    {

        public:

        /**
         * \brief Constructor - setting human-readable error message
         * 
         * \param msg The human-readable error message
         */
        explicit error(std::string msg) : api::error(msg) {}

        DECLARE_DEFAULT_VIRTUAL_DESTRUCTOR(error)
        DECLARE_DEFAULT_MOVE_AND_COPY_SEMANTICS(error)
    };  // end error class

    // Default constructor and virtual destructor - disable copy / move
    // semantics.
    DECLARE_DEFAULT_VIRTUAL_LIFE_CYCLE(datastore)
    DECLARE_NO_MOVE_AND_COPY_SEMANTICS(datastore)

    /**
     * \brief Determine whether or not the storage is ready for use
     */
    virtual bool is_ready(void) const = 0;

    // -- Document Information --

    /**
     * \brief Retrieve the Document Name
     */
    virtual std::wstring name(void) const = 0;

    /**
     *  \brief Set the Document Name
     */
    virtual void set_name(std::wstring n) = 0;

    /**
     * \brief Retrieve the Document Description
     */
    virtual std::wstring description(void) const = 0;

    /**
     * \brief Set the Document Description string
     */
    virtual void set_description(std::wstring d) = 0;

    // -- Account-management --

    virtual account_spr create_account(
        std::wstring full_path
        , account_spr parent
        , std::wstring description
        , date opening_date
        , currency_t opening_balance) = 0;

    virtual account_spr create_account(
        std::wstring full_path
        , account_spr parent
        , std::wstring description) = 0;

    virtual account_spr find_account(std::wstring full_path) = 0;

};  // end datastore class

/**
 * \brief A unique pointer to a datastore object
 */
using datastore_upr = std::unique_ptr<datastore>;

}}  // end api namespace

#endif

/**
 * \page datastores Datastores
 * 
 * *Datastore* is a generic term for a class that stores related account
 * and transation data (roughly analagous to a 'set of books'). The
 * `api::datastore` class declares an abstract interface for this idea,
 * and it is implemented in the `datastores::sqlite::datastore` class.
 * Of course, this hierarchy can be extended so that data may be stored
 * in other ways.
 * 
 * \todo Expand page on Datastores
 */
