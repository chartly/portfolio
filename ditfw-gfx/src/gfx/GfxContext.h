/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"

namespace dit {

    //-----------------------------------//

    class GfxContext
    {
    public:
        GfxContext() = default;
        GfxContext(void* data) : userData(data) {}
        virtual ~GfxContext() {}

        virtual void MakeCurrent() = 0;

    public:
        void* userData = nullptr;
    };

    //-----------------------------------//

}