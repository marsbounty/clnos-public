#pragma once
#include <Windows.h>
#include "Memory.h"
#include <TlHelp32.h>
#include <string>
#include <psapi.h>
using std::string;

class Memory
{
public:
    int GetProcessId(const std::wstring& processName);
    uintptr_t GetModuleBase(DWORD procId, const wchar_t* modName);
    BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);
    BOOL GetDebugPrivileges(void);
    int ReadInt(HANDLE processHandle, int address);
    int GetPointerAddress(HANDLE processHandle, int startAddress, int offsets[], int offsetCount);
    int ReadPointerInt(HANDLE processHandle, int startAddress, int offsets[], int offsetCount);
    float ReadFloat(HANDLE processHandle, int address);
    float ReadPointerFloat(HANDLE processHandle, int startAddress, int offsets[], int offsetCount);
    char* ReadText(HANDLE processHandle, int address);
    char* ReadPointerText(HANDLE processHandle, int startAddress, int offsets[], int offsetCount);
};
