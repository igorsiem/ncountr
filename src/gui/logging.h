/**
 * \file gui/logging.h
 * Declare logging-related functionality, based on qLib and the config
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <qlib/qlib.h>
#include "config.h"

#ifndef _gui_logging_h_included
#define _gui_logging_h_included

namespace logging {

/**
 * \brief Logging level enumeration
 */
using level_t = qlib::logger::level_t;

const level_t debug = level_t::debug;   ///< Constant for debug logging
const level_t info = level_t::info; ///< Constant for info logging
const level_t warning = level_t::warning;   ///< Constant for warning logging
const level_t error = level_t::error;   ///< Constant for error logging

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

}   // end logging namespace

/**
 * \brief A convenience macro for logging a message at any level
 */
#define ENC_LOG( level, msg ) \
    ::logging::logger().log(level, msg )

/**
 * \brief A convenience macro for expressing the standard `__FUNCTION__`
 * macro as a std::wstring
 */
#define __WFUNCTION__ boost::lexical_cast<std::wstring>(__FUNCTION__)

/**
 * \brief Convenience macro for logging an error message
 */
#define ENC_LOG_ERROR( msg ) \
    ENC_LOG(logging::error, msg))

/**
 * \brief Convenience macro for logging a warning message
 */
#define ENC_LOG_WARNING( msg ) \
    ENC_LOG(logging::warning, msg))

/**
 * \brief Convenience macro for logging an info message
 */
#define ENC_LOG_INFO( msg ) \
    ENC_LOG(logging::info, msg))

/**
 * \brief Convenience macro for logging a debug message
 * 
 * Note that the standard __FUNCTION__ string is prepended to the message
 */
#define ENC_LOG_DEBUG( msg ) \
    ENC_LOG(logging::debug, L"{} - {}"_format(__WFUNCTION__, msg))

#endif
