

#include "../../logger/logger.h"

#ifdef LOG_PLATFORM_WINDOWS

#include <windows.h>

int WINAPI DllMain( HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved )
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:

		break;

	case DLL_PROCESS_DETACH:

		break;
	}


	return TRUE;
}

#endif //LOG_PLATFORM_WINDOWS
