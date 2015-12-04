#!/bin/sh

gcc -DLOG_USE_DLL=0 -DLOG_PROCESS_MACRO_IN_LOG_TEXT=1 ./samples/loggertest/loggertest.cpp ./samples/loggertest/loggertest_c.c ./logger/logger.cpp -o ./build/loggertest -ldl -rdynamic -lstdc++ -lc
