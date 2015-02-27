/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "Window.h"

//-----------------------------------//

namespace dit {

    /**
     *	OpenGL window implementation using GLFW to wrap platform-level
     *	windowing and GLEW to wrap the graphics context.
     */
    class GLFWwindow : public Window
    {
    public:
        GLFWwindow(const WindowSettings& settings, GLFWwindow * window, GLEWContext* context);
        virtual ~GLFWwindow();

        RenderContext* createContext(const RenderContextSettings& settings) override;

        void update() override;
        void makeCurrent() override;
        void show(bool visible = true) override;

        void setTitle(const String& title) override;
        void setCursorVisible(bool state) override;
        bool isCursorVisible() const override;
        Vector2i getCursorPosition() const override;
        void setCursorPosition(int32 x, int32 y) override;

        GLFWwindow* window;
        GLEWContext* context;
    };
}