/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "WindowManager.h"

namespace dit
{
    class GLWindow;

    class GLFWWindowManager : public WindowManager
    {
    public:
        GLFWWindowManager();
        virtual ~GLFWWindowManager();

        Window* createWindow(const WindowSettings& settings) override;
        Window* openFileDialog(const String& wildcard, FileDialogFlags flags) override              { return nullptr; }
        Window* openDirectoryDialog(const String& wildcard, DirectoryDialogFlags flags) override    { return nullptr; }

        std::vector<GLWindow*> windows;

        static void OnFrameBufferResize(GLFWwindow * window, int32 width, int32 height);
    };
}