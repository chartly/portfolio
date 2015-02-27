/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"

namespace dit {

    //-----------------------------------//

    class WxDriver : public wxApp
    {
    public:
        Core* core = nullptr;

    public:
        virtual bool OnInit();
        virtual void CleanUp();
    };

    DECLARE_APP(WxDriver)

    //-----------------------------------//
}