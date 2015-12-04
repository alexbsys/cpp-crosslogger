#!/bin/sh

mkdir build

gcc -c -DLOG_THIS_IS_DLL -fPIC -static-libstdc++ -static-libgcc -Wl,-Bstatic -lc logger/logger.cpp -o build/logger.o
gcc build/logger.o -shared -static-libstdc++ -static-libgcc -Wl,-Bstatic -lc -lstdc++ -o build/libloggerdll.so
