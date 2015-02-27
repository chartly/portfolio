/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "InputBackend.h"
#include "Keyboard.h"
#include "Mouse.h"

#include <wx/wx.h>

namespace dit {

    //-----------------------------------//

    class WxInputBackend : public InputBackend
    {
    public:

        virtual Keyboard* GetKeyboard()     { return &keyboard; }
        virtual Mouse* GetMouse()           { return &mouse; }

        void ProcessKeyEvent(const wxKeyEvent&, bool isKeyDown);
        void ProcessCharEvent(const wxKeyEvent&);
        void ProcessMouseEvent(const wxMouseEvent&);

        Keyboard keyboard;
        Mouse mouse;
    };

    //-----------------------------------//

}