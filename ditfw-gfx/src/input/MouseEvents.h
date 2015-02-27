/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "InputDevice.h"

namespace dit {

    //-----------------------------------//

    struct MouseInfo;

    //-----------------------------------//

    enum struct MouseButton : uint8
    {
        // Main mouse buttons
        Left
        , Right
        , Middle
        , Mouse4
        , Mouse5
        , MAX
    };

    //-----------------------------------//

    enum struct MouseEventType : uint8
    {
        // Occurs when the mouse wheel is scrolled
        WheelMove
        , MouseMove
        , MouseDrag
        , ButtonPress
        , ButtonRelease
        , MouseEnter
        , MouseExit
    };

    //-----------------------------------//

    /**
     * Generic mouse event that will be inherited by the more specific
     * mouse event types. This will be used for communication between
     * the input manager, so it has private members and then adds friend
     * access to the Mouse class. That way the private data is hidden to
     * the public.
     */

    struct MouseEvent : public InputEvent
    {
        friend class Mouse;

        MouseEvent(MouseEventType eventType);

        MouseEventType eventType;
    };

    //-----------------------------------//

    /**
     * Mouse event that occurs when the mouse is moved and provides the
     * current mouse position on the screen in screen-coordinates.
     */

    struct MouseMoveEvent : public MouseEvent
    {
        MouseMoveEvent();

        int16 x;
        int16 y;
    };

    //-----------------------------------//

    /**
     * Mouse event that occurs when the mouse is dragged. Use MouseInfo
     * to get more information about which mouse buttons are pressed.
     */

    struct MouseDragEvent : public MouseEvent
    {
        MouseDragEvent();

        int16 dx;
        int16 dy;

        int16 x;
        int16 y;

        MouseInfo* info;
    };

    //-----------------------------------//

    /**
     * Mouse event that occurs when a button of the mouse is pressed.
     * It holds the button that was pressed and also the position of the
     * mouse in the window at the time of the click.
     */

    struct MouseButtonEvent : public MouseEvent
    {
        MouseButtonEvent(MouseEventType eventType);

        // Convenience button test methods.
        bool IsLeftButton() const { return button == MouseButton::Left; }
        bool IsRightButton() const { return button == MouseButton::Right; }
        bool IsMiddleButton() const { return button == MouseButton::Middle; }

        int16 x;
        int16 y;

        MouseButton button;
    };

    //-----------------------------------//

    /**
     * Mouse events that occurs when the mouse wheel is scrolled.
     * In this case the extra information is a delta value that specifies
     * the relative amount of the mouse wheel that was scrolled.
     */

    struct MouseWheelEvent : public MouseEvent
    {
        MouseWheelEvent();

        int16 delta;
    };

    //-----------------------------------//

}