/**
 * \file db_utils.cpp
 * Implement low-level database utilities for the Sqlite Datastore
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include "db_utils.h"

namespace ncountr { namespace datastores { namespace sqlite {

void prepareAndExecute(
        QSqlQuery& query
        , const QString& queryString
        , const std::map<QString, QVariant>& bindings)
{

    logger().log(
        level_t::debug
        , L"query: {}"_format(queryString.toStdWString()));
    
    if (!query.prepare(queryString))
        throw DbUtilsError(DB_UTILS_TR("query preparation error: ") +
            query.lastError().text());
 
    for (auto bv : bindings) query.bindValue(bv.first, bv.second);

    if (!query.exec())
        throw DbUtilsError(DB_UTILS_TR("query execution error: ") +
            query.lastError().text());

}   // end prepareAndExecute

}}} // end ncountr::datastores::sqlite namespace
