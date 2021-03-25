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
#include <fstream>
#include <filesystem>

#include "Log_c.h"

#include "unittest.h"

/**
 * @section basic utility code..
 */

static void deleteDirectory(std::string path)
{
    std::filesystem::remove_all(path); // Delete directory and contents.
}

static std::string getFileName(std::string path)
{
    time_t now = time(NULL);
    struct tm tim = *localtime(&now);
    char FileName[80];

    sprintf(FileName, "%s/log-%04d-%02d-%02d.txt", path.c_str(), tim.tm_year + 1900, tim.tm_mon + 1, tim.tm_mday);

    return std::string(FileName);
}

static int getFileLength(std::string fileName)
{
    std::ifstream infile(fileName, std::ifstream::in);
    int count = 0;
    std::string line;

    while(!infile.eof())
    {
        getline(infile, line);
        count++;
    }

    return count;
}


/**
 * @section test logging code.
 */

#define MAJOR    2
#define ERROR    3
#define WARNING  4
#define NOTICE   5
#define INFO     6
#define DEBUG    7
#define VERBOSE  8

static Log_c log(__FILE__, ERROR);     // Only log serious messages.

extern int remoteFunction(int level = MAJOR);


UNIT_TEST(test0, "Test sending log entries using global log reference.")

    for (int loggingLevel = 1; loggingLevel < Log_c::MAX_LOG_LEVEL; ++loggingLevel)
        log.printf(loggingLevel, "Logging level set to %d.", log.getLogLevel());

NEXT_CASE(test1, "Test sending log entries using local log reference.")

    Log_c bob("Bob", DEBUG);	// Make Bob chatty.
    for (int loggingLevel = 1; loggingLevel < Log_c::MAX_LOG_LEVEL; ++loggingLevel)
        bob.printf(loggingLevel, "Logging level set to %d.", bob.getLogLevel());

NEXT_CASE(test2, "Test sending log entries from remote code.")

    remoteFunction();   // Call test module.

NEXT_CASE(test3, "Test changing logging level.")

    log.setLogLevel(INFO);
    for (int loggingLevel = 1; loggingLevel < Log_c::MAX_LOG_LEVEL; ++loggingLevel)
        log.printf(loggingLevel, "Logging level set to %d.", log.getLogLevel());

NEXT_CASE(test4, "Test interleaving log entries.")

    for (int loggingLevel = 1; loggingLevel < Log_c::MAX_LOG_LEVEL; ++loggingLevel)
    {
        log.printf(loggingLevel, "Logging level set to %d.", log.getLogLevel());
        bob.printf(loggingLevel, "Logging level set to %d.", bob.getLogLevel());
    }

NEXT_CASE(test2, "Test sending verbose log entries from remote code.")

    remoteFunction(VERBOSE);   // Call test module.

END_TEST

int runTests(void)
{
//- Initialize test set up.
    std::filesystem::remove_all("logs"); // Delete directory and contents.
    log.setLogFilePath("logs");
    log.enableTimestamp(false);

    std::cout << "Executing all tests.\n";
//	  VERBOSE_OFF

    RUN_TEST(test0)

    log.flush();

    std::cout << "Check results in " << log.getLogFilePath() << '\n';

    const int err = ERROR_COUNT;
    if (err)
        std::cerr << err << " ERROR(S) encountered!.\n";
    else
        std::cout << "All tests passed.\n";

    return err;
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
    return runTests();
}

