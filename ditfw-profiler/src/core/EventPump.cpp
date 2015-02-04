/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "EventPump.h"

namespace dit {

    void send(EventPump& q, const String& key)
    {
        uint64 hash = MurmurHash64(key.c_str(), key.size(), 0);
        auto e = new VoidEvent{ hash };
        q.events.push_back(e);
    }

}