/**
 * @file    Logger_c.h
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
 * Interface for the Logging Singleton.
 */

#if !defined(_LOGGER_C_H__20190830_1635_8548__INCLUDED_)
#define _LOGGER_C_H__20190830_1635_8548__INCLUDED_

#include <stdlib.h>
#include <string>
#include <vector>

#include <cstdarg>

using namespace std;


// Implement as a Singleton.
class Logger_c
{
public:
    static Logger_c* getInstance();

    int log(const char* qualifier, const char* format, va_list argptr);
    int flush(void);

    bool setLogFilePath(const string & path);
    const string & getLogFilePath(void) { return instance->logFilePath; }
    void enableTimestamp(bool enable) { instance->timestamp = enable; }

private:
//- Hide the default constructor, destructor, copy constructor and assignement operator.
    Logger_c(void) : error(0), count(0) {}
    virtual ~Logger_c(void) { flush(); }
    Logger_c(Logger_c const&);
    void operator=(Logger_c const&);

    bool cacheLine(const char* qualifier, const char* format, va_list argptr);

    static Logger_c* instance;

    int error;
    static const int FILE_NAME_SIZE;
    static const int LINE_LENGTH;   // Maximum length of each Line.
    static const int LINE_COUNT;    // Maximum Number of Lines in the Buffer.
    vector<string> cache;
    int count;                      // Current Number of Lines in the Buffer.
    string logFilePath;
    bool timestamp = true;

};


#endif // !defined(_LOGGER_C_H__20190830_1635_8548__INCLUDED_)

