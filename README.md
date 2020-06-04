# Log_c

A simple implementation of a logger implemented as a singleton.

## Overview

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

