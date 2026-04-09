#include "memscan.h"
#include "meminject.h"

int main()
{
	HANDLE process = MemScan::get_target_proc();
	struct AddrRange range = MemScan::get_range_to_search();
	std::string type = MemScan::get_type_of_value();
	std::vector<memaddr_t> occurrences = MemScan::parse_type_and_search(process, type, range.start, range.end);

	if (!MemScan::display_all_occurrences(occurrences))
		return 3;
	
	std::vector<byte> bytes = MemoryInjector::get_bytes_to_inject();
	std::vector<memaddr_t> addrs = MemoryInjector::get_addresses_to_inject_into(occurrences);
	MemoryInjector::inject(process, bytes, addrs);
	CloseHandle(process);
	return 0;
}