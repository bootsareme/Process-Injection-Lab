#include "meminject.h"
#include <iostream>
#include <sstream>
#include <limits>

#undef max
#undef min

static inline std::string trim(const std::string& s) {
	auto start = s.find_first_not_of(" \t\n\r");
	auto end = s.find_last_not_of(" \t\n\r");
	return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

std::vector<byte> MemoryInjector::get_bytes_to_inject()
{
	std::cout << "\nEnter the raw bytes you want to inject in little endian, with dashes in between (ex. 67 = 43-00-00-00): ";
    std::string line;
    std::cin >> line;

    std::vector<byte> bytes;
    std::stringstream ss(line);
    std::string token;

    while (std::getline(ss, token, '-'))
    {
        if (token.size() != 2)
            throw std::runtime_error("Each byte must be exactly two hex digits.");

        unsigned int value = 0;
        std::stringstream conv;
        conv << std::hex << token;
        conv >> value;
        bytes.push_back(static_cast<byte>(value));
    }

    return bytes;
}

std::vector<memaddr_t> MemoryInjector::get_addresses_to_inject_into(std::vector<memaddr_t> candidates)
{
    std::cin.clear();  // clear failbit
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Enter the index of the addresses (denoted by the [#]) to inject your bytes into or type 'all' to inject into every address (very likely to crash): ";
    std::string line;
    std::getline(std::cin, line);
    line = trim(line);

    if (line == "all" || line == "ALL") 
        return candidates;  // return a copy of the full array

    std::vector<memaddr_t> result;
    std::stringstream ss(line);
    std::string token;

    while (std::getline(ss, token, ',')) 
    {
        token = trim(token);
        try 
        {
            size_t idx = std::stoul(token);
            if (idx < candidates.size()) 
                result.push_back(candidates[idx]);
            else
                std::cerr << "Warning: index " << idx << " out of bounds, skipping.\n";
        }
        catch (const std::exception&) 
        {
            std::cerr << "Warning: invalid input '" << token << "', skipping.\n";
            throw std::runtime_error("invalid bytes");
        }
    }

    return result;
}

void MemoryInjector::inject(HANDLE process, std::vector<byte> bytes, std::vector<memaddr_t> addrs)
{
    for (memaddr_t addr : addrs)
    {
        if (!WriteProcessMemory(process, reinterpret_cast<LPVOID>(addr), bytes.data(), bytes.size(), nullptr))
        {
            std::cerr << "Failed to inject your bytes at address 0x" << std::hex << addr << "\n";
            return;
        }
        std::cout << "Injecting " << bytes.size() << " bytes at address 0x" << std::hex << addr << " succeeded\n";
    }
}
