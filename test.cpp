/**
 * @file    test.cpp
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
 * Unit test code for the Logging Implementation.
 *
 * Build using:
 *    g++ -c -o Log_c.o Log_c.cpp
 *    g++ -c -o test.o test.cpp
 *    g++ -c -o test2.o test2.cpp
 *    g++ -o test test.o test2.o Log_c.o 
 *
 * Test using:
 *    ./test
 *
 */

#include <iostream>

#include "Log_c.h"

#define ERROR 3

static Log_c log(__FILE__, ERROR);     // Only log serious messages.

extern int remoteFunction(int level);

/**
 * Dummy local function to test Logging Implementation. Test both the module's
 * log and a log stream local to this function.
 *
 * @param  level - set the logging level for bob.
 * @return error value or 0 if no errors.
 */
void localFunction(const int level)
{
//- Debug tracking.
    log.printf(7, "test::func() called.");

//- Test a second logger. This is for illustration and usually not necessary.
    Log_c bob("Bob", level);
	std::cout << "Logging level for bob(\"Bob\") set to " << level << std::endl;

//- Do some work.
    log.printf(1, "Logging level set to %d.", log.getLogLevel());
    bob.printf(1, "Logging level set to %d.", bob.getLogLevel());
    log.printf(3, "Logging level set to %d.", log.getLogLevel());
    bob.printf(3, "Logging level set to %d.", bob.getLogLevel());
    log.printf(5, "Logging level set to %d.", log.getLogLevel());
    bob.printf(5, "Logging level set to %d.", bob.getLogLevel());
    log.printf(7, "Logging level set to %d.", log.getLogLevel());
    bob.printf(7, "Logging level set to %d.", bob.getLogLevel());

    bob.flush();
}

/**
 * Test system entry point.
 *
 * @param  argc - command line argument count.
 * @param  argv - command line argument vector.
 * @return error value or 0 if no errors.
 */
int main(int argc, char *argv[])
{
//- Overide default log level from command line. Set the log level to a
//  larger value for more verbose log entries.
    if (argc > 1)
    {
        const int lvl = atoi(argv[1]);
        if ((Log_c::isLogLevelValid(lvl)) && (lvl != ERROR))
        {
            log.setLogLevel(lvl);
            std::cout << "Logging level for log(\"" __FILE__ "\") changed to " << lvl << std::endl;
        }
    }

//- Debug tracking.
    log.printf(7, "main() called.");

//- Do some work.
    localFunction(6);
    remoteFunction(2);   // Call test module with same initial log level.

//- For testing purposes change logging level. Typically you would not change
//  the log levels after initialisation.
    log.setLogLevel(5);
    std::cout << "Logging level for log(\"" __FILE__ "\") changed to " << 5 << std::endl;

//- Do some more work.
    localFunction(4);
    remoteFunction(7);   // Call test module with verbose log level.

    log.flush();

    std::cout << "Check results in " << log.getLogFilePath() << std::endl;

    return 0;
}

