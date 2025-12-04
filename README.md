# Log_c

A simple logger implemented as a singleton.

**USE AT OWN RISK.**

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

The test code is dependent on UnitTest. To compile and run the Logger code you
first need to clone the unit test code, then copy unittest.cpp and unittest.h 
into the Logger directory before executing make.

    git clone https://github.com/PhilLockett/UnitTest.git

To clone, build and run the Logger code, including the test code, execute the 
following unix/linux commands:

    git clone https://github.com/PhilLockett/Logger.git
    cd Logger/
    cp ../UnitTest/unittest.cpp .
    cp ../UnitTest/unittest.h .
    make
    ./test

## Points of interest

This code has the following points of interest:

  * The main logger code (Logger_c) is implemented as a singleton.
  * The API (Log_c) is implemented as a façade.
  * The API hides references to the logger instance.
  * The API maintains the identifying name and logging level.
  * The log file path can be specified as needed (default: '/logs').
  * Timestamps can be suppressed for 'before and after' log file comparison.
