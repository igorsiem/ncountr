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

datastore::datastore(QString filePath) :
    base_t()
    , m_filePath(std::move(filePath))
    , m_db(boost::none)
{
    // Use the file path as a 
    m_db = QSqlDatabase::addDatabase("QSQLITE", m_filePath);
    m_db->setDatabaseName(filePath);

    if (m_db->open())
        logger().log(level_t::debug, L"opened database \"{}\""_format(
            m_filePath.toStdWString()));
    else
    {
        m_db = boost::none;
        logger().log(level_t::error, L"could not open database "
            "\"{}\""_format(m_filePath.toStdWString()));
    }
}   // end constructor

datastore::~datastore(void)
{
    m_db = boost::none;
    QSqlDatabase::removeDatabase(m_filePath);

    logger().log(level_t::debug, L"closed database and connection");
}   // end destructor

}}}  // end ncountr::datastores::sqlite namespace
