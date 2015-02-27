/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "Event.h"
#include "ConcurrentQueue.h"

namespace dit {

    //-----------------------------------//

    class EventPump
    {
    public:
        EventMap* map = nullptr;
        ConcurrentQueue<VoidEvent*> events;

    public:
        void Update()
        {
            VoidEvent* e = nullptr;
            while (events.try_pop(e))
            {
                if (map)
                    map->Post(e);

                delete e;
            }
        }
    };

    //-----------------------------------//

    void send(EventPump& q, const String& key);

    template <typename T>
    void send(EventPump& q, const String& key, T a0)
    {
        typedef Event<T> Event;

        uint64 hash = MurmurHash64(key.c_str(), key.size(), 0);
        auto e = new Event{ hash, a0 };

        q.events.push(e);
    }

    template <typename A0, typename... Args>
    void send(EventPump& q, const String& key, A0 a0, Args... args)
    {
        typedef Event<A0, Args...> Event;

        uint64 hash = MurmurHash64(key.c_str(), key.size(), 0);
        auto e = new Event{ hash, a0, args... };

        q.events.push(e);
    }

}