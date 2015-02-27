/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"

namespace dit {

    //-----------------------------------//

    class InputBackend;
    class WxApplicationBase;

    //-----------------------------------//

    /**
     * This class is a platform abstraction layer responsible to provide
     * instances for platform-dependent engine subsytems like the window
     * and input managers. Each platform has one subclass that is provided
     * provided to the engine.
     */
    class PlatformManager
    {
    public:
        PlatformManager() = default;
        virtual ~PlatformManager() { }

        /// Initialization callback.
        virtual void Init() = 0;

        /// Update callback.
        virtual void Update() = 0;

        /// Creates the window manager subsystem.
        virtual WxApplicationBase* GetApp() = 0;

        /// Creates the input manager subsystem.
        virtual InputBackend* GetInputManager() = 0;
    };

    //-----------------------------------//

}