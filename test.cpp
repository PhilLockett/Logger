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
 *    g++ -std=c++20 -c -o test.o test.cpp
 *    g++ -std=c++20 -c -o test2.o test2.cpp
 *    g++ -std=c++20 -c -o Log_c.o Log_c.cpp
 *    g++ -std=c++20 -c -o unittest.o unittest.cpp
 *    g++ -std=c++20 -o test test.o test2.o Log_c.o unittest.o
 *
 * Test using:
 *    ./test
 *
 */

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

#include "Log_c.h"

#include "unittest.h"

/**
 * @section basic utility code.
 */

static void deleteDirectory(const std::string & path)
{
    std::filesystem::remove_all(path); // Delete directory and contents.
}

static int getFileLength(std::string fileName)
{
    std::ifstream infile(fileName, std::ifstream::in);
    int count = 0;
    std::string line;

    while(!infile.eof())
    {
        getline(infile, line);
        if (!infile.eof() && line.length())
            count++;
    }

    return count;
}

static bool fileToVector(std::vector<std::string> & ret, std::string fileName)
{
//    std::cout << "fileToVector " << fileName << '\n';
    std::ifstream infile(fileName, std::ifstream::in);
    if (infile.eof())
        return false;

    int count = 0;
    std::string line;

    while (!infile.eof())
    {
        getline(infile, line);
        if (!infile.eof() && line.length())
            ret.push_back(std::move(line));
    }

    return true;
}


/**
 * @section test logging code.
 */

enum LogLevel {
    CRITICAL = 1,
    MAJOR, ERROR, WARNING, NOTICE, INFO, DEBUG, VERBOSE,
    MAX = Log_c::MAX_LOG_LEVEL
};

static Log_c log(__FILE__, ERROR);     // Only log serious messages.

extern int remoteFunction(int level = MAJOR);


UNIT_TEST(test0, "Test sending log entries using global log reference.")

//- Initialize test set up.
    const std::string path = "logs";
    deleteDirectory(path);
    log.setLogFilePath(path);
    log.enableTimestamp(false);
    const std::string currentLogFileName = log.getFullLogFileName();

    std::vector<std::string> comp;
    REQUIRE(fileToVector(comp, "expected-log.txt") == true)

    for (int loggingLevel = CRITICAL; loggingLevel < MAX; ++loggingLevel)
        log.printf(loggingLevel, "Logging level set to %d.", log.getLogLevel());

{
    log.flush();
    std::vector<std::string> entries;
    REQUIRE(fileToVector(entries, currentLogFileName) == true)
    REQUIRE(entries.size() == 3)
    REQUIRE(std::equal(entries.begin(), entries.end(), comp.begin()))
}

NEXT_CASE(test1, "Test sending log entries using local log reference.")

    Log_c bob("Bob", DEBUG);    // Make Bob chatty.
    for (int loggingLevel = CRITICAL; loggingLevel < MAX; ++loggingLevel)
        bob.printf(loggingLevel, "Logging level set to %d.", bob.getLogLevel());

{
    bob.flush();
    std::vector<std::string> entries;
    REQUIRE(fileToVector(entries, currentLogFileName) == true)
    REQUIRE(entries.size() == 10)
    REQUIRE(std::equal(entries.begin(), entries.end(), comp.begin()))
}

NEXT_CASE(test2, "Test sending log entries from remote code.")

    remoteFunction();   // Call test module.

{
    log.flush();
    std::vector<std::string> entries;
    REQUIRE(fileToVector(entries, currentLogFileName) == true)
    REQUIRE(entries.size() == 12)
    REQUIRE(std::equal(entries.begin(), entries.end(), comp.begin()))
}

NEXT_CASE(test3, "Test changing logging level.")

    log.setLogLevel(INFO);
    for (int loggingLevel = 1; loggingLevel < Log_c::MAX_LOG_LEVEL; ++loggingLevel)
        log.printf(loggingLevel, "Logging level set to %d.", log.getLogLevel());

{
    log.flush();
    std::vector<std::string> entries;
    REQUIRE(fileToVector(entries, currentLogFileName) == true)
    REQUIRE(entries.size() == 18)
    REQUIRE(std::equal(entries.begin(), entries.end(), comp.begin()))
}

NEXT_CASE(test4, "Test interleaving log entries.")

    for (int loggingLevel = 1; loggingLevel < Log_c::MAX_LOG_LEVEL; ++loggingLevel)
    {
        log.printf(loggingLevel, "Logging level set to %d.", log.getLogLevel());
        bob.printf(loggingLevel, "Logging level set to %d.", bob.getLogLevel());
    }

{
    log.flush();
    std::vector<std::string> entries;
    REQUIRE(fileToVector(entries, currentLogFileName) == true)
    REQUIRE(entries.size() == 31)
    REQUIRE(std::equal(entries.begin(), entries.end(), comp.begin()))
}

NEXT_CASE(test5, "Test sending verbose log entries from remote code.")

    remoteFunction(VERBOSE);   // Call test module.

NEXT_CASE(test6, "Validate generated log file.")

    log.flush();

    std::vector<std::string> entries;
    REQUIRE(fileToVector(entries, currentLogFileName) == true)
    REQUIRE(entries.size() == 39)
    REQUIRE(entries == comp)

END_TEST


/**
 * @section test sending a large number of log entries code.
 */

UNIT_TEST(test8, "Test sending a large number of log entries.")

//- Initialize test set up.
    const std::string path = "speed";
    deleteDirectory(path);
    log.setLogFilePath(path);
    log.enableTimestamp(true);
    log.setLogLevel(NOTICE);

    const int ENTRIES = 100000;
    for (int i = 0; i < ENTRIES; ++i)
    {
        for (int loggingLevel = CRITICAL; loggingLevel < MAX; ++loggingLevel)
            log.printf(loggingLevel, "Logging level set to %d - adding log entry %d", log.getLogLevel(), i);
    }

    std::string currentLogFileName = log.getFullLogFileName();
    log.flush();
    REQUIRE(getFileLength(currentLogFileName) == (ENTRIES*NOTICE))

END_TEST


int runTests(void)
{
    std::cout << "Executing all tests.\n";
//    VERBOSE_OFF

    RUN_TEST(test0)
    RUN_TEST(test8)

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

