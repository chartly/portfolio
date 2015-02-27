/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "GLFWwindow.h"

namespace dit {

    //-----------------------------------//

    GLFWwindow::GLFWwindow(const WindowSettings& settings, GLFWwindow* win, GLEWContext* con)
        : Window(settings)
        , window(win)
        , context(con)
    {}

    GLFWwindow::~GLFWwindow()
    {
        glfwDestroyWindow(window);
    }

    RenderContext* GLFWwindow::createContext(const RenderContextSettings& settings)
    {
        return nullptr;
    }

    void GLFWwindow::update() 
    {}

    void GLFWwindow::makeCurrent()
    {
        glfwMakeContextCurrent(window);
    }

    void GLFWwindow::show(bool visible) 
    {
        if (visible)
            glfwShowWindow(window);
        else
            glfwHideWindow(window);
    }

    void GLFWwindow::setTitle(const String& title) 
    {
        glfwSetWindowTitle(window, title.c_str());
        LogInfo( "Changing window title to '%s'", title.c_str() );
    }

    void GLFWwindow::setCursorVisible(bool state)
    {
        if(state)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    bool GLFWwindow::isCursorVisible() const
    {
        return glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
    }

    void GLFWwindow::setCursorPosition(int32 x, int32 y)
    {
        glfwSetCursorPos(window, (double)x, (double)y);
    }

    Vector2i GLFWwindow::getCursorPosition() const
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return Vector2i((int32)x, (int32)y);
    }

    //-----------------------------------//

}