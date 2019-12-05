// Logger_c.cpp: implementation of the Logging Singleton.
//
///////////////////////////////////////////////////////////////////////////////

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

Logger_c* Logger_c::instance = 0;   // Initialize instance on demand.

const int Logger_c::FileNameSize = 80;
const int Logger_c::LineLength = 512;     // Maximum length of each Line.
const int Logger_c::LineCount = 16;       // Maximum Number of Lines in the Buffer.


//- Create instance if necessary and return reference.
Logger_c* Logger_c::getInstance(void)
{
    if (instance == 0)
    {
        instance = new Logger_c();

        instance->SetLogFilePath("/logs");	// Set up default log path.
    }

    return instance;
}


//- Sets the path for the log files and ensures that the directory exists.
bool Logger_c::SetLogFilePath(const string & path)
{
//- Save the new path and Strip off trailing '/' if necessary.
    const size_t filePathLen = path.length();
    if ((filePathLen) && (path[filePathLen-1] != '/'))
    {
        instance->LogFilePath = path;
    }
	else
    {
        instance->LogFilePath = path.substr(0, filePathLen-1);
    }

//- Check if the direstory exists and create if necessary.
    DIR* dir = opendir(instance->LogFilePath.c_str());
    if (dir)
    {
		// Directory already exists.
        closedir(dir);
    }
    else
    if (ENOENT == errno)
    {
		// Create a new Directory.
        instance->Error = mkdir(instance->LogFilePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    else
    {
		// opendir() failed for some other reason.
        instance->Error = 1;
    }

    return (instance->Error ? false : true);
}


//- Creates and buffers the log entry.
bool Logger_c::CacheLine(const char* qualifier, const char* format, va_list argptr)
{
	char line[LineLength];
    int bytes = 0;
    char * p = line;

//- Conditionally add the time stamp.
    if (Timestamp == true)
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
        instance->Error = 1;
    }

//- Add the new line to the buffer and increment the line count.
    instance->Cache.push_back(line);
    (instance->Count)++;

//- Return true if the Buffer is full.
    return ((instance->Count) == (Logger_c::LineCount));
}


//- Flush the buffer sending output to todays log file.
int Logger_c::Flush(void)
{
    time_t now = time(NULL);
    struct tm tim = *localtime(&now);
    char FileName[FileNameSize];
    int ret = 0;

    sprintf(FileName, "%s/log-%04d-%02d-%02d.txt", instance->LogFilePath.c_str(), tim.tm_year + 1900, tim.tm_mon + 1, tim.tm_mday);

//- Copy the buffer to the log file.
    ofstream outfile(FileName, ofstream::out | ofstream::app);
    for (vector<string>::iterator ii = instance->Cache.begin(); ii != instance->Cache.end(); ++ii)
        outfile << (*ii) << endl;

//- Clear the buffer.
    instance->Cache.clear();

    return ret;
}


//- Log the line and possibly flush the buffer if full.
int Logger_c::Log(const char* qualifier, const char* format, va_list argptr)
{
//- Abort on previous error.
    if (instance->Error)
    {
        return -2;
    }

//- Buffer the log line then flush the buffer if full.
    int ret = 0;
    if (CacheLine(qualifier, format, argptr))
    {
        ret = Flush();
    }

    return ret;
}


