// Logger_c.h: interface for the Logging Singleton.
//
//////////////////////////////////////////////////////////////////////

#if !defined(LOGGER_C_H__20190830_1635_8548_ABCDEF0123456789__INCLUDED_)
#define LOGGER_C_H__20190830_1635_8548_ABCDEF0123456789__INCLUDED_

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

    int Log(const char* qualifier, const char* format, va_list argptr);
    int Flush(void);

    const string & GetLogFilePath(void) { return instance->LogFilePath; }
    bool SetLogFilePath(const string & path);
    void EnableTimestamp(bool enable) { instance->Timestamp = enable; }

private:
//- Hide the default constructor, destructor, copy constructor and assignement operator.
    Logger_c(void) : Error(0), Count(0) {}
    virtual ~Logger_c(void) { Flush(); }
    Logger_c(Logger_c const&);
    void operator=(Logger_c const&);

    bool CacheLine(const char* qualifier, const char* format, va_list argptr);

    static Logger_c* instance;

    int Error;
    static const int FileNameSize;
    static const int LineLength;    // Maximum length of each Line.
    static const int LineCount;     // Maximum Number of Lines in the Buffer.
    vector<string> Cache;
    int Count;                      // Current Number of Lines in the Buffer.
    string LogFilePath;
    bool Timestamp = true;

};


#endif // !defined(LOGGER_C_H__20190830_1635_8548_ABCDEF0123456789__INCLUDED_)
