# cpp-crosslogger
Crossplatform C/C++ logger with runtime analysis functions

Platforms support:
- Windows
- POSIX (Linux, CygWin, etc)

Functions:
- Quickly logging (caches header processing)
- Determination of the name of DLL from which the function was called
- Displays a list of DLLs with file version information
- Write to log extended information about the OS version
- Output stack trace with symbols
- The creation of the dump file when application crashes and storing crash info (stacktrace, etc.)
- Configuration through registry
- Configuration through ini file
- Configuration through program Code
- Support for multiple instances of the logger in different modules (if the EXE and DLL files using each of its logger)
- Scrolling log file by file size, scrolling at each start, limiting the number of files
- Support for 32-bit and 64-bit architectures
