/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "API.h"
#include "Core.h"
#include "Mouse.h"

namespace dit {

    //-----------------------------------//

#define CAST_EVENT(type, var, evt)		\
    type var = static_cast<type>(evt);

    void Mouse::ProcessEvent(const InputEvent& event)
    {
        if (event.deviceType != InputDeviceType::Mouse)
            return;

        CAST_EVENT(const MouseEvent&, mevt, event)

            switch (mevt.eventType)
        {
            case MouseEventType::ButtonPress:
            {
                CAST_EVENT(const MouseButtonEvent&, mbe, mevt)
                    OnButtonPressed(mbe);
                break;
            }

            case MouseEventType::ButtonRelease:
            {
                CAST_EVENT(const MouseButtonEvent&, mbe, mevt)
                    OnButtonReleased(mbe);
                break;
            }

            case MouseEventType::MouseMove:
            {
                CAST_EVENT(const MouseMoveEvent&, mm, mevt)
                    OnMoved(mm);
                break;
            }

            case MouseEventType::MouseDrag:
            {
                CAST_EVENT(const MouseDragEvent&, mde, mevt)
                    OnDragged(mde);
                break;
            }

            case MouseEventType::WheelMove:
            {
                CAST_EVENT(const MouseWheelEvent&, mwe, mevt)
                    OnWheelMove(mwe);
                break;
            }

            case MouseEventType::MouseEnter:
            {
                OnEnter();
                break;
            }

            case MouseEventType::MouseExit:
            {
                OnExit();
                break;
            }
        }
    }

    //-----------------------------------//

    bool Mouse::IsButtonPressed(MouseButton button) const
    {
        switch (button)
        {
        case MouseButton::Left:
            return state.leftButton;
        case MouseButton::Right:
            return state.rightButton;
        case MouseButton::Middle:
            return state.middleButton;
        case MouseButton::Mouse4:
            return state.aux1Button;
        case MouseButton::Mouse5:
            return state.aux2Button;
        }

        log("could not map enum: Invalid mouse button");
        return false;
    }

    //-----------------------------------//

    void Mouse::OnMoved(const MouseMoveEvent& event)
    {
        state.x = event.x;
        state.y = event.y;

        for (const auto& fn : onMouseMove)
            fn(event);
    }

    //-----------------------------------//

    void Mouse::OnDragged(const MouseDragEvent& de)
    {
        MouseDragEvent& event = (MouseDragEvent&) de;

        event.dx = state.x - event.x;
        event.dy = state.y - event.y;
        event.info = &state;

        state.x = event.x;
        state.y = event.y;

        for (const auto& fn : onMouseDrag)
            fn(event);
    }

    //-----------------------------------//

    void Mouse::OnButtonPressed(const MouseButtonEvent& mbp)
    {
        state.x = mbp.x;
        state.y = mbp.y;

        SetButton(mbp.button, true);
        for (const auto& fn : onMouseButtonPress)
            fn(mbp);
    }

    //-----------------------------------//

    void Mouse::OnButtonReleased(const MouseButtonEvent& mbe)
    {
        state.x = mbe.x;
        state.y = mbe.y;

        SetButton(mbe.button, false);
        for (const auto& fn : onMouseButtonRelease)
            fn(mbe);
    }

    //-----------------------------------//

    void Mouse::OnEnter()
    {
        state.insideWindow = true;
        for (const auto& fn : onMouseEnter)
            fn();
    }

    //-----------------------------------//

    void Mouse::OnExit()
    {
        state.insideWindow = false;
        for (const auto& fn : onMouseExit)
            fn();
    }

    //-----------------------------------//

    void Mouse::OnWheelMove(const MouseWheelEvent& mevt)
    {
        for (const auto& fn : onMouseWheelMove)
            fn(mevt);
    }

    //-----------------------------------//

    void Mouse::SetButton(MouseButton button, bool clicked)
    {
        switch (button)
        {
        case MouseButton::Left:
            state.leftButton = clicked;
            break;

        case MouseButton::Right:
            state.rightButton = clicked;
            break;

        case MouseButton::Middle:
            state.middleButton = clicked;
            break;

        case MouseButton::Mouse4:
            state.aux1Button = clicked;
            break;

        case MouseButton::Mouse5:
            state.aux2Button = clicked;
            break;
        }
    }

    //-----------------------------------//

    MouseInfo::MouseInfo()
        : x(0), y(0)
        , leftButton(false)
        , rightButton(false)
        , middleButton(false)
        , aux1Button(false)
        , aux2Button(false)
        , insideWindow(false)
    { }

    //-----------------------------------//

    MouseEvent::MouseEvent(MouseEventType eventType)
        : InputEvent(InputDeviceType::Mouse)
        , eventType(eventType)
    { }

    //-----------------------------------//

    MouseMoveEvent::MouseMoveEvent()
        : MouseEvent(MouseEventType::MouseMove)
    { }

    //-----------------------------------//

    MouseDragEvent::MouseDragEvent()
        : MouseEvent(MouseEventType::MouseDrag)
    { }

    //-----------------------------------//

    MouseButtonEvent::MouseButtonEvent(MouseEventType eventType)
        : MouseEvent(eventType)
    { }

    //-----------------------------------//

    MouseWheelEvent::MouseWheelEvent()
        : MouseEvent(MouseEventType::WheelMove)
    { }

    //-----------------------------------//

}