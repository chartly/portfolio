/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"

#include <wx/wx.h>
#include <wx/glcanvas.h>

namespace dit {

    //-----------------------------------//

    class WxWindow;
    class WxInputBackend;

    //-----------------------------------//

    class WxGLCanvas : public wxGLCanvas
    {
    public:
        WxGLCanvas(WxWindow* window
            , wxWindow* parent
            , wxWindowID id = wxID_ANY
            , const int* attribList = nullptr
            , const wxPoint& pos = wxDefaultPosition
            , const wxSize& size = wxDefaultSize
            , long style = wxFULL_REPAINT_ON_RESIZE | wxBORDER_NONE
            , const wxString& name = wxGLCanvasName);

        ~WxGLCanvas();

    public:
        void OnUpdate(wxTimerEvent& event);
        void OnRender(wxTimerEvent& event);
        void OnPaint(wxPaintEvent& event);

    public:
        WxWindow* window = nullptr;
        wxTimer updateTimer;
        wxTimer renderTimer;

        float maxUpdateHz = 1.f / 60.f;
        float maxRenderHz = 1.f / 60.f;

        DECLARE_EVENT_TABLE()
    };

    //-----------------------------------//
}