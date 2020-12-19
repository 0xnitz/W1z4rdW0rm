#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <TlHelp32.h>

#include "Injector.h"
#include "Config.h"

bool file_exists(const char filename[])
{
	FILE* f = NULL;
	fopen_s(&f, filename, "r");

	return f != NULL;
}

bool check_in_folder_of_dll()
{
	char filename[] = DLL_NAME;

	return file_exists(filename);
}

HANDLE find_process()
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	Process32First(snapshot, &processEntry);
	while (Process32Next(snapshot, &processEntry))
	{
		if (wcscmp(processEntry.szExeFile, L"explorer.exe") == 0)
			return OpenProcess(MAXIMUM_ALLOWED, false, processEntry.th32ProcessID);
		
		/*if (is_process_good(processEntry))
			return OpenProcess(MAXIMUM_ALLOWED, false, processEntry.th32ProcessID);*/
	}

	return NULL;
}

bool is_process_good(PROCESSENTRY32 processEntry)
{
	MEMORY_BASIC_INFORMATION mbi;
	LPVOID offset = 0;
	HANDLE process = OpenProcess(MAXIMUM_ALLOWED, false, processEntry.th32ProcessID);

	if (process)
	{
		printf("Exe file -> %ls\n", processEntry.szExeFile);
		while (VirtualQueryEx(process, offset, &mbi, sizeof(mbi)))
		{
			offset = (LPVOID)((DWORD_PTR)mbi.BaseAddress + mbi.RegionSize);
			if (mbi.AllocationProtect == PAGE_EXECUTE_READWRITE && mbi.State == MEM_COMMIT && mbi.Type == MEM_PRIVATE)
			{
				printf("\tRWX: %p %lx\n", mbi.BaseAddress, mbi.RegionSize);

				if (mbi.RegionSize > 0x1000) {
					CloseHandle(process);
					return true;
				}
			}
		}
		CloseHandle(process);
	}

	return false;
}

bool inject_dll_into_process()
{
	char dll_name[] = DLL_NAME;

	HANDLE process_to_inject = find_process();

	if (process_to_inject == NULL)
		return false;

	LPVOID addr = VirtualAllocEx(process_to_inject, NULL, sizeof dll_name, MEM_COMMIT, PAGE_READWRITE);

	if (addr == NULL)
		return false;

	WriteProcessMemory(process_to_inject, addr, dll_name, sizeof dll_name, NULL);

	PTHREAD_START_ROUTINE thread_start_routine = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "LoadLiraryA");

	CreateRemoteThread(process_to_inject, NULL, NULL, thread_start_routine, NULL, NULL, NULL);

	CloseHandle(process_to_inject);

	return true;
}
