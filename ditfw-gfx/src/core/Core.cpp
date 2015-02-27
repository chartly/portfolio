/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "Core.h"
#include "Entity.h"

#include <FileWatcher/FileWatcherWin32.h>
#include <boost/filesystem.hpp>

namespace dit {

    //-----------------------------------//

    Core* Core::_instance = nullptr;

    //-----------------------------------//
    
    Core* core()
    {
        return Core::_instance;
    }

    //-----------------------------------//

    Core::Core()
    {
        if (_instance == nullptr)
            _instance = this;

        // init the logger / forward errors to stdout
        auto printfn = [](Log::Entry* e) { printf("%s", e->msgLine.c_str()); };

        logger.errorHandlers.push_back(printfn);
        logger.infoHandlers.push_back(printfn);
    }

    //-----------------------------------//

    Core::~Core()
    {
        if (_instance == this)
            _instance = nullptr;
    }

    //-----------------------------------//

    void Core::Init()
    {
        // create the root event pump
        root = new Entity{};
        root->Init();

        // log the cwd
        auto cwd = boost::filesystem::current_path();
        log("cwd: \"%0\"", cwd.string().c_str());
    }

    //-----------------------------------//

    void Core::Update()
    {
        // check for file changes from os
        fileWatcher.Update();

        // process any update messages
        root->Update(timer.GetCurrent() - lastFrameTime);
        lastFrameTime = timer.GetElapsed();
    }

    //-----------------------------------//

    void Core::Shutdown()
    {
        // destroy the root entity
        root->Shutdown();
        delete root;
        root = nullptr;
    }

}