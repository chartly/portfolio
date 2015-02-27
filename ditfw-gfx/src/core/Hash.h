/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"

namespace dit {

    // Hashes some data using the Murmur2 hash.
    // http://sites.google.com/site/murmurhash/

    uint32 MurmurHash32(const void* key, uint32 size, uint32 seed);
    uint64 MurmurHash64(const void* key, uint64 len, uint64 seed);

}