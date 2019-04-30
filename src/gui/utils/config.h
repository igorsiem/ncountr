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

#include <boost/program_options.hpp>

#ifndef _gui_config_h_included
#define _gui_config_h_included

/**
 * \brief Alias for the `boost` namespace
 */
namespace bst {

    using namespace boost;

    namespace po = program_options; ///< Alias for `boost::program_options`

}   // end bst namespace

/**
 * \brief Parse the command-line to configure the application
 * 
 * This function sets up the Boost Program Options command-line parsiing
 * functionality with an `options_description` object, and uses its to parse
 * the command-line options passed to the `main` function, propulating a
 * Boost `variables_map` that can be subsequently passed around the
 * application to configure it.
 * 
 * \param argc The number of the command-line arguments
 * 
 * \param argv The vector of command-line argument strings
 * 
 * \param vm The variables map objects that is populated with config
 * information
 * 
 * \param help_signalled Whether or not the 'help' option was specified; in
 * this case, the options description is printed to stdout, and this flag is
 * set to `true`; in this case, the main application should simply exit
 * without further action
 */
extern void parse_command_line(
    int argc
    , char* argv[]
    , bst::po::variables_map& vm
    , bool& help_signalled);

#endif
