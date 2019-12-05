// mod.cpp: Dummy module to test Logging Implementation.
//
//////////////////////////////////////////////////////////////////////

#include "Log_c.h"

#define MAJOR 2

static Log_c log(__FILE__, MAJOR);     // Only log serious messages.

int module(int level)
{
//- For testing purposes change logging level. Typically you would not change the log levels after initialisation.
    if (level != MAJOR)
	{
        log.setLogLevel(level);
    }

//- Debug tracking.
    log.printf(7, "mod::module() called.");

//- Do some work.
    log.printf(1, "Logging level set to %d.", log.getLogLevel());
    log.printf(3, "Logging level set to %d.", log.getLogLevel());
    log.printf(5, "Logging level set to %d.", log.getLogLevel());
    log.printf(7, "Logging level set to %d.", log.getLogLevel());

    return 0;
}

