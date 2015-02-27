/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "API.h"
#include "Core.h"
#include "Window.h"

namespace dit {

    //-----------------------------------//

    Window::Window(const WindowSettings& settings)
        : settings(settings)
    {
        log("creating window (size: %0x%1, title: '%2')", settings.width, settings.height, settings.title.c_str());
    }

    //-----------------------------------//

    void Window::HandleDraw()
    {
        for (const auto& fn : onDrawEvent)
            fn();
    }

    //-----------------------------------//

    void Window::HandleUpdate()
    {
        for (const auto& fn : onUpdateEvent)
            fn();
    }

    //-----------------------------------//

    void Window::HandleResize()
    {
        for (const auto& fn : onResizeEvent)
            fn(settings.width, settings.height);
    }

    //-----------------------------------//

    void Window::HandleClose()
    {
        for (const auto& fn : onCloseEvent)
            fn();
    }

    //-----------------------------------//

    void Window::HandleFocus(bool focusLost)
    {
        for (const auto& fn : onFocusEvent)
            fn(focusLost);
    }

    //-----------------------------------//

}