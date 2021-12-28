/**
 * @file    Log_c.h
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
 * Interface for the Logging Implementation.
 */

#if !defined(_LOG_C_H__201130_1555__INCLUDED_)
#define _LOG_C_H__201130_1555__INCLUDED_

#include <stdlib.h>
#include <string>
#include <array>
#include <future>

#include <cstdarg>


/**
 * @section Logging Singleton.
 *
 * Implementation of the Logging Singleton. This code does the work of
 * formatting log entries, caching them and writing them to the log file.
 */

class Logger_c
{
public:
    static const int FILE_NAME_LENGTH{180}; // Maximum length of the path to the log file.
    static const int LINE_LENGTH{512};      // Maximum length of a line.
    static const int MAX_LINES{256};        // Maximum number of cached lines.

//- Delete the copy constructor and assignement operator.
    Logger_c(const Logger_c &) = delete;
    void operator=(const Logger_c &) = delete;

    static Logger_c & getInstance(void) { static Logger_c instance; return instance; }

    int log(const char* qualifier, const char* format, va_list argptr) { std::lock_guard<std::mutex> lock(logMutex); return _log(qualifier, format, argptr); }
    int flush(void) { std::lock_guard<std::mutex> lock(logMutex); return _flush(); }

    bool setLogFilePath(const std::string & path) { std::lock_guard<std::mutex> lock(logMutex); return _setLogFilePath(path); }
    std::string getFullLogFileName(void) const  { std::lock_guard<std::mutex> lock(logMutex); return _getFullLogFileName(); }
    const std::string & getLogFilePath(void) { std::lock_guard<std::mutex> lock(logMutex); return logFilePath; }

    void enableTimestamp(bool enable) { std::lock_guard<std::mutex> lock(logMutex); timestamp = enable; }

private:
//- Hide the default constructor and destructor.
    Logger_c(void) : error{}, count{}, timestamp{true} {}
    virtual ~Logger_c(void) { flush(); }

    std::string _getFullLogFileName(void) const;
    bool _setLogFilePath(const std::string & path);

    int _flush(void);
    int _getTimestamp(char * p) const;
    bool _cacheLine(const char* qualifier, const char* format, va_list argptr);
    int _log(const char* qualifier, const char* format, va_list argptr);

    std::once_flag checkFilePathSet;
    mutable std::mutex logMutex;
    std::array<std::string, MAX_LINES> cache;
    std::string logFilePath;
    int error;
    int count;                      // Current number of cached lines.
    bool timestamp;

};


/**
 * @section Logging referencer.
 *
 * This is the user interface implemented as a facade.
 *
 * Holds the module name and the current log level setting and alleviates the
 * getInstance() need.
 */
class Log_c
{
public:
    static const int MAX_LOG_LEVEL{9};      // Highest logging level supported.
    static const int MODULE_NAME_LEN{20};   // Maximum module name length.

    Log_c(const char* module, int level = 6);

    static bool isLogLevelValid(int level) { return (level >= 0) && (level <= MAX_LOG_LEVEL); }

    int logf(int level, const char* format, ...) const;
    int flush(void) const { return Logger_c::getInstance().flush(); }

    int getLogLevel(void) const { return logLevel; }
    void setLogLevel(int V) { logLevel = V; }

    std::string getFullLogFileName(void) const { return Logger_c::getInstance().getFullLogFileName(); }
    const std::string & getLogFilePath(void) const { return Logger_c::getInstance().getLogFilePath(); }
    bool setLogFilePath(const std::string & path) const { return Logger_c::getInstance().setLogFilePath(path); }
    void enableTimestamp(bool enable) const { Logger_c::getInstance().enableTimestamp(enable); }


private:
    char module[MODULE_NAME_LEN+1];
    int logLevel;                   // Current logging level cut off.

};

#endif // !defined(_LOG_C_H__201130_1555__INCLUDED_)
