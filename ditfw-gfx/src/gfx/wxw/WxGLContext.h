/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "GfxContext.h"

//-----------------------------------//
// Forward decls

class wxGLContext;
class wxGLCanvas;

//-----------------------------------//

namespace dit {

    //-----------------------------------//
    // Forward decls

    class WxWindow;

    //-----------------------------------//

    class WxGLContext : public GfxContext
    {
    public:

        WxGLContext(wxGLCanvas* canvas, WxWindow* window);
        virtual ~WxGLContext();

        void MakeCurrent();

    public:
        wxGLContext* glCtx = nullptr;
    };

    //-----------------------------------//
}