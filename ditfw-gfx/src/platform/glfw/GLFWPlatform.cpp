/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "GLFWPlatform.h"
#include "GLFWApp.h"

namespace dit {

    //-----------------------------------//

    GLFWPlatform::~GLFWPlatform()
    {
        Deallocate(input);
        Deallocate(windows);

        glfwTerminate();
    }

    void GLFWPlatform::init()
    {
        // hook up framework adapters
        glfwSetErrorCallback(GLFWPlatform::OnError);

        // initialize glfw
        if(!glfwInit())
        {
            LogError("failed to initialize glfw.");
            return;
        }

        LogInfo("initialized glfw.");

        // alloc system adapters
        windows = Allocate(AllocatorGetHeap(), GLWindowManager);
        input = Allocate(AllocatorGetHeap(), GLInputManager);
    }

    void GLFWPlatform::update()
    {
        // update each window
        for(auto w : windows->windows)
        {
            w->update();
            //if(w->shouldClose)
            //  windowsToRemove.pushBack(w)
        }

        input->keyboard->resetKeys();
        glfwPollEvents();
    }

    WindowManager* GLFWPlatform::getWindowManager()
    {
        return windows;
    }

    InputManager* GLFWPlatform::getInputManager()
    {
        return input;
    }

    void GLFWPlatform::OnError(int32 error, const char * desc)
    {
        String msg;
        mini::format(msg, "glfw error: %0 : \"%1\"", error, desc);
        LogError(msg.c_str());
    }

    //-----------------------------------//
}