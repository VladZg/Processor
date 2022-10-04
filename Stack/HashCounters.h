#ifndef HASHCOUNTERS_H
#define HASHCOUNTERS_H

#include <stdint.h>

uint32_t MurHash(const void* obj, uint32_t len, uint32_t seed);

#endif
