#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <TlHelp32.h>
#include <string>
#include <vector>

using memaddr_t = uintptr_t;

struct AddrRange
{
	memaddr_t start;
	memaddr_t end;
};

namespace MemScan 
{
	/*
	Given a process handle, a target value, and a range to search,
	these functions return a list of memory addresses where the value begins.
	If the value spans multiple bytes, the returned address is the lowest (starting) address.
	*/
	template <typename T>
	std::vector<memaddr_t> search(HANDLE process, T value, memaddr_t start, memaddr_t stop)
	{
		std::vector<memaddr_t> results;
		SIZE_T bytes_read;
		T buffer;

		for (memaddr_t addr = start; addr < stop; ++addr)
			if (ReadProcessMemory(process, reinterpret_cast<LPCVOID>(addr), &buffer, sizeof(T), &bytes_read))
				if (bytes_read == sizeof(T) && buffer == value)
					results.push_back(addr);

		return results;
	}

	HANDLE get_target_proc();
	AddrRange get_range_to_search();
	std::string get_type_of_value();
	std::vector<memaddr_t> parse_type_and_search(HANDLE process, std::string type, memaddr_t lowest, memaddr_t highest);
	bool display_all_occurrences(std::vector<memaddr_t> occurrences);
}
