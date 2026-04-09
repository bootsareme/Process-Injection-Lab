#pragma once

#include <Windows.h>
#include <string>

DWORD FindProcessID(const std::wstring& processName);
bool InjectDLL(DWORD pid, const std::string& dllPath);
void LogError(const std::string& msg);