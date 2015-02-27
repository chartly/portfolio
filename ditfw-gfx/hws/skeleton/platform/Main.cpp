/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "App.h"

namespace boost {
    void throw_exception(std::exception const & e)
    {
        throw e;
    }
}

//-----------------------------------//

using dit::App;
IMPLEMENT_APP_CONSOLE(App)

//-----------------------------------//