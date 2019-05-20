/**
 * \file gui/api.h
 * Namespace management declarations
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <api/api.h>
#include <api/sqlite_datastore/sqlite_datastore.h>

#ifndef _gui_api_h_included
#define _gui_api_h_included

namespace Api {

using namespace ncountr::api;

};  // end Api namespace

namespace Utils {

inline QDate toQDate(Api::date d)
    { return ncountr::datastores::sqlite::to_qdate(d); }

inline Api::date toApiDate(QDate d)
    { return ncountr::datastores::sqlite::to_api_date(d); }

}   // end Utils namespace

#endif
