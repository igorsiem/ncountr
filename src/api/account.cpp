/**
 * \file api/account.cpp
 * Implementation for the `account` interface
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <boost/algorithm/string.hpp>
#include "account.h"

namespace ncountr { namespace api {

const std::wstring account::account_path_separator = L"/";

bool account::valid_name(const std::wstring& n)
{
    return (n.find(account_path_separator) == std::wstring::npos);
}   // end valid_name method

std::vector<std::wstring> account::split_path(const std::wstring& p)
{
    std::vector<std::wstring> result;
    boost::split(
        result
        , p
        , boost::algorithm::is_any_of(account_path_separator));
    return result;
}   // end split_path method

std::wstring account::concatenate_path(const std::vector<std::wstring>& p)
{
    return boost::algorithm::join(p, account_path_separator);
}   // end concatenate_path method

///std::wstring account::full_path(void)
///{
///    if (parent_path().empty()) return name();
///    else return (parent_path() + account_path_separator + name());
///}   // end full_path

}}  //end ncountr::api namespace
