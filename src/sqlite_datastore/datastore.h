/**
 * \file sqlite_datastore/datastore.h
 * Declare the `sqlite::datastore` class
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <boost/optional.hpp>
#include <QSqlDatabase>
#include "../api/api.h"

#ifndef _sqlite_datastore_h_included
#define _sqlite_datastore_h_included

namespace ncountr { namespace datastores { namespace sqlite {

/**
 * \brief SQLite implementation of the `api::datastore` interface
 */
class datastore : public api::datastore
{

    // --- External Definitions ---

    public:

    using base_t = api::datastore;  ///< Our base class

    /**
     * \brief Constructor - attempts to open the database
     * 
     * If opening the database is succcessful, `is_ready` will return `true`.
     */
    explicit datastore(QString filePath);

    /**
     * \brief Destructor - shuts down the database connection
     */
    virtual ~datastore(void);

    // Default virtual destructor, and disable copy / move semantics
    DECLARE_NO_MOVE_AND_COPY_SEMANTICS(datastore)

    /**
     * \brief Determine whether or not the storage is ready for use
     * 
     * For a Sqlite Datastore, this means checking to see whether the
     * database object is present.
     */
    virtual bool is_ready(void) const
    {
        return (m_db != boost::none);
    }   // end is_ready

    // --- Internal Attributes ---

    private:

    QString m_filePath;                 ///< Path of the database file

    boost::optional<QSqlDatabase> m_db; ///< Database connection

};  // end datastore class

}}}  // end ncountr::datastores::sqlite namespace

#endif
