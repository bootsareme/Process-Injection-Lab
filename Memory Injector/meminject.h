#pragma once

#include "memscan.h"
#include <vector>

using byte = char;

namespace MemoryInjector
{
	std::vector<byte> get_bytes_to_inject();
	std::vector<memaddr_t> get_addresses_to_inject_into(std::vector<memaddr_t> candidates);
	void inject(HANDLE process, std::vector<byte> bytes, std::vector<memaddr_t> addrs);
}