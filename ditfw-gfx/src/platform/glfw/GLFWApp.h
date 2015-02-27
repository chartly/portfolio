/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "CS350/API.h"
#include "CS350/GL/GLPlatform.h"
#include "CS350/GL/GLBuffers.h"
#include "CS350/GL/HalfEdgeMesh.h"
#include "CS350/GL/GLSLProgram.h"
#include "CS350/Camera.h"
#include "CS350/Crosshair.h"

#include "Engine/Engine.h"
#include "Resources/ResourceManager.h"
#include "Core/Timer.h"

namespace dit {

    class GLApp
    {
    public:
        GLApp();
        ~GLApp();

        virtual void init();
        virtual void onLoad();
        virtual void shutdown();
        virtual void update();
        virtual void draw();
        virtual void start();

        virtual void onKeyPress(KeyEvent const & e);
        virtual void onKeyRelease(KeyEvent const & e);
        virtual void onReloadButton(void* data);

        GLPlatform platform;
        Engine engine;
        GLWindow* window = nullptr;
        GLEWContext* context = nullptr;
        TwBar* tweakbar = nullptr;
        Timer loadClock;
        uint32 fpsCam = 1;

    public:
        static GLApp* instance() { return s_instance; }

    private:
        static GLApp* s_instance;

        void initTweakbar();
        void reloadMesh();

    private:
        std::array<std::string, 4> meshNames = { { 
            "<none>"
            , "horse_lowres_cleaned.obj"
            , "happy_lowres_cleaned.obj"
            , "dragon_lowres_cleaned.obj" } };
        uint32 meshIdx = 0;

        HalfEdgeMeshPtr mesh;
        Camera camera;
        Archive* archive = nullptr;
        
        GLSLProgramManager shaders;
        GLSLProgramPtr lineShader;
        GLSLProgramPtr modelShader;

        Crosshair crosshair;
        Array<Crosshair> camFrames;
        Timer camDebugClock;

        vec2 lastMousePos;
        uint32 lastCamKey = 0;
        bool shouldClose = false;
    };

    GLApp* fldApp();
    HalfEdgeMesh* parseObj(String const & s);
}

#ifdef glewGetContext
#undef glewGetContext
#endif
#define glewGetContext() (dit::fldApp()->context)