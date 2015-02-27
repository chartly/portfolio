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

    enum class InputDeviceType : uint32
    {
        Keyboard,
        Mouse,
        Joystick
    };

    //-----------------------------------//

    struct InputEvent
    {
    public:
        InputEvent() = default;
        InputEvent(const InputEvent& rhs) = delete;
        InputEvent(InputDeviceType devType)
            : deviceType(devType)
        {}

        InputDeviceType deviceType;
    };

    //-----------------------------------//

    class InputDevice
    {
    public:
        InputDevice() = default;
        virtual ~InputDevice() {}

        InputDevice(const InputDevice& rhs) = delete;
        InputDevice& operator=(const InputDevice& rhs) = delete;

        virtual void ProcessEvent(const InputEvent& e) = 0;
    };

    //-----------------------------------//

}