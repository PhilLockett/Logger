/**
 * @file    Log_c.cpp
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
 * Logging Implementation.
 */

#include <stdio.h>

#include "Log_c.h"


/**
 * The only Constructor.
 *
 * @param  ModuleName - String identifier used in log messages.
 * @param  level - the logging level.
 */
Log_c::Log_c(const char* moduleName, int level) : logLevel(level)
{
    // Reference to Logging Singleton.
    loggerRef = Logger_c::getInstance();

    // Pad or truncate moduleName.
    sprintf(module, "%-*.*s", MODULE_NAME_LEN, MODULE_NAME_LEN, moduleName);
}


/**
 * Check logging level and send the line to the log if sufficiently impotant.
 *
 * @param  level - the logging level for this log entry.
 * @param  format - the log entry format string, followed by parameters.
 * @return negative error value or 0 if no errors.
 */
int Log_c::printf(int level, const char* format, ...)
{
    if (level <= logLevel)
    {
        va_list argptr;
        va_start(argptr, format);
        char qualifier[80];    // ugly

        //- Use the module name and logging level as qualifier.
        sprintf(qualifier, "%s L%d -", module, level);

        int ret = loggerRef->log(qualifier, format, argptr);

        va_end(argptr);

        return ret;
    }

    return -1;
}


#if defined(UNIT_TESTING)

///////////////////////////////////////////////////////////////////////////////
//
// Test code for Logging Singleton.
//    Test two log streams with logging messages of decreasing importance.
//    Adjust the logging level of each stream
//    send the same messages again
//
// Build using:
//    g++ -c -o Logger_c.o Logger_c.cpp
//    g++ -c -o Log_c.o Log_c.cpp
//    g++ -o test Logger_c.o Log_c.o 
//
// Test using:
//    ./test
//
// Check results in '/logs' directory.
//
static Log_c log(__FILE__, 3);     // Only log serious messages.

int main(void)
{
    int ret = 0;

//- Test a second logger. This is for illustration and usually not necessary.
    Log_c bob("Bob", 6);            // Make Bob verbose.

//- Test both log streams with logging messages of decreasing importance.
    log.printf(1, "Logging level set to %d", log.getLogLevel());
    log.printf(3, "Logging level set to %d", log.getLogLevel());
    log.printf(5, "Logging level set to %d", log.getLogLevel());
    log.printf(7, "Logging level set to %d", log.getLogLevel());
    bob.printf(1, "Logging level set to %d", bob.getLogLevel());
    bob.printf(3, "Logging level set to %d", bob.getLogLevel());
    bob.printf(5, "Logging level set to %d", bob.getLogLevel());
    bob.printf(7, "Logging level set to %d", bob.getLogLevel());

//- For testing purposes adjust the logging level of each stream. Typically
//  you would not change the log levels after initialisation.
    log.setLogLevel(5);
    bob.setLogLevel(4);

//- Now test both log streams again with the same logging messages.
    log.printf(1, "Logging level set to %d", log.getLogLevel());
    log.printf(3, "Logging level set to %d", log.getLogLevel());
    log.printf(5, "Logging level set to %d", log.getLogLevel());
    log.printf(7, "Logging level set to %d", log.getLogLevel());
    bob.printf(1, "Logging level set to %d", bob.getLogLevel());
    bob.printf(3, "Logging level set to %d", bob.getLogLevel());
    bob.printf(5, "Logging level set to %d", bob.getLogLevel());
    bob.printf(7, "Logging level set to %d", bob.getLogLevel());

    log.flush();
    bob.flush();

    return ret;
}

#endif // defined(UNIT_TESTING)

