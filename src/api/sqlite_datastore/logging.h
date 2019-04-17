/**
 * \file sqlite_datastore/logging.h
 * Some basic logging (re)declarations for use within the Sqlite Database
 * library
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include "../api.h"

#ifndef _sqlite_logging_h_included
#define _sqlite_logging_h_included

namespace ncountr { namespace datastores { namespace sqlite {

/**
 * \brief Convenient alias for the logger level enumerator type
 * 
 * \todo Move this to a single logging header for the library
 */
using level_t = qlib::logger::level_t;

/**
 * \brief Convenience method for retrieving a reference to the single logger
 * instance
 * 
 * \return A reference to the logging singleton
 * 
 * \todo Move this to a single logging header for the library
 */
inline qlib::logger& logger(void) { return qlib::logger::instance(); }

}}} // end ncountr::datastores::sqlite namespace

#endif