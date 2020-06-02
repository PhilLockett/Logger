/**
 * @file    Log_c.h * @author  Phil Lockett <phillockett65@gmail.com>
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

#include "Logger_c.h"

using namespace std;

class Log_c
{
public:
    static const int MAX_LOG_LEVEL = 9;      // Highest logging level supported.
    static const int MODULE_NAME_LEN = 20;   // Maximum module name length.

    Log_c(const char* module, int level = 6);

    static bool isLogLevelValid(int level) { return (level >= 0) && (level <= MAX_LOG_LEVEL); }

    int printf(int level, const char* format, ...);
    int flush(void) { return loggerRef->flush(); }

    int getLogLevel(void) const { return logLevel; }
    void setLogLevel(int V) { logLevel = V; }

    const string & getLogFilePath(void) { return loggerRef->getLogFilePath(); }
    bool setLogFilePath(const string & path) { return loggerRef->setLogFilePath(path); }
    void enableTimestamp(bool enable) { loggerRef->enableTimestamp(enable); }


private:
    Logger_c* loggerRef;

    char module[MODULE_NAME_LEN+1];
    int logLevel;

};

#endif // !defined(_LOG_C_H__201130_1555__INCLUDED_)

