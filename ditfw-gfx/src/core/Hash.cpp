/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "Hash.h"

namespace dit {

    //-----------------------------------------------------------------------------
    // MurmurHash2, by Austin Appleby

    // Note - This code makes a few assumptions about how your machine behaves -

    // 1. We can read a 4-byte value from any address without crashing
    // 2. sizeof(int) == 4

    // And it has a few limitations -

    // 1. It will not work incrementally.
    // 2. It will not produce the same results on little-endian and big-endian
    //    machines.

    uint32 MurmurHash32(const void* key, uint32 size, uint32 seed)
    {
        if (!key || size == 0) return 0;

        // 'm' and 'r' are mixing constants generated offline.
        // They're not really 'magic', they just happen to work well.

        const unsigned int m = 0x5bd1e995;
        const int r = 24;

        // Initialize the hash to a 'random' value

        unsigned int h = seed ^ size;

        auto data = (const char *) key;

        // Mix 4 bytes at a time into the hash

        while (size >= 4)
        {
            unsigned int k = *(unsigned int *) key;

            k *= m;
            k ^= k >> r;
            k *= m;

            h *= m;
            h ^= k;

            data += 4;
            size -= 4;
        }

        // Handle the last few bytes of the input array

        switch (size)
        {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
            h *= m;
        };

        // Do a few final mixes of the hash to ensure the last few
        // bytes are well-incorporated.

        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        return h;
    }

    //-----------------------------------//

    uint64 MurmurHash64(const void* key, uint64 len, uint64 seed)
    {
        const uint64 m = 0xc6a4a7935bd1e995ULL;
        const uint32 r = 47;

        uint64 h = seed ^ (len * m);

        const uint64 * data = (const uint64 *) key;
        const uint64 * end = data + (len / 8);

        while (data != end)
        {
#ifdef PLATFORM_BIG_ENDIAN
            uint64 k = *data++;
            char *p = (char *)&k;
            char c;
            c = p[0]; p[0] = p[7]; p[7] = c;
            c = p[1]; p[1] = p[6]; p[6] = c;
            c = p[2]; p[2] = p[5]; p[5] = c;
            c = p[3]; p[3] = p[4]; p[4] = c;
#else
            uint64 k = *data++;
#endif

            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;
        }

        const unsigned char * data2 = (const unsigned char*) data;

        switch (len & 7)
        {
        case 7: h ^= uint64(data2[6]) << 48;
        case 6: h ^= uint64(data2[5]) << 40;
        case 5: h ^= uint64(data2[4]) << 32;
        case 4: h ^= uint64(data2[3]) << 24;
        case 3: h ^= uint64(data2[2]) << 16;
        case 2: h ^= uint64(data2[1]) << 8;
        case 1: h ^= uint64(data2[0]);
            h *= m;
        };

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;
    }
}