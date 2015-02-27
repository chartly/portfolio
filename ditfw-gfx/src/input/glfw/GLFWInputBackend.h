/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "InputManager.h"
#include "Keyboard.h"
#include "Mouse.h"

namespace dit
{

    class GLFWInputManager : public InputManager
    {
    public:
        GLFWInputManager();
        virtual ~GLFWInputManager();

        virtual Keyboard* getKeyboard() override;
        virtual Mouse* getMouse() override;

        Keyboard* keyboard;
        Mouse* mouse;

        static void OnKeyboardInput(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods);
        static void OnCharInput(GLFWwindow* window, uint32 key);
        static void OnMouseEnterWindow(GLFWwindow * window, int32 enteringWindow);
        static void OnMouseMove(GLFWwindow * window, double x, double y);
        static void OnMouseButton(GLFWwindow * window, int32 button, int32 action, int32 mods);
        static void OnMouseScroll(GLFWwindow * window, double xOffset, double yOffset);

        static Keys ConvertFromGLFWKey(int32 key);
        static MouseButton ConvertFromGLFWMouseButton(int32 button);
    };
}