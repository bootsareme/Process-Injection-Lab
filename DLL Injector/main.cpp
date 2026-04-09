#include <iostream>
#include <windows.h>
#include "Injector.h"

static constexpr const char *DLL_PATH = "DLL Payload.dll";

int main()
{
    std::wstring process_name;
    std::cout << "Enter target process (e.g. victim.exe): ";
    std::wcin >> process_name;

    DWORD pid = FindProcessID(process_name);
    if (pid == 0)
    {
        std::cout << "[!] Process not found\n";
        return 1;
    }
    std::cout << "[+] Found PID: " << pid << "\n";

    if (InjectDLL(pid, DLL_PATH))
        std::cout << "[+] Injection successful\n";
    else
        std::cout << "[!] Injection failed\n";

    return 0;
}