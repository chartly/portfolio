/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "InputDevice.h"
#include "KeyboardEvents.h"

#include <array>
#include <vector>
#include <functional>

namespace dit {

    //-----------------------------------//

    /**
     * Keyboard device.
     */

    class Keyboard : public InputDevice
    {
    public:
        typedef std::function<void(const KeyEvent&)> KeyEventConsumer;

    public:
        Keyboard();

        virtual void ProcessEvent(const InputEvent& event);

        bool IsKeyDown(Keys keyCode) const;
        void Reset();

        std::vector<KeyEventConsumer> onKeyPress;
        std::vector<KeyEventConsumer> onKeyRelease;
        std::vector<KeyEventConsumer> onKeyText;

    private:
        void KeyPressed(const KeyEvent& keyEvent);
        void KeyReleased(const KeyEvent& keyEvent);

    public:
        std::array<bool, (uint32)Keys::MAX> keyState;
        Keys lastKey;
    };
}