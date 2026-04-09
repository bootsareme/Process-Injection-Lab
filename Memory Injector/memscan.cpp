#include "memscan.h"
#include <iostream>

template <typename T>
static T read_value_from_input()
{
	T value;
	std::cout << "Enter the value you want to find in decimal: ";
	std::cin >> std::dec >> value;
	return value;
}

static HANDLE find_proc_handle(std::wstring process_name)
{
	HANDLE proc_snapshot = 0;
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	proc_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	do {
		if (wcscmp(pe32.szExeFile, process_name.c_str()) == 0)
		{
			HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, true, pe32.th32ProcessID);
			return process;
		}
	} while (Process32Next(proc_snapshot, &pe32));

	return NULL;
}

HANDLE MemScan::get_target_proc()
{
	std::cout << "Enter your target executable name (for example, victim.exe): ";
	std::wstring process_name;
	std::wcin >> process_name;
	HANDLE process = find_proc_handle(process_name);

	if (process == NULL)
	{
		std::wcout << "Are you sure " << process_name << " is running?\n";
		throw std::exception("process not running");
	}

	std::wcout << "Found " << process_name << " running and established privileged handle to it...\n\n";
	return process;
}

AddrRange MemScan::get_range_to_search()
{
	std::cout << "Now find the range of addresses where you suspect your target lies in (you might want to do some initial reversing)\n";
	memaddr_t lowest, highest;
	std::cout << "Enter the lowest address in your range in hex (for example, 0xDEADBEEF): 0x";
	std::cin >> std::hex >> lowest;
	std::cout << "Enter the highest address in your range in hex (for example, 0xFEEDBEEF): 0x";
	std::cin >> std::hex >> highest;

	if (lowest > highest)
		throw std::runtime_error("Lowest address cannot be greater than highest address!\n");

	std::cout << "Preparing to search " << highest - lowest << " bytes of memory...\n\n";
	return { lowest, highest };
}

std::string MemScan::get_type_of_value()
{
	std::cout << "Enter the suspected type of your value (char/short/int/long/longlong/float/double): ";
	std::string type;
	std::cin >> type;
	return type;
}

std::vector<memaddr_t> MemScan::parse_type_and_search(HANDLE process, std::string type, memaddr_t lowest, memaddr_t highest)
{
	if (type == "short" || type == "int" || type == "long" || type == "longlong")
	{
		char is_unsigned;
		std::cout << "Do you suspect your type is unsigned (y/n)? ";
		std::cin >> is_unsigned;

		if (is_unsigned == 'y')
		{
			if (type == "short")
				return MemScan::search(process, read_value_from_input<unsigned short>(), lowest, highest);
			else if (type == "int")
				return MemScan::search(process, read_value_from_input<unsigned int>(), lowest, highest);
			else if (type == "long")
				return MemScan::search(process, read_value_from_input<unsigned long>(), lowest, highest);
			else if (type == "longlong")
				return MemScan::search(process, read_value_from_input<unsigned long long>(), lowest, highest);
			else
				throw std::exception("This type does not have an signed variant!\n");
		}
		else
		{
			if (type == "short")
				return MemScan::search(process, read_value_from_input<short>(), lowest, highest);
			else if (type == "int")
				return MemScan::search(process, read_value_from_input<int>(), lowest, highest);
			else if (type == "long")
				return MemScan::search(process, read_value_from_input<long>(), lowest, highest);
			else if (type == "longlong")
				return MemScan::search(process, read_value_from_input<long long>(), lowest, highest);
			else
				throw std::exception("This type does not have an signed variant!\n");
		}
	}
	else if (type == "char")
		return MemScan::search(process, read_value_from_input<char>(), lowest, highest);
	else if (type == "float")
		return MemScan::search(process, read_value_from_input<float>(), lowest, highest);
	else if (type == "double")
		return MemScan::search(process, read_value_from_input<double>(), lowest, highest);
	else
		throw std::exception("Only primitive non-array types are supported!\n");
}

bool MemScan::display_all_occurrences(std::vector<memaddr_t> occurrences)
{
	if (occurrences.empty())
	{
		std::cout << "\nYour value was not found :(\n";
		return false;
	}

	std::cout << "\nFound " << occurrences.size() << " occurrences of your value in your specified memory range:\n";
	for (int i = 0; i < occurrences.size(); i++)
		std::cout << std::dec << "\t@ [" << i << "] 0x" << std::hex << std::uppercase << occurrences[i] << std::endl;

	return true;
}