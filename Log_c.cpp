// Log_c.cpp: Logging Implementation.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "Log_c.h"

///////////////////////////////////////////////////////////////////////////////
//
// Reference to Logging Singleton.
//
Log_c::Log_c(const char* ModuleName, int Level) : logLevel(Level)
{
    loggerRef = Logger_c::getInstance();
    sprintf(Module, "%-*.*s", module_name_len, module_name_len, ModuleName);
}

// Check logging level and conditionally send the line to the log.
int Log_c::printf(int Level, const char* Format, ...)
{
    if (Level <= logLevel)
    {
        va_list argptr;
        va_start(argptr, Format);
        char qualifier[80];

        //- Use the module name and logging level as qualifier.
        sprintf(qualifier, "%s L%d -", Module, Level);

        int ret = loggerRef->Log(qualifier, Format, argptr);

        va_end(argptr);

        return ret;
    }

    return -1;
}

#if 0

///////////////////////////////////////////////////////////////////////////////
//
// Test code for Logging Singleton.
//    Test two log streams with logging messages of decreasing importance
//    Adjust the logging level of each stream
//    send the same messages again
//
// Build using:
//    g++ -c -o Logger_c.o Logger_c.cpp
//    g++ -c -o Log_c.o Log_c.cpp
//    g++ -o test Logger_c.o Log_c.o 
//
// Test using:
//    ./test
//
// Check results in '/logs' directory.
//
static Log_c log(__FILE__, 3);     // Only log serious messages.

int main(void)
{
    int ret = 0;

    Log_c bob("Bob", 6);            // Make Bob verbose.

    log.printf(1, "Logging level set to %d", log.getLogLevel());
    log.printf(3, "Logging level set to %d", log.getLogLevel());
    log.printf(5, "Logging level set to %d", log.getLogLevel());
    log.printf(7, "Logging level set to %d", log.getLogLevel());
    bob.printf(1, "Logging level set to %d", bob.getLogLevel());
    bob.printf(3, "Logging level set to %d", bob.getLogLevel());
    bob.printf(5, "Logging level set to %d", bob.getLogLevel());
    bob.printf(7, "Logging level set to %d", bob.getLogLevel());

    // Testing. Typically you would not set the log levels after initialisation.
    log.setLogLevel(5);
    bob.setLogLevel(4);

    log.printf(1, "Logging level set to %d", log.getLogLevel());
    log.printf(3, "Logging level set to %d", log.getLogLevel());
    log.printf(5, "Logging level set to %d", log.getLogLevel());
    log.printf(7, "Logging level set to %d", log.getLogLevel());
    bob.printf(1, "Logging level set to %d", bob.getLogLevel());
    bob.printf(3, "Logging level set to %d", bob.getLogLevel());
    bob.printf(5, "Logging level set to %d", bob.getLogLevel());
    bob.printf(7, "Logging level set to %d", bob.getLogLevel());

    log.flush();

    return ret;
}

#endif

