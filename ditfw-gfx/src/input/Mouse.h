/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "InputDevice.h"
#include "MouseEvents.h"

#include <vector>
#include <functional>

namespace dit {

    //-----------------------------------//

    /**
     * Holds the mouse state.
     */

    struct MouseInfo
    {
        MouseInfo();

        // Position state.
        int16 x;
        int16 y;
        bool insideWindow;

        // Buttons state.
        bool leftButton;
        bool rightButton;
        bool middleButton;
        bool aux1Button;
        bool aux2Button;
    };

    //-----------------------------------//

    class Mouse : public InputDevice
    {
    public:
        typedef std::function <void(const MouseMoveEvent&)> MouseMoveConsumer;
        typedef std::function <void(const MouseDragEvent&)> MouseDragConsumer;
        typedef std::function <void(const MouseButtonEvent&)> MouseButtonConsumer;
        typedef std::function <void(const MouseWheelEvent&)> MouseWheelConsumer;
        typedef std::function <void(void)> MouseEventConsumer;

    public:

        virtual void ProcessEvent(const InputEvent& event);
        bool IsButtonPressed(MouseButton button) const;

    public:
        MouseInfo state;

        std::vector<MouseMoveConsumer> onMouseMove;
        std::vector<MouseDragConsumer> onMouseDrag;
        std::vector<MouseButtonConsumer> onMouseButtonPress;
        std::vector<MouseButtonConsumer> onMouseButtonRelease;
        std::vector<MouseWheelConsumer> onMouseWheelMove;
        std::vector<MouseEventConsumer> onMouseEnter;
        std::vector<MouseEventConsumer> onMouseExit;

    private:

        // Occurs when a mouse button is pressed.
        void OnButtonPressed(const MouseButtonEvent& e);

        // Occurs when a mouse button is released.
        void OnButtonReleased(const MouseButtonEvent& e);

        // Occurs when the mouse is moved.
        void OnMoved(const MouseMoveEvent& e);

        // Occurs when the mouse is dragged.
        void OnDragged(const MouseDragEvent& e);

        // Occurs when the mouse cursor enters the window.
        void OnEnter();

        // Occurs when the mouse cursor exits the window.
        void OnExit();

        // Occurs when the mouse wheel is scrolled.
        void OnWheelMove(const MouseWheelEvent& e);

        // Sets the button state in the mouse info.
        void SetButton(MouseButton button, bool clicked);
    };

    //-----------------------------------//

}