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

    // Default constructor and virtual destructor - disable copy / move
    // semantics.
    DECLARE_DEFAULT_VIRTUAL_LIFE_CYCLE(datastore)
    DECLARE_NO_MOVE_AND_COPY_SEMANTICS(datastore)

    /**
     * \brief Determine whether or not the storage is ready for use
     */
    virtual bool is_ready(void) const = 0;

};  // end datastore class

/**
 * \brief A unique pointer to a datastore object
 */
using datastore_upr = std::unique_ptr<datastore>;

}}  // end api namespace

#endif
