/**
 * \Implement datastore.cpp
 * Declare the `sqlite::datastore` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <fmt/format.h>
using namespace fmt::literals;

#include <qlib/qlib.h>
#include "datastore.h"

namespace ncountr { namespace datastores { namespace sqlite {

using level_t = qlib::logger::level_t;
inline qlib::logger& logger(void) { return qlib::logger::instance(); }

datastore::datastore(QString filePath) :
    base_t()
    , m_db(boost::none)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db->setDatabaseName(filePath);

    if (m_db->open())
        logger().log(level_t::info, L"opened database \"{}\""_format(
            filePath.toStdWString()));
    else
    {
        m_db = boost::none;
        logger().log(level_t::error, L"could not open database "
            "\"{}\""_format(filePath.toStdWString()));
    }
}   // end constructor

}}}  // end ncountr::datastores::sqlite namespace
