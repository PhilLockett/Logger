# Log_c

A simple logger implemented as a singleton.

## Overview

A logger collects together system wide messages, useful in a real time 
environment. It should also allow for logging levels to be easily adjusted so 
that more granular messages from specific modules can be obtained to 
facilitate debugging.

This is a simple logger implemented as a singleton with a façade interface. 
All log entries are sent to the same log file. The default log path is 
hardcoded as "/logs/", but can be adjusted to suit, or changed dynamically 
with the setLogFilePath() method. As the test code shows, usual usage is to 
create a 'log_c' instance supplying it with a module/stream/file name and the 
logging level required.

Logging levels are integer values from 1 (most critical) to 'MAX_LOG_LEVEL' 
(least critical) which is currently hardcoded to 9, but can easily be 
changed.

The logger code is wholly contained in the files 'Log_c.cpp' and 'Log_c.h'. 
All other files are to support the unit test code. The code is liberally 
commented. The test code exercises most of the API and illustrates the usage.

## Cloning and Running

To clone, build and run this code, execute the following unix/linux commands:

    git clone https://github.com/PhilLockett/Logger.git
    cd Logger/
    make
    ./test

The initial logging level for 'test.cpp' can be set from the command line, 
but the test code does set the logging level to 5 half way through testing.
Example of setting the initial logging level for 'test.cpp' to the maximum:

    ./test 9

## Points of interest

This code has the following points of interest:

  * The main logger code (Logger_c) is implemented as a singleton.
  * The API (Log_c) is implemented as a façade.
  * The API hides the pointer to the logger instance.
  * The API maintains the modules name and logging level.
  * The log file path can easily be modified as needed.
  * Timestamps can be suppressed for before and after log file comparison.
