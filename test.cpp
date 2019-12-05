// test.cpp: main to test Logging Implementation.
//
//////////////////////////////////////////////////////////////////////

#include <iostream>

#include "Log_c.h"

///////////////////////////////////////////////////////////////////////////////
//
// Test code for Logging Singleton.
//
// Build using:
//    g++ -c -o Logger_c.o Logger_c.cpp
//    g++ -c -o Log_c.o Log_c.cpp
//    g++ -c -o test.o test.cpp
//    g++ -c -o mod.o mod.cpp
//    g++ -o test test.o mod.o Logger_c.o Log_c.o 
//
// Test using:
//    ./test
//
// Check results in '/logs' directory.
//

#define ERROR 3

static Log_c log(__FILE__, ERROR);     // Only log serious messages.

extern int module(int level);

void func(const int level)
{
//- Debug tracking.
    log.printf(7, "test::func() called.");

//- Do some work.
    log.printf(1, "Logging level set to %d.", log.getLogLevel());
    log.printf(3, "Logging level set to %d.", log.getLogLevel());
    log.printf(5, "Logging level set to %d.", log.getLogLevel());
    log.printf(7, "Logging level set to %d.", log.getLogLevel());

//- Test a second logger. Usually not necessary.
    Log_c bob("func()", level);

//- Do some bob work.
    bob.printf(1, "Logging level set to %d.", bob.getLogLevel());
    bob.printf(3, "Logging level set to %d.", bob.getLogLevel());
    bob.printf(5, "Logging level set to %d.", bob.getLogLevel());
    bob.printf(7, "Logging level set to %d.", bob.getLogLevel());
}

int main(int argc, char *argv[])
{
//- Overide default log level from command line. Set to a larger value for more verbose log entries.
	if (argc > 1)
	{
		const int lvl = atoi(argv[1]);
        if ((Log_c::isLogLevelValid(lvl)) && (lvl != ERROR))
		{
            log.setLogLevel(lvl);
            std::cout << "Logging level set to " << lvl << std::endl;
        }
	}

//- Debug tracking.
    log.printf(7, "main() called.");

//- Do some work.
	func(6);
    module(2);   // Call test module with same initial log level i.e. no change.

//- For testing purposes change logging level. Typically you would not change the log levels after initialisation.
    log.setLogLevel(5);

//- Do some more work.
	func(4);
    module(7);   // Call test module with verbose log level.

    log.flush();

    std::cout << "Check results in " << log.getLogFilePath() << std::endl;

    return 0;
}
