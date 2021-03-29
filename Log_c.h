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
 * Interface for the Logging Implementation. Creates the reference to
 * the Logger Singleton.
 */

#if !defined(_LOG_C_H__201130_1555__INCLUDED_)
#define _LOG_C_H__201130_1555__INCLUDED_

#include <stdlib.h>
#include <string>

#include <cstdarg>


/**
 * @section Logging Singleton.
 *
 * Implementation of the Logging Singleton. This code does the work of
 * formatting log entries, buffering them and writing them to the log file.
 */

class Logger_c
{
public:
    static const int FILE_NAME_SIZE = 180;  // Maximum length of the log file path and name.
    static const int LINE_LENGTH = 512;     // Maximum length of each line.
    static const int MAX_LINES = 16;        // Maximum number of lines in the buffer.

//- Delete the copy constructor and assignement operator.
    Logger_c(const Logger_c &) = delete;
    void operator=(const Logger_c &) = delete;

    static Logger_c & getInstance(void) { static Logger_c instance; return instance; }

    int log(const char* qualifier, const char* format, va_list argptr);
    int flush(void);

    bool setLogFilePath(const std::string & path);
    std::string getCurrentLogFilePath(void);
    const std::string & getLogFilePath(void) { return Logger_c::getInstance().logFilePath; }
    void enableTimestamp(bool enable) { Logger_c::getInstance().timestamp = enable; }

private:
//- Hide the default constructor and destructor.
    Logger_c(void) : error(0), count(0) {}
    virtual ~Logger_c(void) { flush(); }

    bool cacheLine(const char* qualifier, const char* format, va_list argptr);

    static Logger_c* instance;

    int error;
    std::string cache[MAX_LINES];
    int count;                      // Current Number of Lines in the Buffer.
    std::string logFilePath;
    bool timestamp = true;

};


/**
 * @section Logging referencer.
 *
 * This is the user interface implemented as a facade.
 *
 * This code holds a local reference to the logging singleton, the module name
 * and the current log level setting.
 */
class Log_c
{
public:
    static const int MAX_LOG_LEVEL = 9;      // Highest logging level supported.
    static const int MODULE_NAME_LEN = 20;   // Maximum module name length.

    Log_c(const char* module, int level = 6);

    static bool isLogLevelValid(int level) { return (level >= 0) && (level <= MAX_LOG_LEVEL); }

    int printf(int level, const char* format, ...);
    int flush(void) { return Logger_c::getInstance().flush(); }

    int getLogLevel(void) const { return logLevel; }
    void setLogLevel(int V) { logLevel = V; }

    std::string getCurrentLogFilePath(void) { return Logger_c::getInstance().getCurrentLogFilePath(); }
    const std::string & getLogFilePath(void) { return Logger_c::getInstance().getLogFilePath(); }
    bool setLogFilePath(const std::string & path) { return Logger_c::getInstance().setLogFilePath(path); }
    void enableTimestamp(bool enable) { Logger_c::getInstance().enableTimestamp(enable); }


private:
    char module[MODULE_NAME_LEN+1];
    int logLevel;

};

#endif // !defined(_LOG_C_H__201130_1555__INCLUDED_)

