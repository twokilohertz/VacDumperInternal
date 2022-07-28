#pragma once

#include <cstdint>

void* find_pattern(const uint8_t* pattern, size_t pattern_size, uintptr_t start_addr, size_t addr_range);
bool write_byte(void* address, uint8_t byte);