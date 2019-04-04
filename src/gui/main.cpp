/**
 * \file main.cpp
 * Entry point for the GUI executable
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <QApplication>

#include <fmt/format.h>
using namespace fmt::literals;

#include <qlib/qlib.h>

#include <api/api.h>

#include "config.h"
#include "logging.h"
#include "mainwindow.h"

/**
 * \brief Entry point for the GUI executable
 * 
 * This function instantiates the main window and application classes, shows
 * the window, and starts the application loop
 * 
 * \param argc The number of command-line arguments
 * 
 * \param argv The array of command-line argument
 * 
 * \return The return code from `QApplication::exec` (non-zero on error)
 */
int main(int argc, char *argv[])
{

    int result = 0;

    try
    {

        // Config and logging
        bst::po::variables_map vm;
        bool help_signalled = false;
        parse_command_line(argc, argv, vm, help_signalled);
        logging::setup(vm);

        // If we didn't ask for help, run the application
        if (!help_signalled)
        {

            logging::logger().log(
                logging::level_t::info
                , L"application starting");

            logging::logger().log(
                logging::level_t::info
                , L"API version {}"_format(api::wversion()));

            QApplication a(argc, argv);
            MainWindow w;
            w.show();

            result = a.exec();

        }   // end if we want to run the application

    }   // end try block
    catch (const std::exception& error)
    {
        std::cerr << "[ERR] " << error.what() << std::endl;
        result = -1;
    }
    catch (...)
    {
        std::cerr << "[ERR] unrecognised exception" << std::endl;    }

    logging::logger().log(
        logging::level_t::debug
        , L"application exiting with result {}"_format(result));

    return result;

}   // end main function
