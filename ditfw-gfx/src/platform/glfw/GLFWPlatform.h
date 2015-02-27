/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "PlatformManager.h"

namespace dit
{

    class GLFWPlatform : public PlatformManager
    {
    public:
        virtual ~GLFWPlatform();

        virtual void init() override;
        virtual void update() override;

        virtual WindowManager* getWindowManager() override;
        virtual InputManager* getInputManager() override;

        GLWindowManager* windows = nullptr;
        GLInputManager* input = nullptr;

        static void OnError(int32 error, const char * desc);
    };

}