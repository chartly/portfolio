/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "StringExt.h"
#include "GfxContext.h"

#include <functional>
#include <array>
#include <vector>

namespace dit {

    //-----------------------------------//

    class InputBackend;
    typedef void* NativeWindowHandle;

    //-----------------------------------//

    enum class WindowStyles : uint32
    {
        None = 0
        , TopLevel = (1 << 1)     /// A top-level window is a window that is not a child.
        , MiniFrame = (1 << 2)    /// A miniframe is a window with minimal chrome.
        , Borderless = (1 << 3)   /// A borderless window has no chrome.
        , Fullscreen = (1 << 4)   /// Fullscreen windows take the full space of the screen.
    };

    //-----------------------------------//

    struct WindowSettings
    {
        WindowSettings() = default;
        WindowSettings(uint16 _width, uint16 _height, String _title, NativeWindowHandle _handle, WindowStyles _styles = WindowStyles::None)
            : width(_width), height(_height), styles(_styles), title(std::move(_title)), handle(_handle)
        {}

        uint16 width = 1280;
        uint16 height = 720;
        WindowStyles styles = WindowStyles::None;
        NativeWindowHandle handle = nullptr;
        String title = "";
    };

    //-----------------------------------//

    /**
     * Represents onscreen display areas in the target window system.
     */
    class Window : public GfxContext
    {
    public:
        typedef std::function<void(void)> DrawEventConsumer;
        typedef std::function<void(void)> UpdateEventConsumer;
        typedef std::function<void(uint16, uint16)> ResizeEventConsumer;
        typedef std::function<void(void)> CloseEventConsumer;
        typedef std::function<void(bool)> FocusEventConsumer;

    public:
        Window(const WindowSettings& settings);

        virtual GfxContext* CreateContext() = 0;

        virtual void Update()                               {}
        virtual void MakeCurrent()                          {}
        virtual void Show(bool visible = true)              {}
        virtual bool PumpEvents()                           { return true; }
        virtual void SetTitle(const String& title)          {}
        virtual void SetCursorVisible(bool state)           {}
        virtual bool IsCursorVisible() const                { return false; }
        virtual void SetCursorCapture(bool state)           {}
        std::array<int32, 2> GetCursorPosition() const      { return{ { 0, 0 } }; }
        virtual void SetCursorPosition(int32 x, int32 y)    {}
        virtual bool HasFocus() const                       { return false; }
        virtual InputBackend* GetInput()                    { return nullptr; }

    public:
        void HandleDraw();
        void HandleUpdate();
        void HandleResize();
        void HandleClose();
        void HandleFocus(bool focusLost);

    public:
        WindowSettings settings;
        GfxContext* ctx;

        std::vector<DrawEventConsumer> onDrawEvent;
        std::vector<UpdateEventConsumer> onUpdateEvent;
        std::vector<ResizeEventConsumer> onResizeEvent;
        std::vector<CloseEventConsumer> onCloseEvent;
        std::vector<FocusEventConsumer> onFocusEvent;
    };

    //-----------------------------------//

}