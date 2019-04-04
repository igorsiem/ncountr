/**
 * \file logging.h
 * Declare logging-related functionality, based on qLib and the config
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include "config.h"

#ifndef _gui_logging_h_included
#define _gui_logging_h_included

namespace logging {

/**
 * \brief Logging level enumeration
 */
using level_t = qlib::logger::level_t;

/**
 * \brief Retrieve the logging interface
 * 
 * This is a convenience method for retrieve a reference to the qLib logging
 * singleton.
 * 
 * \return A reference to the single qlib::logger object
 */
inline qlib::logger& logger(void) { return qlib::logger::instance(); }

/**
 * \brief Set up system logging in accordance with command-line options
 * 
 * \param vm The configuration variables map parsed from the command-line;
 * the `logging-level` item is used to determine logging configuration
 */
extern void setup(const bst::po::variables_map& vm);

}   // end log namespace

#endif
