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

Fast examples:

code:
    LOG_INFO("Hello world! Ten: %d, string: %s!!",10,"ABCDEFG");
result in log:
    [INFO] 04.12.2015 16:47:27.676 [6152:6744] [loggertest.exe!main] Hello world! Ten: 10, string: ABCDEFG!!

code:
    LOG_WARNING("Testing warning at $(srcfile), line $(line)");
result in log:
    [WARNING] 04.12.2015 16:47:27.676 [6152:6744] [loggertest.exe!main] Testing warning at loggertest.cpp, line 185


code:
    char t[128];
    for (int i=0; i<128; i++) t[i] = i;
    LOG_BINARY_ERROR(t,sizeof(t));
result in log:
    [ERROR] 04.12.2015 16:47:35.519 [6152:6744] [loggertest.exe!main] 
    0000: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |................
    0010: 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F |................
    0020: 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F |.!"#$%&'()*+,-./
    0030: 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F |0123456789:;<=>?
    0040: 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F |@ABCDEFGHIJKLMNO
    0050: 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F |PQRSTUVWXYZ[\]^_
    0060: 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F |`abcdefghijklmno
    0070: 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F |pqrstuvwxyz{|}~
    
code:
    LOG_MODULES_DEBUG;
result in log:
    [DEBUG] 04.12.2015 16:47:35.511 [6152:6744] [loggertest.exe!main]
    Base	Size	Module name	Image name	Version info
    ------------------------------------------------
    0x000000013F5E0000	360448	loggertest.exe	F:\WORK\logger\bin\Debug_x64\loggertest.exe	
    0x0000000076CD0000	1748992	ntdll.dll	C:\Windows\SYSTEM32\ntdll.dll	LangId:4B00409	FileVersion:"6.1.7600.16385 (win7_rtm.090713-1255)"	ProductVersion:"6.1.7600.16385"	CompanyName:"Microsoft Corporation"	FileDescription:"NT Layer DLL"	
    0x0000000076BB0000	1175552	kernel32.dll	C:\Windows\system32\kernel32.dll	LangId:4B00409	FileVersion:"6.1.7600.16385 (win7_rtm.090713-1255)"	ProductVersion:"6.1.7600.16385"	CompanyName:"Microsoft Corporation"	FileDescription:"Windows NT BASE API Client DLL"	
    0x000007FEFCEF0000	438272	KERNELBASE.dll	C:\Windows\system32\KERNELBASE.dll	LangId:4B00409	FileVersion:"6.1.7600.16385 (win7_rtm.090713-1255)"	ProductVersion:"6.1.7600.16385"	CompanyName:"Microsoft Corporation"	FileDescription:"Windows NT BASE API Client DLL"	
    0x0000000076AB0000	1024000	USER32.dll	C:\Windows\system32\USER32.dll	LangId:4B00409	FileVersion:"6.1.7600.16385 (win7_rtm.090713-1255)"	ProductVersion:"6.1.7600.16385"	CompanyName:"Microsoft Corporation"	FileDescription:"Multi-User Windows USER API Client DLL"	
    ...

code:
    LOG_STACKTRACE_DEBUG;
result in log:
    [DEBUG] 04.12.2015 16:47:28.777 [6152:6744] [loggertest.exe!TestFunction] Stack trace:
    [2] <-- TestFunction +215 bytes [FUNCTION: TestFunction] [Line: f:\work\logger\samples\loggertest\loggertest.cpp(70) +0 bytes] [Mod:  loggertest, base: 000000013F5E0000] 
     Offset: 0x000000013F607427 Sym:  type: PDB, file: F:\WORK\logger\bin\Debug_x64\loggertest.exe
    [3] <-- Func +242 bytes [FUNCTION: Func] [Line: f:\work\logger\samples\loggertest\loggertest.cpp(80) +5 bytes] [Mod:  loggertest, base: 000000013F5E0000] 
     Offset: 0x000000013F607552 Sym:  type: PDB, file: F:\WORK\logger\bin\Debug_x64\loggertest.exe
    [4] <-- main +689 bytes [FUNCTION: main] [Line: f:\work\logger\samples\loggertest\loggertest.cpp(195) +5 bytes] [Mod:  loggertest, base: 000000013F5E0000] 
     Offset: 0x000000013F607BA1 Sym:  type: PDB, file: F:\WORK\logger\bin\Debug_x64\loggertest.exe
    [5] <-- __tmainCRTStartup +412 bytes [FUNCTION: __tmainCRTStartup] [Line: f:\dd\vctools\crt_bld\self_64_amd64\crt\src\crtexe.c(555) +25 bytes] [Mod:  loggertest, base: 000000013F5E0000] 
     Offset: 0x000000013F60C31C Sym:  type: PDB, file: F:\WORK\logger\bin\Debug_x64\loggertest.exe
    [6] <-- mainCRTStartup +14 bytes [FUNCTION: mainCRTStartup] [Line: f:\dd\vctools\crt_bld\self_64_amd64\crt\src\crtexe.c(371) +0 bytes] [Mod:  loggertest, base: 000000013F5E0000] 
     Offset: 0x000000013F60C16E Sym:  type: PDB, file: F:\WORK\logger\bin\Debug_x64\loggertest.exe
    [7] <-- BaseThreadInitThunk +13 bytes [FUNCTION: BaseThreadInitThunk] [Mod:  kernel32, base: 0000000076BB0000] 
     Offset: 0x0000000076BCF56D Sym:  type: -exported-, file: C:\Windows\system32\kernel32.dll
    [8] <-- RtlUserThreadStart +33 bytes [FUNCTION: RtlUserThreadStart] [Mod:  ntdll, base: 0000000076CD0000] 
     Offset: 0x0000000076D03281 Sym:  type: -exported-, file: C:\Windows\SYSTEM32\ntdll.dll
    
