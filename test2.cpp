/**
 * @file    test2.cpp
 * @author  Phil Lockett <phillockett65@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * Dummy module to test Logging Implementation. Called from test.cpp.
 */

#include <iostream>

#include "Log_c.h"

#define MAJOR 2

static Log_c log(__FILE__, MAJOR);     // Only log serious messages.

/**
 * Dummy remote function to test Logging Implementation.
 *
 * @param  level - adjust the logging level for mod.
 * @return error value or 0 if no errors.
 */
int remoteFunction(int level)
{
//- For testing purposes change logging level. Typically you would not change
//  the log levels after initialisation.
    if (level != MAJOR)
    {
        log.setLogLevel(level);
        std::cout << "Logging level for log(\"" __FILE__ "\") changed to " << level << '\n';
    }

//- Debug tracking.
    log.printf(7, "mod::module() called.");

//- Do some work.
    log.printf(1, "Logging level set to %d.", log.getLogLevel());
    log.printf(3, "Logging level set to %d.", log.getLogLevel());
    log.printf(5, "Logging level set to %d.", log.getLogLevel());
    log.printf(7, "Logging level set to %d.", log.getLogLevel());

    return 0;
}

