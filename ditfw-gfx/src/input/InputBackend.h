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

    class Keyboard;
    class Mouse;

    //-----------------------------------//

    class InputBackend
    {
    public:
        InputBackend() = default;
        virtual ~InputBackend() = default;

        virtual Keyboard* GetKeyboard() = 0;
        virtual Mouse* GetMouse() = 0;
    };

    //-----------------------------------//

}