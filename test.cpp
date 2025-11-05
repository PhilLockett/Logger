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
#include <sstream>
#include <filesystem>
#include <vector>
#include <thread>

#include "Log_c.h"

#include "TextFile.h"
#include "unittest.h"


/**
 * @section basic utility code.
 */

static bool checkFileExists(const std::string & path)
{
    return std::filesystem::exists(path);
}

static void deleteDirectory(const std::string & path)
{
    std::filesystem::remove_all(path); // Delete directory and contents.
}

// Counts the number of lines in a text file.
static int getFileLength(const std::string & fileName)
{
    std::ifstream infile(fileName, std::ifstream::in);
    if (!infile.is_open())
        return 0;

    int count = 0;
    std::string line;

    while (getline(infile, line))
    {
        if (!infile.eof() && line.length())
            count++;
    }

    infile.close();

    return count;
}

// Checks that all lines in a text file are of the required length.
static bool checkFileLineLength(const std::string & fileName, int length)
{
    std::ifstream infile(fileName, std::ifstream::in);
    if (!infile.is_open())
        return false;

    std::string line;

    while (getline(infile, line))
    {
        if (!infile.eof() && line.length())
            if (line.length() != length)
                return false;
    }

    infile.close();

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


/**
 * @section test sending a log entry to default log file location.
 */

UNIT_TEST(testDefault, "Test sending a log entry to default log file location.")

    REQUIRE(log.logf(MAJOR, "Testing logging code.") == 0)
    log.flush();

    std::string currentLogFileName = log.getFullLogFileName();
    REQUIRE(checkFileExists(currentLogFileName) == true)

END_TEST


/**
 * @section test sending log entries in various ways to check that low priority entires are filtered out..
 */

extern int remoteFunction(int level = MAJOR);

UNIT_TEST(test0, "Test sending log entries using global log reference.")

//- Initialize test set up.
    int targetCount{};
    const std::string path = "logs/";
    deleteDirectory(path);
    REQUIRE(log.setLogFilePath(path) == true)
    log.enableTimestamp(false);
    const std::string currentLogFileName = log.getFullLogFileName();

    TextFile<> comp{std::string{"expected-log.txt"}};
    comp.read(39);
    REQUIRE(comp.size() == 39);

    for (int loggingLevel = CRITICAL; loggingLevel < MAX; ++loggingLevel)
        log.logf(loggingLevel, "Logging level set to %d.", log.getLogLevel());

    targetCount = 3;
    log.flush();
    TextFile<> entries{currentLogFileName};
    entries.read(targetCount);

    REQUIRE(entries.size() == targetCount)
    REQUIRE(entries.equal(comp, targetCount))

NEXT_CASE(test1, "Test sending log entries using local log reference.")

    Log_c test1Log("test1", DEBUG);    // Make test1 chatty.
    for (int loggingLevel = CRITICAL; loggingLevel < MAX; ++loggingLevel)
        test1Log.logf(loggingLevel, "Logging level set to %d.", test1Log.getLogLevel());

    targetCount = 10;
    log.flush();
    entries.clear();
    entries.read(targetCount);

    REQUIRE(entries.size() == targetCount)
    REQUIRE(entries.equal(comp, targetCount))

NEXT_CASE(test2, "Test sending log entries from remote code.")

    remoteFunction();   // Call test module.

    targetCount = 12;
    log.flush();
    entries.clear();
    entries.read(targetCount);

    REQUIRE(entries.size() == targetCount)
    REQUIRE(entries.equal(comp, targetCount))

NEXT_CASE(test3, "Test changing logging level.")

    log.setLogLevel(INFO);
    for (int loggingLevel = 1; loggingLevel < Log_c::MAX_LOG_LEVEL; ++loggingLevel)
        log.logf(loggingLevel, "Logging level set to %d.", log.getLogLevel());

    targetCount = 18;
    log.flush();
    entries.clear();
    entries.read(targetCount);

    REQUIRE(entries.size() == targetCount)
    REQUIRE(entries.equal(comp, targetCount))

NEXT_CASE(test4, "Test interleaving log entries.")

    for (int loggingLevel = 1; loggingLevel < Log_c::MAX_LOG_LEVEL; ++loggingLevel)
    {
        log.logf(loggingLevel, "Logging level set to %d.", log.getLogLevel());
        test1Log.logf(loggingLevel, "Logging level set to %d.", test1Log.getLogLevel());
    }

    targetCount = 31;
    log.flush();
    entries.clear();
    entries.read(targetCount);

    REQUIRE(entries.size() == targetCount)
    REQUIRE(entries.equal(comp, targetCount))

NEXT_CASE(test5, "Test sending verbose log entries from remote code.")

    remoteFunction(VERBOSE);   // Call test module.

    targetCount = 39;
    log.flush();
    entries.clear();
    entries.read(targetCount);

    REQUIRE(entries.size() == targetCount)
    REQUIRE(entries.equal(comp, targetCount))

END_TEST


/**
 * @section test sending a large number of log entries code.
 */

UNIT_TEST(test6, "Test sending a large number of log entries.")

//- Initialize test set up.
    const std::string path = "speed";
    const int ENTRIES = 100000;
    const int LEVEL = NOTICE;

    deleteDirectory(path);
    REQUIRE(log.setLogFilePath(path) == true)
    log.enableTimestamp(true);
    log.setLogLevel(LEVEL);

    for (int i = 0; i < ENTRIES; ++i)
        for (int loggingLevel = CRITICAL; loggingLevel < MAX; ++loggingLevel)
            log.logf(loggingLevel, "Logging level set to %d - adding log entry %d", log.getLogLevel(), i);

    log.flush();

    std::string currentLogFileName = log.getFullLogFileName();
    REQUIRE(getFileLength(currentLogFileName) == (ENTRIES*LEVEL))

END_TEST


/**
 * @section test a large number of log entries generated by different threads.
 */

static void worker(const int count, const int level)
{
static std::mutex displayMutex;

    std::stringstream id;
    id << "Thread " << std::this_thread::get_id();
    Log_c threadLog(id.str().c_str(), level);

    if (IS_VERBOSE)
    {
        std::lock_guard<std::mutex> lock(displayMutex);
        std::cout << "\t" << id.str() << " working" << std::endl;
    }

    for (int i = 0; i < count; ++i)
        for (int loggingLevel = CRITICAL; loggingLevel < MAX; ++loggingLevel)
            threadLog.logf(loggingLevel, "Entry %6d", i);

    if (IS_VERBOSE)
    {
        std::lock_guard<std::mutex> lock(displayMutex);
        std::cout << "\t" << id.str() << " finished" << std::endl;
    }
}

static void startWorkers(const int threads, const int count, const int level)
{
    std::vector<std::future<void>> futures;
    futures.reserve(threads);
    for (int i = 0; i < threads; ++i)
    {
        futures.push_back(std::async(std::launch::async, worker, count, level));
    }
}

UNIT_TEST(test7, "Test a large number of log entries generated by different threads.")

//- Initialize test set up.
    const std::string path = "threads";
    const int ENTRIES = 1000;
    const int THREADS = 10;
    const int LEVEL = NOTICE;

    deleteDirectory(path);
    REQUIRE(log.setLogFilePath(path) == true)
    log.enableTimestamp(true);
    log.setLogLevel(LEVEL);

    startWorkers(THREADS, ENTRIES, LEVEL);

    log.flush();
    std::string currentLogFileName = log.getFullLogFileName();

    REQUIRE(getFileLength(currentLogFileName) == (THREADS*ENTRIES*LEVEL))
    REQUIRE(checkFileLineLength(currentLogFileName, 54) == true)

END_TEST


/**
 * @section launch the tests and check the results.
 */

static int runTests(void)
{
    std::cout << "Executing all tests.\n";
//    VERBOSE_OFF

    RUN_TEST(testDefault)
    RUN_TEST(test0)
    RUN_TEST(test6)
    RUN_TEST(test7)

    const int err{FINISHED};
    OUTPUT_SUMMARY;

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

