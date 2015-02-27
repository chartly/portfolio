/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "API.h"
#include "Core.h"
#include "WxGLContext.h"
#include "WxWindow.h"

#include <gl/glew.h>
#include <gl/wglew.h>

#include "WxGLCanvas.h"

#ifdef glewGetContext
#undef glewGetContext
#endif
#define glewGetContext() (dit::glew::glewCtx)

namespace dit {

    //-----------------------------------//

    namespace glew {

        GLEWContext* glewCtx = nullptr;

    }

    WxGLContext::WxGLContext(wxGLCanvas* canvas, WxWindow* window)
        : GfxContext(window)
        , glCtx(nullptr)
    {
        glCtx = new wxGLContext(canvas);

        if (!glCtx)
        {
            log("error creating wx cavas");
            return;
        }
    }

    //-----------------------------------//

    WxGLContext::~WxGLContext()
    {
        log("destroying ogl contex");
        if (glewGetContext() != nullptr)
        {
            delete glewGetContext();
            glewGetContext() = nullptr;
        }

        delete glCtx;
    }

    //-----------------------------------//

    void WxGLContext::MakeCurrent()
    {
        if (userData == nullptr)
        {
            log("WxGLContext::MakeCurrent() failed because the window handle is null.");
            return;
        }

        auto canvas = ((WxWindow*) userData)->canvas;
        assert(canvas && "expected a valid wxGLCanvas instance");

        glCtx->SetCurrent(*canvas);

        if (glewGetContext() == nullptr)
        {
            glewGetContext() = new GLEWContext{};
            auto res = glewInit();
            if (res != GLEW_OK)
            {
                String err{ (char*) glewGetErrorString(res) };
                log("glew failed to initialize: %0", err.c_str());
            }
        }
    }

    //-----------------------------------//

}