/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "Window.h"

#include <wx/wx.h>

namespace dit {

    //-----------------------------------//

    class WxGLContext;
    class WxGLCanvas;

    class WxInputBackend;

    //-----------------------------------//

    /**
     * Window implementation using the wxWidgets GUI framework (wxGLCanvas).
     * This will be used to integrate engine rendering into wxWidgets-based
     * applications, thus making it possible to do custom tools, like world
     * editors, model viewers, etc. This class only implements the Window
     * interface on the engine side. There is another class, WxRenderContext,
     * that handles the rest of the work.
     */

    class WxWindow : public Window
    {
        friend class WxGLCanvas;

    public:
        WxWindow(const WindowSettings& settings, wxWindow* window, WxInputBackend* inputManager);
        virtual ~WxWindow();

        GfxContext* CreateContext();

        void MakeCurrent();
        void Show(bool hide);

        void SetTitle(const String& title);
        void SetCursorVisible(bool state);
        void SetCursorCapture(bool state);
        void SetCursorPosition(int32 x, int32 y);
        void SetCursorVisiblePriority(bool state, int32 priority);

        bool IsCursorVisible() const;
        std::array<int32, 2> GetCursorPosition() const;
        bool HasFocus() const;

        void ProcessResize(const wxSize& size);
        void ProcessIdle(wxIdleEvent& event);

        InputBackend* GetInput();

    public:
        wxWindow* wxw                   = nullptr;
        WxInputBackend* input           = nullptr;
        WxGLCanvas* canvas              = nullptr;
        bool cursorVisible              = true;
        int32 cursorPriority            = 0;
    };

    //-----------------------------------//

}