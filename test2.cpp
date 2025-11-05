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

static const int MAJOR   = 2;

// Only log serious messages unless it is overridden.
static Log_c log(__FILE__, MAJOR);

/**
 * Dummy remote function to test Logging Implementation.
 *
 * @param  level - adjust the logging level for mod.
 * @return error value or 0 if no errors.
 */
int remoteFunction(int loggingLevel)
{
//- For testing purposes change logging level. Typically you would not change
//  the log levels after initialisation.
    if (loggingLevel != MAJOR)
        log.setLogLevel(loggingLevel);

    for (int logEntryLevel = 1; logEntryLevel < Log_c::MAX_LOG_LEVEL; ++logEntryLevel)
        log.logf(logEntryLevel, "Logging level set to %d for remoteFunction(%d).", log.getLogLevel(), loggingLevel);

    return 0;
}

