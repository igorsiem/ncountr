/**
 * \file api.h
 * Implementation for a stub api
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <qlib/qlib.h>
#include "api.h"

namespace api {

std::string version(void)
{
    return "0.0";
}

std::wstring wversion(void)
{
    return boost::lexical_cast<std::wstring>(version());
}

}   // end api namespace
