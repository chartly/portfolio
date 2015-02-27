/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "API.h"
#include "WxWindow.h"
#include "WxGLContext.h"
#include "WxGLCanvas.h"
#include "WxInputBackend.h"

namespace dit {

    //-----------------------------------//

    WxWindow::WxWindow(const WindowSettings& settings, wxWindow* _window, WxInputBackend* _inputManager)
        : Window(settings)
        , wxw(_window)
        , canvas(nullptr)
        , cursorVisible(true)
        , cursorPriority(0)
        , input(_inputManager)
    {
        userData = this;
        wxw->SetClientData(this);
        wxw->Bind(wxEVT_IDLE, &WxWindow::ProcessIdle, this);
    }

    //-----------------------------------//

    WxWindow::~WxWindow()
    {
        ctx = nullptr;
    }

    //-----------------------------------//

    GfxContext* WxWindow::CreateContext()
    {
        if (wxw == nullptr)
            return nullptr;
        if (canvas == nullptr)
            return nullptr;

        ctx = new WxGLContext{ canvas, this };
        return ctx;
    }

    //-----------------------------------//

    void WxWindow::Show(bool hide)
    {
        wxw->Show(hide);
    }

    //-----------------------------------//

    void WxWindow::MakeCurrent()
    {
        if (!ctx || !canvas)
            return;

        ((WxGLContext*) ctx)->MakeCurrent();
    }

    //-----------------------------------//

    bool WxWindow::HasFocus() const
    {
        return canvas->HasFocus();
    }

    //-----------------------------------//

    std::array<int32, 2> WxWindow::GetCursorPosition() const
    {
        const wxMouseState& mouseState = wxGetMouseState();

        int32 x = mouseState.GetX();
        int32 y = mouseState.GetY();
        canvas->ScreenToClient(&x, &y);

        return { { x, y } };
    }

    //-----------------------------------//

    void WxWindow::SetCursorPosition(int32 x, int32 y)
    {
        canvas->WarpPointer(x, y);
    }

    //-----------------------------------//

    bool WxWindow::IsCursorVisible() const
    {
        return cursorVisible;
    }

    //-----------------------------------//

    void WxWindow::SetCursorVisible(bool state)
    {
        cursorVisible = state;

        if (!cursorVisible)
            canvas->SetCursor(wxCursor(wxCURSOR_BLANK));
        else
            canvas->SetCursor(wxNullCursor);
    }

    //-----------------------------------//

    void WxWindow::SetCursorVisiblePriority(bool state, int32 priority)
    {
        if (cursorPriority > priority)
            return;

        SetCursorVisible(state);

        if (!state)
            cursorPriority = 0;
    }

    //-----------------------------------//

    void WxWindow::SetCursorCapture(bool captureMouse)
    {
        bool hasCapture = canvas->HasCapture();

        if (captureMouse)
            canvas->CaptureMouse();
        else if (hasCapture)
            canvas->ReleaseMouse();
    }

    //-----------------------------------//

    void WxWindow::ProcessResize(const wxSize& size)
    {
        settings.width = size.GetX();
        settings.height = size.GetY();

        HandleResize();
        canvas->Refresh();
    }

    //-----------------------------------//

    void WxWindow::ProcessIdle(wxIdleEvent& event)
    {
        HandleUpdate();

        // keep 'em coming
        event.RequestMore();
    }

    //-----------------------------------//

    void WxWindow::SetTitle(const String& title)
    {
        // Our canvas has no title to set, the best we can do is to
        // set the title as the help text of the wxWidgets control.
        canvas->SetHelpText(title.c_str());
    }

    //-----------------------------------//

    InputBackend* WxWindow::GetInput()
    {
        return input;
    }

    //-----------------------------------//

}