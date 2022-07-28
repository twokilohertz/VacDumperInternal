// mem.cpp

#include "mem.h"
#include <Windows.h>

void* find_pattern(const uint8_t* pattern, size_t pattern_size, uintptr_t start_addr, size_t addr_range)
{
	bool		found		= false;
	uintptr_t	curr_addr	= start_addr;

	// While not at the end of search area and the pattern has not been found
	while (curr_addr <= start_addr + addr_range - 1 && !found)
	{
		// Iterate over each byte in pattern
		for (int i = 0; i < pattern_size; i++)
		{
			// If match ...
			if (*(uint8_t*)(curr_addr + i) == pattern[i])
			{
				found = true;
				continue;
			}
			else // Otherwise, break out of loop
			{
				found = false;
				break;
			}
		}

		// Break out of while loop if found
		if (found) break;

		// ... move along to next byte
		curr_addr++;
	}

	// Failed to find pattern, return null pointer
	if (!found) return nullptr;

	return reinterpret_cast<void*>(curr_addr);
}

bool write_byte(void* address, uint8_t byte) // TODO: Error checking
{
	SYSTEM_INFO sysInfo = { 0 };
	GetSystemInfo(&sysInfo);

	DWORD oldProt = NULL;
	VirtualProtect(address, sysInfo.dwPageSize, PAGE_EXECUTE_READWRITE, &oldProt);

	*(uint8_t*)address = byte;

	VirtualProtect(address, sysInfo.dwPageSize, oldProt, &oldProt);

	return true;
}
