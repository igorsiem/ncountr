/**
 * \file error_handling.h
 * Convenient macros for top-level error handling
 * 
 * \author Igor Siemienowicz
 * 
 * \copyright Copyright Igor Siemienowicz 2018 Distributed under the Boost
 * Software License, Version 1.0. (See accompanying file ../LICENSE_1_0.txt
 * or copy at https://www.boost.org/LICENSE_1_0.txt
 */

#include <QException>
#include <QMessageBox>
#include "error.h"
#include "logging.h"

#ifndef _gui_error_handling_h_included
/// \cond
#define _gui_error_handling_h_included
/// \endcond

/**
 * \brief Beginning of a top-level TRY/CATCH block for event handlers
 */
#define ACTION_TRY try

/**
 * \brief The end of the top-level try / catch block
 * 
 * This macro catches all exceptions, extracting any information that is
 * available, and displays an appropriate error MessageBox, with the given
 * action description.
 * 
 * \param actionDesc A string describing the action that was being attempted
 * when the error occurred; this is often the same as the menu text
 */
#define ACTION_CATCH_DURING( actionDesc ) \
    catch (const ::Error& err) \
    { \
        logging::logger().log(logging::level_t::error, \
            (QString(actionDesc) + " - " + err.message()).toStdWString()); \
        QMessageBox::critical( \
            this, \
            actionDesc, \
            err.message()); \
    } \
    catch (const QException&) \
    { \
        QString msg("an unrecognised Qt exception was encountered"); \
        logging::logger().log(logging::level_t::error, \
            (QString(actionDesc) + " - " + msg).toStdWString()); \
        QMessageBox::critical( \
            this, \
            actionDesc, \
            msg); \
    } \
    catch (const std::exception& err) \
    { \
        QString msg(err.what()); \
        logging::logger().log(logging::level_t::error, \
            (QString(actionDesc) + " - " + msg).toStdWString()); \
        QMessageBox::critical( \
            this, \
            actionDesc, \
            msg); \
    } \
    catch (...) \
    { \
        QString msg(tr("An unrecognised error condition has arised. The " \
            "operation could not be completed.")); \
        logging::logger().log(logging::level_t::error, \
            (QString(actionDesc) + " - " + msg).toStdWString()); \
        QMessageBox::critical( \
            this, \
            actionDesc, \
            msg); \
    }

#endif
