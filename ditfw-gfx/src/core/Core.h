/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "Timer.h"
#include "Log.h"
#include "ThreadPool.h"
#include "FileWatcher/FileWatcher.h"

namespace dit {

    //-----------------------------------//

    class Entity;

    //-----------------------------------//

    class Core
    {
    public:
        Core();
        ~Core();

        void Init();
        void Update();
        void Shutdown();

    public:
        Timer       timer;
        Log         logger;
        ThreadPool  threads = { 8 };
        FileWatcher fileWatcher;

        Entity* root = nullptr;

        double lastFrameTime;

        // static access
    private:
        static Core* _instance;
        friend Core* core();
    };

    //-----------------------------------//

    Core* core();

    //-----------------------------------//

}