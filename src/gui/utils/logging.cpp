/**
 * \file logging.cpp
 * Implement logging-related functionality, based on qLib and the config
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <QDebug>
#include <qlib/qlib.h>
#include "logging.h"

namespace logging {

void setup(const bst::po::variables_map& vm)
{
    // Handle in descending order of criticality - e.g. if you ask for INF
    // messages, you get ERR and WAR as well...
    qlib::logger::instance().add(
        { level_t::error }
        , [](level_t, const std::wstring& msg)
            {
                qCritical() << qPrintable(QString::fromStdWString(msg));
            });

    if (vm["logging-level"].as<std::string>() == "ERR") return;

    qlib::logger::instance().add(
        { level_t::warning }
        , [](level_t, const std::wstring& msg)
            {
                qWarning() << qPrintable(QString::fromStdWString(msg));
            });

    if (vm["logging-level"].as<std::string>() == "WAR") return;

    qlib::logger::instance().add(
        { level_t::info }
        , [](level_t, const std::wstring& msg)
            {
                qInfo() << qPrintable(QString::fromStdWString(msg));
            });

    if (vm["logging-level"].as<std::string>() == "INF") return;

    qlib::logger::instance().add(
        { level_t::debug }
        , [](level_t, const std::wstring& msg)
            {
                qDebug() << qPrintable(QString::fromStdWString(msg));
            });

}   // end setup_logging function

}   // end log namespace
