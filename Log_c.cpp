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
#include <filesystem>
#include <algorithm>

#include "Log_c.h"


/**
 * @section Logging Singleton.
 *
 * Implementation of the Logging Singleton. This code does the work of
 * formatting log entries, caching them and writing them to the log file.
 */


/**
 * Construct the full log file name for todays log file.
 *
 * @return a new string containing the log file name.
 */
std::string Logger_c::_getFullLogFileName(void) const
{
    time_t now = time(NULL);
    struct tm tim = *localtime(&now);
    char FileName[FILE_NAME_LENGTH];

    const char * path = logFilePath.c_str();
    sprintf(FileName, "%s/log-%04d-%02d-%02d.txt", path, tim.tm_year + 1900, tim.tm_mon + 1, tim.tm_mday);

    return std::string(FileName);
}


/**
 * Sets the path for the log files and ensures that the directory exists.
 * Also updates "error" if problems encountered, clears it otherwise.
 *
 * @return true if successful, false otherwise.
 */
bool Logger_c::_setLogFilePath(const std::string & path)
{
    if (path.length())
    {
        false;
    }

//- Save the new path and Strip off trailing '/' if present.
    const std::string JUNK = "/\n\r\\";
    const size_t last = path.find_last_not_of(JUNK);
    if (last == std::string::npos)
    {
        logFilePath = path;
    }
    else
    {
        logFilePath = path.substr(0, last + 1);
    }

//- Create directory.
    return std::filesystem::create_directories(logFilePath);
}


/**
 * Write the cache into the current log file then clear the cache.
 *
 * @return negative error value or 0 if no errors.
 */
int Logger_c::_flush(void)
{
    int ret = 0;

    // Set up default log path, if necessary.
    std::call_once(checkFilePathSet, [this](){ if (logFilePath.empty()) _setLogFilePath("/logs"); });

//- Write the cache to the log file.
    std::ofstream outfile(_getFullLogFileName(), std::ofstream::out | std::ofstream::app);
    const int entries = count;
    auto line2Log = [&outfile](const auto & s) { outfile << s << '\n'; };
    std::for_each_n(cache.begin(), entries, line2Log);

//- Clear the cache.
    count = 0;

    return ret;
}


/**
 * Insert a time stamp into the buffer pointed at by p.
 *
 * @param p - pointer to buffer to hold time stamp.
 * @return the length of the time stamp.
 */
int Logger_c::_getTimestamp(char * p) const
{
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);

    struct tm tim = *localtime(&tp.tv_sec);

    const int Micros = tp.tv_nsec/1000;

    return sprintf(p, "%02d:%02d:%02d.%06d ", tim.tm_hour, tim.tm_min, tim.tm_sec, Micros);
}
/**
 * Generates and caches the log entry.
 *
 * @param  qualifier - log entry qualifier, usually module name and log level.
 * @param  format - the log entry format string.
 * @param  argptr - parameters for format string.
 * @return true if the cache is full, false otherwise.
 */
bool Logger_c::_cacheLine(const char* qualifier, const char* format, va_list argptr)
{
    char line[LINE_LENGTH];
    char * p = line;

//- Conditionally add the time stamp.
    if (timestamp == true)
    {
        p += _getTimestamp(p);
    }

//- Add the qualifier.
    p += sprintf(p, "%s ", qualifier);

//- Now add the actual log entry.
    if (vsprintf(p, format, argptr) < 0)
    {
        error = 1;
    }

//- Add the new line to the cache and increment the line count.
    cache[count] = line;
    ++count;

    return ((count) == (MAX_LINES));
}


/**
 * Put the log entry in the cache. If this fills it, flush the cache.
 *
 * @param  qualifier - log entry qualifier, usually module name and log level.
 * @param  format - the log entry format string.
 * @param  argptr - parameters for format string.
 * @return negative error value or 0 if no errors.
 */
int Logger_c::_log(const char* qualifier, const char* format, va_list argptr)
{
//- Abort on previous error.
    if (error)
    {
        return -2;
    }

//- Cache the log entry then flush the cache if full.
    int ret = 0;
    if (_cacheLine(qualifier, format, argptr))
    {
        ret = _flush();
    }

    return ret;
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
Log_c::Log_c(const char* moduleName, int level) : logLevel{level}
{
    // Pad or truncate moduleName.
    sprintf(module, "%-*.*s", MODULE_NAME_LEN, MODULE_NAME_LEN, moduleName);
}


/**
 * Compare logging levels and cache the entry if sufficiently important.
 *
 * @param  level - the logging level for this log entry.
 * @param  format - the log entry format string, followed by parameters.
 * @return negative error value or 0 if no errors.
 */
int Log_c::printf(int level, const char* format, ...) const
{
    if (level <= logLevel)
    {
        va_list argptr;
        va_start(argptr, format);
        char qualifier[MODULE_NAME_LEN+10];    // ugly

        //- Use the module name and logging level as qualifier.
        sprintf(qualifier, "%s L%d -", module, level);

        int ret = Logger_c::getInstance().log(qualifier, format, argptr);

        va_end(argptr);

        return ret;
    }

    return -1;
}
