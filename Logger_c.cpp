/**
 * @file    Logger_c.cpp
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
 * Implementation of the Logging Singleton. This code does the work of
 * formatting log entries, buffering them and writing them to the log file.
 */

#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>

#include <string.h>

#include "Logger_c.h"


///////////////////////////////////////////////////////////////////////////////
//
// Logging Singleton.
//

Logger_c* Logger_c::instance = 0;       // Initialize instance on demand.

const int Logger_c::FILE_NAME_SIZE = 80;
const int Logger_c::LINE_LENGTH = 512;  // Maximum length of each Line.
const int Logger_c::LINE_COUNT = 16;    // Maximum Number of Lines in the Buffer.


/**
 * Create logging instance, if necessary, and return reference.
 *
 * @return reference to the logging instance.
 */
Logger_c* Logger_c::getInstance(void)
{
    if (instance == 0)
    {
        instance = new Logger_c();

        instance->setLogFilePath("/logs");  // Set up default log path.
    }

    return instance;
}


/**
 * Log the line and flush the buffer if full.
 *
 * @param  qualifier - log entry qualifier, usually module name and log level.
 * @param  format - the log entry format string.
 * @param  argptr - parameters for format string.
 * @return negative error value or 0 if no errors.
 */
int Logger_c::log(const char* qualifier, const char* format, va_list argptr)
{
//- Abort on previous error.
    if (instance->error)
    {
        return -2;
    }

//- Buffer the log line then flush the buffer if full.
    int ret = 0;
    if (cacheLine(qualifier, format, argptr))
    {
        ret = flush();
    }

    return ret;
}


/**
 * Flush the buffer sending output to todays log file.
 *
 * @return negative error value or 0 if no errors.
 */
int Logger_c::flush(void)
{
    time_t now = time(NULL);
    struct tm tim = *localtime(&now);
    char FileName[FILE_NAME_SIZE];
    int ret = 0;

    sprintf(FileName, "%s/log-%04d-%02d-%02d.txt", instance->logFilePath.c_str(), tim.tm_year + 1900, tim.tm_mon + 1, tim.tm_mday);

//- Copy the buffer to the log file.
    ofstream outfile(FileName, ofstream::out | ofstream::app);
    for (vector<string>::iterator ii = instance->cache.begin(); ii != instance->cache.end(); ++ii)
    {
        outfile << (*ii) << endl;
    }

//- Clear the buffer.
    instance->cache.clear();

    return ret;
}


/**
 * Sets the path for the log files and ensures that the directory exists.
 * Also updates "error" if problems encountered, clears it otherwise.
 *
 * @return true if successful, false otherwise.
 */
bool Logger_c::setLogFilePath(const string & path)
{
//- Save the new path and Strip off trailing '/' if present.
    const size_t filePathLen = path.length();
    if ((filePathLen) && (path[filePathLen-1] != '/'))
    {
        instance->logFilePath = path;
    }
    else
    {
        instance->logFilePath = path.substr(0, filePathLen-1);
    }

//- Check if the direstory exists and create if necessary.
    DIR* dir = opendir(instance->logFilePath.c_str());
    if (dir)
    {
        // Directory already exists.
        closedir(dir);
    }
    else
    if (ENOENT == errno)
    {
        // Create a new Directory.
        instance->error = mkdir(instance->logFilePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    else
    {
        // opendir() failed for some other reason.
        instance->error = 1;
    }

    return (instance->error ? false : true);
}


/**
 * Creates and buffers the log entry.
 *
 * @param  qualifier - log entry qualifier, usually module name and log level.
 * @param  format - the log entry format string.
 * @param  argptr - parameters for format string.
 * @return true if the Buffer is full, false otherwise.
 */
bool Logger_c::cacheLine(const char* qualifier, const char* format, va_list argptr)
{
    char line[LINE_LENGTH];
    int bytes = 0;
    char * p = line;

//- Conditionally add the time stamp.
    if (timestamp == true)
    {
        struct timespec tp;
        clock_gettime(CLOCK_REALTIME, &tp);

        struct tm tim = *localtime(&tp.tv_sec);

        const int Micros = tp.tv_nsec/1000;
        bytes += sprintf(p+bytes, "%02d:%02d:%02d.%06d ", tim.tm_hour, tim.tm_min, tim.tm_sec, Micros);
    }

//- Add the qualifier.
    bytes += sprintf(p+bytes, "%s ", qualifier);

//- Now add the actual log entry.
    if (vsprintf(p+bytes, format, argptr) < 0)
    {
        instance->error = 1;
    }

//- Add the new line to the buffer and increment the line count.
    instance->cache.push_back(line);
    (instance->count)++;

    return ((instance->count) == (LINE_COUNT));
}

