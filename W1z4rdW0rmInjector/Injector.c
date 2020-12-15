#include <stdlib.h>
#include <string.h>

#include "Injector.h"
#include "Config.h"

bool check_in_folder_of_dll()
{
	TCHAR* filename = DLL_NAME;

	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(filename, &FindFileData);

	if (handle == INVALID_HANDLE_VALUE) {
		return false;
	}

	return true;
}
