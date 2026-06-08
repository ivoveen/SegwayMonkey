#include "xorshift32.h"
#include <cstdint>

uint32_t RandomUInt(uint32_t& seed)
{
	seed ^= seed << 13, seed ^= seed >> 17;
	seed ^= seed << 5; return seed;
}