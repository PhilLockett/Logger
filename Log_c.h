// Log_c.h: interface for the Logging Implementation.
// Creates the reference to Logger Singleton.
//
//////////////////////////////////////////////////////////////////////

#if !defined(LOG_C_H__201130_1555_8548_ABCDEF0123456789__INCLUDED_)
#define LOG_C_H__201130_1555_8548_ABCDEF0123456789__INCLUDED_

#include "Logger_c.h"

using namespace std;

class Log_c
{
public:
    static const int max_log_level = 9;
    static const int module_name_len = 20;

    static bool isLogLevelValid(int level) { return (level >= 0) && (level <= max_log_level); }
    Log_c(const char* module, int level = 6);

    int printf(int level, const char* Format, ...);
    int flush(void) { return loggerRef->Flush(); }

    int getLogLevel(void) const { return logLevel; }
    void setLogLevel(int V) { logLevel = V; }

    const string & getLogFilePath(void) { return loggerRef->GetLogFilePath(); }
    bool setLogFilePath(const string & path) { return loggerRef->SetLogFilePath(path); }
    void enableTimestamp(bool enable) { loggerRef->EnableTimestamp(enable); }


private:
    Logger_c* loggerRef;

    char Module[module_name_len+1];
    int logLevel;

};

#endif // !defined(LOG_C_H__201130_1555_8548_ABCDEF0123456789__INCLUDED_)
