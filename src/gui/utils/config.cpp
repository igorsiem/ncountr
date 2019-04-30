/**
 * \file config.h
 * Declare config-related functionality, based on Boost.ProgramOptions
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <iostream>
#include <stdexcept>

#include "config.h"

void parse_command_line(
        int argc
        , char* argv[]
        , bst::po::variables_map& vm
        , bool& help_signalled)
{
    bst::po::options_description desc("Allowed Options");

    // Set up the options
    desc.add_options()
        ("help,h", "display help")
        (
            "logging-level,l"
            , bst::po::value<std::string>()->default_value("INF")->notifier(
                [](std::string l)
                {
                    if ((l != "ERR") && (l != "WAR") && (l != "INF") &&
                            (l != "DEB"))
                    {
                        std::wcerr << L"[ERR] logging level must be one of "
                            "\"ERR\", \"WAR\", \"INF\" or \"DEB\"" <<
                            std::endl;

                        throw std::runtime_error("configuration error");
                    }
                })
            , "logging level [ERR|WAR|INF|DEB]"
        )
        ;

        // Parse the options, and run notifiers
        bst::po::store(bst::po::parse_command_line(argc, argv, desc), vm);
        bst::po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            help_signalled = true;
        }
        else help_signalled = false;
}   // end parse_command_line function