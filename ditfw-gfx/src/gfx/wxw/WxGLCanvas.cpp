/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "WxGLCanvas.h"
#include "WxWindow.h"
#include "WxInputBackend.h"

namespace dit {

    //-----------------------------------//

    enum class Timers : uint32
    {
        Update = wxID_HIGHEST + 321
        , Render
    };

    BEGIN_EVENT_TABLE(WxGLCanvas, wxGLCanvas)
        EVT_TIMER((uint32) Timers::Update, WxGLCanvas::OnUpdate)
        EVT_TIMER((uint32)Timers::Render, WxGLCanvas::OnRender)
        EVT_PAINT(WxGLCanvas::OnPaint)
    END_EVENT_TABLE()

    //-----------------------------------//

    WxGLCanvas::WxGLCanvas(WxWindow* window_, wxWindow* parent, wxWindowID id, const int* attribList, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
        : wxGLCanvas(parent, id, attribList, pos, size, style | (wxFULL_REPAINT_ON_RESIZE & (~wxCLIP_CHILDREN)), name)
        , window(window_)
        , updateTimer(this, (uint32)Timers::Update)
        , renderTimer(this, (uint32)Timers::Render)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);

        updateTimer.Start(maxUpdateHz);
        renderTimer.Start(maxRenderHz);
    }

    //-----------------------------------//

    WxGLCanvas::~WxGLCanvas()
    {
        renderTimer.Stop();
        updateTimer.Stop();
    }

    //-----------------------------------//

    void WxGLCanvas::OnUpdate(wxTimerEvent& event)
    {
        if (window)
            window->HandleUpdate();
    }

    //-----------------------------------//
    
    void WxGLCanvas::OnRender(wxTimerEvent& event)
    {
        Refresh();
    }

    //-----------------------------------//

    void WxGLCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        // From the PaintEvent docs: "the application must always create
        // a wxPaintDC object, even if you do not use it."
        // http://docs.wxwidgets.org/trunk/classwx_paint_event.html
        wxPaintDC dc(this);

        if (window)
            window->HandleDraw();

        SwapBuffers();
    }

    //-----------------------------------//

}