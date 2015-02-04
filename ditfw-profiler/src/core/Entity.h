/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "EventPump.h"

namespace dit {

    //-----------------------------------//

    class Entity
    {
    public:
        Entity() = default;
        virtual ~Entity() = default;

        virtual void Init() {}
        virtual void Update(double dt)
        {
            events.Update();
        }
        virtual void Shutdown() {}

        void Activate(EventMap* map)
        {
            events.map = map;
        }

    public:
        EventPump events;
    };

    //-----------------------------------//

}