#include "../Include/Config.h"
#include <stdint.h>
#include "../Include/Defines.h"

uint32_t MurHash(const void* obj, uint32_t len, uint32_t seed)
{
    ASSERT(obj != NULL);

    static const uint32_t c1 = 0xcc9e2d51;
    static const uint32_t c2 = 0x1b873593;
    static const uint32_t r1 = 15;
    // static const uint32_t r2 = 13;
    // static const uint32_t m = 5;
    // static const uint32_t n = 0xe6546b64;

    uint8_t hash = seed;
    int n_blocks = len / 4;
    const uint32_t* blocks_start = (const uint32_t*) obj;

    for (int i = 0; i < n_blocks; i++)
    {
        uint32_t k = blocks_start[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        // hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
    }

    const uint8_t *tail = (const uint8_t *) ((const uint32_t*) obj + n_blocks * 4);
    uint32_t k1 = 0;

    switch (len & 3)
    {
        case 3:
            k1 ^= tail[2] << 16;

        case 2:
            k1 ^= tail[1] << 8;

        case 1:
            k1 ^= tail[0];
    }

    k1 *= c1;
    k1 = (k1 << r1) | (k1 >> (32 - r1));
    k1 *= c2;
    hash ^= k1;

    hash ^= len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}
