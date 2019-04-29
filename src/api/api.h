/**
 * \file api.h
 * Header file for a stub api
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <string>

#include "account.h"
#include "currency.h"
#include "datastore.h"
#include "date.h"
#include "error.h"

#ifndef _api_api_h_included
#define _api_api_h_included

/**
 * \brief Declarations related to core nCountr functionality
 */
namespace ncountr {

/**
 * Declarations for the Application Programming Interface to the core logic
 */
namespace api {

/**
 * \brief Retrieve the version string for the library
 */
extern std::string version(void);

/**
 * \brief Retrieve the version (wide) string for the library
 */
extern std::wstring wversion(void);

}}   // end ncountr::api namespace

#endif

/**
 * \page api_arch API-based Architecture
 * 
 * *nCounter* has a three-layer architecture:
 * 
 * 1. The *Presentation Layer* includes the GUI and all functionality for
 *    interacting with the User. The code for this functionality is contained
 *    in the `src/gui` subdirectory
 * 
 * 2. The *API Layer* defines the business logic of the application in
 *    abstract terms. The code for the API is in the `src/api` folder.
 * 
 * 3. The *Storage Layer* provides concrete implementations of the
 *    abstractions defined in the API Layer, mainly comprising an
 *    implementation of the `api::datastore` type. The initial version of the
 *    application provides a Storage Layer based on SQLite database files,
 *    but other implementations are planned for future iterations.
 * 
 * \todo Note on string handling, and switching between `QString` and
 * `std::wstring`.
 */

/**
 * \page naming_conventions Naming Conventions
 * 
 * \todo Explain naming conventions, and where camel-case is taken from Qt,
 * and snake-case from the C++ standard
 */

/**
 * \page translation_support Support for Language Translation
 * 
 * \todo Explain about using Qt's language translation framework for all
 * User-facing strings, noting that log messages are always in English
 */