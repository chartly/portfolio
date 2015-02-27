/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "API.h"
#include "WxDriver.h"

namespace dit {

    //-----------------------------------//

    bool WxDriver::OnInit()
    {
        if (!wxApp::OnInit())
            return false;

        wxInitAllImageHandlers();

        core = new Core{};
        core->Init();
        //core->platform->wxApp->Show();

        return true;
    }

    //-----------------------------------//

    void WxDriver::CleanUp()
    {
        core->Shutdown();

        delete core;
        core = nullptr;
    }

    //-----------------------------------//

}