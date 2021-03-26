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
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>

#include <string>

#include "Log_c.h"


/**
 * @section Logging Singleton.
 *
 * Implementation of the Logging Singleton. This code does the work of
 * formatting log entries, buffering them and writing them to the log file.
 */




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
    if (Logger_c::getInstance().error)
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
 * Construct the full log file path for todays log file.
 *
 * @return a new string containing the log file path.
 */
std::string Logger_c::getCurrentLogFilePath(void)
{
    time_t now = time(NULL);
    struct tm tim = *localtime(&now);
    char FileName[180];

    const char * path = Logger_c::getInstance().logFilePath.c_str();
    sprintf(FileName, "%s/log-%04d-%02d-%02d.txt", path, tim.tm_year + 1900, tim.tm_mon + 1, tim.tm_mday);

    return std::string(FileName);
}


/**
 * Flush the buffer sending output to todays log file.
 *
 * @return negative error value or 0 if no errors.
 */
int Logger_c::flush(void)
{
    int ret = 0;

    if (Logger_c::getInstance().logFilePath.empty())
        Logger_c::getInstance().setLogFilePath("/logs");	// Set up default log path.

//- Copy the buffer to the log file.
    std::ofstream outfile(getCurrentLogFilePath(), std::ofstream::out | std::ofstream::app);
    const int entries = Logger_c::getInstance().count;
    for (int i = 0; i < entries; ++i)
    {
        outfile << Logger_c::getInstance().cache[i] << '\n';
    }

//- Clear the buffer.
    Logger_c::getInstance().count = 0;

    return ret;
}


/**
 * Sets the path for the log files and ensures that the directory exists.
 * Also updates "error" if problems encountered, clears it otherwise.
 *
 * @return true if successful, false otherwise.
 */
bool Logger_c::setLogFilePath(const std::string & path)
{
//- Save the new path and Strip off trailing '/' if present.
    const size_t filePathLen = path.length();
    if ((filePathLen) && (path[filePathLen-1] != '/'))
    {
        Logger_c::getInstance().logFilePath = path;
    }
    else
    {
        Logger_c::getInstance().logFilePath = path.substr(0, filePathLen-1);
    }

//- Check if the directory exists and create if necessary.
    const char * logPath = Logger_c::getInstance().logFilePath.c_str();
    DIR* dir = opendir(logPath);
    if (dir)
    {
        // Directory already exists.
        closedir(dir);
    }
    else
    if (ENOENT == errno)
    {
        // Create a new Directory.
        Logger_c::getInstance().error = mkdir(logPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    else
    {
        // opendir() failed for some other reason.
        Logger_c::getInstance().error = 1;
    }

    return (Logger_c::getInstance().error ? false : true);
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
    char line[512];
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
        Logger_c::getInstance().error = 1;
    }

//- Add the new line to the buffer and increment the line count.
    Logger_c::getInstance().cache[Logger_c::getInstance().count] = line;
    ++(Logger_c::getInstance().count);

    return ((Logger_c::getInstance().count) == (__MAX_LINES__));
}



/**
 * @section Logging referencer.
 *
 * This code holds a local reference to the logging singleton, the module name
 * and the current log level setting.
 */

/**
 * The only Constructor.
 *
 * @param  ModuleName - String identifier used in log messages.
 * @param  level - the logging level.
 */
Log_c::Log_c(const char* moduleName, int level)
    : loggerRef(Logger_c::getInstance()), logLevel(level)
{
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
        char qualifier[MODULE_NAME_LEN+10];    // ugly

        //- Use the module name and logging level as qualifier.
        sprintf(qualifier, "%s L%d -", module, level);

        int ret = loggerRef.log(qualifier, format, argptr);

        va_end(argptr);

        return ret;
    }

    return -1;
}


