/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"

#include <glload/gl_4_0.hpp>
#include <glload/gl_load.hpp>
#include <glmesh/glmesh.h>
#include <glutil/glutil.h>

#include "App.h"
#include "Entity.h"
#include "BasicMesh.h"
#include "MeshOperations.h"
#include "WxWindow.h"
#include "WxInputBackend.h"
#include "WxGLCanvas.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace fs = boost::filesystem;

namespace dit {

#pragma region Window implementation
    
    //-----------------------------------//

#pragma NOTE("//-----------------------------------//")
#pragma NOTE("class SkeletonWindow : public AppWindow")
#pragma NOTE("{")

    void SkeletonWindow::OnOpenFile(wxCommandEvent& event)
    {
        wxFileDialog ofd {
        this
        , "Open OBJ file"
        , "", ""
        , "OBJ files (*.obj)|*.obj"
        , wxFD_OPEN | wxFD_FILE_MUST_EXIST };

        auto res = ofd.ShowModal();
        if (res == wxID_CANCEL)
            return;

        String fileName{ ofd.GetPath().c_str() };

        auto loadFn = [=]() {
            log("loading mesh \"%0\"", fileName.c_str());

            auto mesh = MeshFromOBJ(fileName);
            auto msp = std::make_shared<BasicMesh>();
            msp.reset(mesh);

            send(core()->root->events, "mesh-loaded", msp, fileName);
        };

        #pragma NOTE("01) Turn on / off asynchronous file loading.")
        core()->threads.Post(loadFn);
        //loadFn();
    }

    //-----------------------------------//

#pragma NOTE("02) Insert custom event handlers for wxEvents coming directly from the GUI.")

    void SkeletonWindow::OnExit(wxCommandEvent& event)
    {
        this->Close();
    }

    //-----------------------------------//

    void SkeletonWindow::OnPropGridChanging(wxPropertyGridEvent& event)
    {
        event.Veto();
    }

    //-----------------------------------//

    void SkeletonWindow::OnFlagSolidMesh(wxCommandEvent& event)
    {
        send(core()->root->events, "toggle-flag", App::Flags::SolidMesh);
    }

    //-----------------------------------//

    void SkeletonWindow::OnFlagCoordFrame(wxCommandEvent& event)
    {
        send(core()->root->events, "toggle-flag", App::Flags::CoordFrame);
    }

    //-----------------------------------//

    void SkeletonWindow::OnFlagVertNormals(wxCommandEvent& event)
    {
        send(core()->root->events, "toggle-flag", App::Flags::VertNormals);
    }

    void SkeletonWindow::OnFlagFaceNormals(wxCommandEvent& event)
    {
        send(core()->root->events, "toggle-flag", App::Flags::FaceNormals);
    }

    //-----------------------------------//

#pragma NOTE("}")

#pragma endregion
#pragma region wxApp overrides

    //-----------------------------------//

#pragma NOTE("//-----------------------------------//")
#pragma NOTE("class App : public wxApp")
#pragma NOTE("{")
    bool App::OnInit()
    {
        // init wx
        if (!wxApp::OnInit())
            return false;

        wxInitAllImageHandlers();

        Init();

        return true;
    }

    //-----------------------------------//

    void App::CleanUp()
    {
        Shutdown();
    }

    //-----------------------------------//

    int32 App::FilterEvent(wxEvent& event)
    {
        if (input == nullptr)
            return -1;
#pragma NOTE("03) Trap custom events in the event pump (such as kb / mouse input) from wxWidgets to translate into custom event formats.")

        // keyboard events
        auto type = event.GetEventType();
        if (type == wxEVT_CHAR)
        {
            input->ProcessCharEvent((wxKeyEvent&) event);
            return 1;
        }
        else if (type == wxEVT_KEY_DOWN)
        {
            input->ProcessKeyEvent((wxKeyEvent&) event, true);
            return 1;
        }
        else if (type == wxEVT_KEY_UP)
        {
            input->ProcessKeyEvent((wxKeyEvent&) event, false);
            return 1;
        }

        // mouse events
        static std::array<wxEventType, 13> mouseEvents{ {
            wxEVT_LEFT_DOWN
            , wxEVT_LEFT_UP
            , wxEVT_MIDDLE_DOWN
            , wxEVT_MIDDLE_UP
            , wxEVT_RIGHT_DOWN
            , wxEVT_RIGHT_UP
            , wxEVT_MOTION
            , wxEVT_LEFT_DCLICK
            , wxEVT_MIDDLE_DCLICK
            , wxEVT_RIGHT_DCLICK
            , wxEVT_LEAVE_WINDOW
            , wxEVT_ENTER_WINDOW
            , wxEVT_MOUSEWHEEL } };

        for (auto meType : mouseEvents)
        {
            if (type == meType)
            {
                input->ProcessMouseEvent((wxMouseEvent&) event);
                return -1; // still want to propogate the mouse event to the gui
            }
        }

        return -1;
    }

    //-----------------------------------//

#pragma endregion
#pragma region Common interface

    //-----------------------------------//

    void App::Init()
    {
#pragma NOTE("04) App::Init()")
        flags = { { true, false, false, true, false, false } };

        // create stuff
        core = new Core{};
        input = new WxInputBackend{};

        // initialize all the things
        core->Init();
        InitGfx();
        InitEvents();

        // get the mouse pos
        auto& ms = input->mouse.state;
        lastMousePos = float2{ ms.x, ms.y };

        // show it
        window->Show(true);
    }

    //-----------------------------------//

    void App::InitGfx()
    {
        using namespace glmesh;

#pragma NOTE("05) App::InitGfx() .... Create the gui window")
        // create the window
        auto hw = new SkeletonWindow{ nullptr, input };
        int32 width = 0, height = 0;
        hw->GetClientSize(&width, &height);
        auto gwp = hw->gameWindowPanel;

        window = new WxWindow{ { (uint16) width, (uint16) height, String{ hw->GetTitle().c_str() }, hw }, hw, input };

        // create the cavas / gl context
        std::array<int32, 9> glAttrs{ {
                WX_GL_RGBA, 1
                , WX_GL_LEVEL, 0
                , WX_GL_DOUBLEBUFFER, 1
                , WX_GL_DEPTH_SIZE, 32
                , 0
                } };

        window->canvas = new WxGLCanvas{ window, gwp, -1, glAttrs.data(), gwp->GetPosition(), gwp->GetSize(), gwp->GetWindowStyle() };
        window->canvas->Reparent(hw);

        window->CreateContext();
        window->MakeCurrent();

#pragma NOTE("06) App::InitGfx() .... OpenGL intialization")
        // init open gl
        auto glloadRes = glload::LoadFunctions();
        String glloadStatus;
        if (!glloadRes)
            glloadStatus = "failed to initialize";
        else
            glloadStatus = "initialized";

        log("%0 OpenGL %1.%2", glloadStatus.c_str(), glload::GetMajorVersion(), glload::GetMinorVersion());

        // connect to wx events
        window->onUpdateEvent.push_back([&]() {
            Update();
        });

        window->onDrawEvent.push_back([&]() {
            Draw();
        });


#pragma NOTE("07) App::InitGfx() .... Create any additional VAO/VBO's or shaders.")
        // create the glmesh immediate mode helpers
        AttributeList attrs{ {
                { 0, 3, VDT_SINGLE_FLOAT, ADT_FLOAT }       // position
                , { 1, 3, VDT_SINGLE_FLOAT, ADT_FLOAT }     // normal
                , { 2, 3, VDT_UNSIGN_BYTE, ADT_NORM_FLOAT } // color
                } };

        vtxFmt = new VertexFormat{ attrs };
        stream = new StreamBuffer{ 4 * 1024 * 1024 };

        // load a shader
        LoadShader("model");
        LoadShader("line");

        // camera stuff
        camera = new Camera{};
        camera->position() = float3{ 5.f, 5.f, 5.f };
        camera->rotation() = lookAt(camera->position(), float3{ 0.f, 0.f, 0.f }, float3{ 0.f, 1.f, 0.f });
        camera->inverse() = inverse(camera->rotation());
    }

    //-----------------------------------//

    void App::InitEvents()
    {
        //-----------------------------------//
        // app events

#pragma NOTE("08) App::InitEvents() .... Create app-specific event handlers, such as file watching, toggling flags from the gui, or storing a mesh that's just been loaded.")

        // filewatcher stuff
        FileWatcher::Listener fileWatch = [&](WatchID watchid, const std::string& dir, const std::string& filename, Action action) {
            this->OnFileWatch(watchid, dir.c_str(), filename.c_str(), action);
        };
        core->fileWatcher.Add(shaderBinPath.string(), fileWatch);

        // mesh loading
        std::function<void(BasicMeshPtr, String)> onMeshLoaded = [&](BasicMeshPtr mesh, String path) {

            // create the model
            auto mdl = std::make_shared<Model>(mesh, nullptr, zero<float3>(), one<float3>(), 0.f);
            float3 exts = mdl->meshData->exts[1] - mdl->meshData->exts[0];
            float maxExt = std::max(exts.x, std::max(exts.y, exts.z));
            maxExt = 1.f / maxExt;
            
            mdl->scale = float3{ maxExt };
            mdl->drawable = BuildMesh(mdl->meshData);

            // populate the ui
            auto wxw = (SkeletonWindow*)(window->wxw);
            wxw->modelFile->SetValueFromString(wxString{ path.c_str() });
            wxw->triCount->SetValueFromInt(mdl->meshData->triangles());
            wxw->vertCount->SetValueFromInt(mdl->meshData->vertices());

            // only one model at a time
            models.clear();
            models.push_back(mdl);

            log("model scale: %0", maxExt);
        };

        // shader selection
        std::function<void(App::Flags)> onFlag = [&](App::Flags type) {
            flags[(uint32) type] = !flags[(uint32) type];
        };

#pragma NOTE("09) App::InitEvents() .... Bind the app's listeners, and set them as the global event handlers.")
        // app event table
        appEvents = {
            { "mesh-loaded", onMeshLoaded }
            , { "toggle-flag", onFlag } };
        core->root->events.map = &appEvents;

        //-----------------------------------//
        // input

        // hook up mouse input
        Mouse::MouseDragConsumer onMouseDrag = [&](const MouseDragEvent& e) {
            OnMouseDrag(e);
        };
        Mouse::MouseWheelConsumer onMouseWheel = [&](const MouseWheelEvent& e) {
            OnMouseWheel(e);
        };

        input->mouse.onMouseDrag.push_back(onMouseDrag);
        input->mouse.onMouseWheelMove.push_back(onMouseWheel);

        // hook up the keyboard stuff
        Keyboard::KeyEventConsumer onKeys = [&](const KeyEvent&) {
            OnKeys();
        };

        input->keyboard.onKeyPress.push_back(onKeys);
    }

    //-----------------------------------//

    void App::Shutdown()
    {
        // shutdown / clean up
        models.clear();
        core->Shutdown();

        // destroy resources
        delete stream;
        delete vtxFmt;
        delete camera;
        delete input;
        delete window;
        delete core;

        stream = nullptr;
        vtxFmt = nullptr;
        camera = nullptr;
        input = nullptr;
        window = nullptr;
        core = nullptr;
    }

    //-----------------------------------//

    void App::Update()
    {
#pragma NOTE("10) App::Update()")

        // clear last frame's filewatch events
        watchEvents.clear();

        // calculate normalized mouse coords
        auto& ms = input->mouse.state;
        float mx = ((2.f * ms.x) / 1280) - 1.f;
        float my = 1.f - (2.f * ms.y) / 720;

        lastMousePos = float2{ mx, my };

        // move the camera around
        camera->rotation() = lookAt(camera->position(), float3{ 0.f, 0.f, 0.f }, float3{ 0.f, 1.f, 0.f });
        camera->inverse() = inverse(camera->rotation());

        // pump events from other threads
        core->Update();
    }

    //-----------------------------------//

    void App::OnKeys()
    {
#pragma NOTE("11) App::OnKeys() ..... Make any changes to the keyboard input response here.")
        auto& kb = input->keyboard;

        // drop out on escape
        if (kb.IsKeyDown(Keys::Escape))
            window->wxw->Close();

        // move the current model around
        if (!models.empty())
        {
            float3 scale = 1.f / models[0]->scale;
            float3 p{ 0.f };

            if (kb.IsKeyDown(Keys::Left))
                p -= float3{ 1.f, 0.f, 0.f };
            if (kb.IsKeyDown(Keys::Right))
                p += float3{ 1.f, 0.f, 0.f };
            if (kb.IsKeyDown(Keys::Up))
                p -= float3{ 0.f, 1.f, 0.f };
            if (kb.IsKeyDown(Keys::Down))
                p += float3{ 0.f, 1.f, 0.f };

            models[0]->pos += p * scale;
        }
    }

    //-----------------------------------//

    void App::OnMouseDrag(const MouseDragEvent& e)
    {
#pragma NOTE("12) App::OnMouseDrag() ..... Make any changes to the mouse handling of the camera here.")
        auto& cam = *camera;

        float3 fwd = cam.position() - zero<float3>();
        float3 up{ 0.f, 1.f, 0.f };
        float3 right = normalize(cross(fwd, up));
        float3 pos = cam.position();

        float r = length(fwd);
        float K = std::min(r / 10.f, 1.f) * 10.f;

        auto& ms = input->mouse.state;
        float mx = ((2.f * ms.x) / 1280) - 1.f;
        float my = 1.f - (2.f * ms.y) / 720;

        pos += K * (mx - lastMousePos.x) * right;
        pos += -K * (my - lastMousePos.y) * up;

        cam.position() = r * normalize(pos - zero<float3>());
    }

    //-----------------------------------//

    void App::OnMouseWheel(const MouseWheelEvent& e)
    {
        auto& cam = *camera;
        float3 fwd = cam.position() - zero<float3>();
        float d = length(fwd);
        if (d < .15f && e.delta > 0)
            return;

        cam.position() = cam.position() + (float) e.delta * -.3f * (fwd / d);
    }

    //-----------------------------------//

    void App::Draw()
    {
        auto& cam = *camera;

#pragma NOTE("<---------- BEGIN FRAME ---------->")
        gl::LineWidth(2.f);

        // culling
        gl::Enable(gl::CULL_FACE);
        gl::CullFace(gl::BACK);

        // depth buffer
        gl::Enable(gl::DEPTH_TEST);
        gl::DepthFunc(gl::LESS);
        gl::DepthMask(gl::TRUE_);

        // clear
        gl::ClearColor(.1f, .1f, .1f, 1.f);
        gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

        auto size = window->canvas->GetSize();
        gl::Viewport(0, 0, size.x, size.y);

#pragma NOTE("13) Selecting a shader with which to draw verts.")
        // set up model shader
        auto prog = shaders[currShader];
        if (prog == nullptr)
            return;

        gl::UseProgram(prog->id);

#pragma NOTE("14) Calculate any matrices / colors / etc that need to be sent as input to the shader each frame.")
        float4x4 view = cam.view();
        float4x4 projection = cam.projection();

        float4x4 wv = view;
        float4x4 wvp = projection * view;
        float3x3 wit = one<float3x3>();

        // the position of the light
        float4 lightPos{ 5.f, 5.f, 5.f, 1.f };

        // this is diffuse
        float3 Kd{ 1.f, 1.f, 1.f };
        float3 Ld{ 1.f, 1.f, 1.f };

#pragma NOTE("15) Set the shader uniforms before anything is drawn.")
        prog->set("lightViewPos", lightPos);
        prog->set("Kd", Kd);
        prog->set("Ld", Ld);
        prog->set<const float4x4&>("wv", wv);
        prog->set<const float4x4&>("wvp", wvp);
        prog->set<const float3x3&>("wit", wit);


        // draw reticle
        if (flags[(uint32)Flags::CoordFrame])
        {
            using namespace mesh;
            using namespace glmesh;

            float3 eye = zero<float3>();

            std::array<float3, 3> axes { {
                float3{ 1.f, 0.f, 0.f }
                , float3{ 0.f, 1.f, 0.f }
                , float3{ 0.f, 0.f, -1.f } } };

            std::array<float3, 6> vtx { {
                eye - 1.f * axes[0]
                , eye + 1.f * axes[0]
                , eye - 1.f * axes[1]
                , eye + 1.f * axes[1]
                , eye - 1.f * axes[2]
                , eye + 1.f * axes[2] } };

            std::array<glmesh::gen::Color, 6> clrs{ {
                glmesh::gen::Color{ 255, 0, 0, 255 }
                , glmesh::gen::Color{ 0, 255, 0, 255 }
                , glmesh::gen::Color{ 0, 0, 255, 255 }
                , glmesh::gen::Color{ 255, 0, 0, 255 }
                , glmesh::gen::Color{ 0, 255, 0, 255 } 
                , glmesh::gen::Color{ 0, 0, 255, 255 } } };

            glmesh::Draw drawable{ gl::LINES, vtx.size(), *vtxFmt, *stream };
            for (uint32 i = 0; i < vtx.size(); ++i)
            {
                drawable.Attrib(vtx[i]);
                drawable.Attrib(one<float3>());    // just pass in some value for the normal
                drawable.Attrib<byte>(clrs[i][0], clrs[i][1], clrs[i][2], clrs[i][3]);
            }

            drawable.Render();
        }


#pragma NOTE("16) Draw any meshes you have.")
        // draw meshes
        for (auto& mdl : models)
        {
            wv = view * mdl->world();
            wvp = projection * wv;

            prog->set<const float4x4&>("wv", wv);
            prog->set<const float4x4&>("wvp", wvp);

            if (flags[(uint32)Flags::SolidMesh])
                mdl->drawable->Render();
            if (flags[(uint32) Flags::VertNormals])
                DrawVertNormals(mdl->meshData);
            if (flags[(uint32) Flags::FaceNormals])
                DrawFaceNormals(mdl->meshData);
        }

#pragma NOTE("<---------- END FRAME ---------->")
    }

    //-----------------------------------//

    void App::DrawVertNormals(BasicMeshPtr mesh)
    {
        using namespace mesh;
        using namespace glmesh;

        glmesh::Draw drawable{ gl::LINES, mesh->vertices() * 2, *vtxFmt, *stream };

#pragma NOTE("17) App::DrawVertMormals() ..... Build and draw a series of LINES to show the vertex normals.")
        walk(vertices(*mesh), [&](const VertexIterator& vtx) -> VertexIterator {

            auto p = vtx.pos();
            auto n = vtx.normal();

            // submit the vertex's attributes
            drawable.Attrib(p);
            drawable.Attrib(n);
            drawable.Attrib<byte>(255, 0, 0, 255);

            // submit the vertex + normal
            drawable.Attrib(p + n);
            drawable.Attrib(n);
            drawable.Attrib<byte>(255, 0, 0, 255);

            // continue looping
            return next(vtx);
        });

        // send verts to gpu
        drawable.Render();
    }

    //-----------------------------------//

    void App::DrawFaceNormals(BasicMeshPtr mesh)
    {
        using namespace mesh;
        using namespace glmesh;

        glmesh::Draw drawable{ gl::LINES, mesh->triangles() * 2, *vtxFmt, *stream };

#pragma NOTE("17) App::DrawFaceMormals() ..... Build and draw a series of LINES to show the face normals.")
        walk(faces(*mesh), [&](const FaceIterator& face) -> FaceIterator {

            // centroid of the three verts and normal
            float3 p{ 0.f };
            float3 n = face.normal();

            // accumulate vertex positions
            auto verts = vertices(face);
            walk(begin(verts), [&](const VertexIterator& vtx) -> VertexIterator {

                p += vtx.pos();

                return next(vtx);
            });

            p /= 3.f;

            // submit vtx at the center of the triangle
            drawable.Attrib(p);
            drawable.Attrib(n);
            drawable.Attrib<byte>(0, 0, 255, 255);

            // submit centroid + normal
            drawable.Attrib(p + n);
            drawable.Attrib(n);
            drawable.Attrib<byte>(0, 0, 255, 255);

            // continue looping
            return next(face);
        });

        // send verts to gpu
        drawable.Render();
    }

    //-----------------------------------//

    GlMeshPtr App::BuildMesh(BasicMeshPtr mesh)
    {
        using namespace mesh;
        using namespace glmesh;

        Timer loadClock;
        loadClock.Reset();

#pragma NOTE("18) App::BuildMesh() ..... Create a VAO/VBO out of the mesh data you loaded from file.")

        VertexFormat fmt{ *vtxFmt };

        CpuDataWriter writer{ fmt, mesh->triangles() * 3 };
        walk(faces(*mesh), [&](const FaceIterator& face) -> FaceIterator {

            auto fvts = vertices(face);
            walk(begin(fvts), [&](const VertexIterator& vtx) -> VertexIterator {
                writer.Attrib(vtx.pos());
                writer.Attrib(vtx.normal());
                writer.Attrib<byte>(128, 128, 128, 255);

                return next(vtx);
            });

            return next(face);
        });

        // send vtx data to gpu
        auto vtxBuffer = writer.TransferToBuffer(gl::ARRAY_BUFFER, gl::STATIC_DRAW);

        // create the attribute arrays
        std::array<uint32, 2> vao { { 0, 0 } };
        gl::GenVertexArrays(2, &vao[0]);

        gl::BindBuffer(gl::ARRAY_BUFFER, vtxBuffer);

        // variant -> "all"
        gl::BindVertexArray(vao[0]);
        fmt.BindAttributes(0);

        // variant -> "pos-only"
        gl::BindVertexArray(vao[1]);
        fmt.BindAttribute(0, 0);

        gl::BindBuffer(gl::ARRAY_BUFFER, 0);

        MeshVariantMap variants;
        variants["all"] = vao[0];
        variants["pos-only"] = vao[1];

        // render commands
        RenderCmdList cmds;
        cmds.DrawArrays(gl::TRIANGLES, 0, mesh->triangles() * 3);

        auto newMesh = new Mesh{ { { vtxBuffer } }, vao[0], cmds, variants };
        GlMeshPtr ptr;
        ptr.reset(newMesh);

        // log it
        auto time = loadClock.GetElapsed();
        log("build time: %0s", time);

        return ptr;
    }

    //-----------------------------------//

    ShaderPtr App::LoadShader(const String& baseName)
    {
        static String msg;

#pragma NOTE("19) App::LoadShader() ..... Load any requested vertex or fragment shaders from file.")
        String vtx;
        String frag;

        // generate file names
        mini::format(vtx, "%0%1.vs.glsl", shaderBinPath.string().c_str(), baseName.c_str());
        mini::format(frag, "%0%1.fs.glsl", shaderBinPath.string().c_str(), baseName.c_str());

        // create human-friendly log message
        mini::format(msg, "loaded shader [\"%0\"] {\"%1\", \"%2\"} ", baseName.c_str(), vtx.c_str(), frag.c_str());

        // hash shader base-name & files
        std::array<uint64, 2> keys = { { 
            MurmurHash64(vtx.c_str(), vtx.size(), 0)
            , MurmurHash64(frag.c_str(), frag.size(), 0) } };

        // load the text from file
        vtx = std::move(LoadTextFromFile(fs::path{ vtx.c_str() }));
        frag = std::move(LoadTextFromFile(fs::path{ frag.c_str() }));

        // store shader for future reference
        auto idx = CreateShader(vtx, frag);
        ShaderPtr prog;
        if (idx != 0)
        {
            prog = std::make_shared<Shader>(baseName, idx);

            shaders[baseName] = prog;
            loadedShaders.set(keys[0], baseName);
            loadedShaders.set(keys[1], baseName);

            log(msg.c_str());
        }

        return prog;
    }

    //-----------------------------------//

    uint32 App::CreateShader(const String& vtxShader, const String& fragShader)
    {
        // compile / link
        uint32 prog = 0;

        try
        {
            prog = glutil::LinkProgram(vtxShader, fragShader);
        }
        catch (const glutil::CompileLinkException& e)
        {
            log("failed to compile/link shader: %0", e.what());
            return 0;
        }

        return prog;
    }

    //-----------------------------------//

    String App::LoadTextFromFile(boost::filesystem::path file)
    {
#pragma NOTE("20) App::LoadTextFromFile() ..... Reload the text from any files that you care about which have changed while the game is running.")
        String text;

        // error handling
        if (!exists(file))
        {
            log("cannot open file \"%0\"", file.string().c_str());
            return text;
        }
        else if (is_directory(file))
        {
            log("cannot load directory as file \"%0\"", file.string().c_str());
            return text;
        }
        
        // load and format
        fs::ifstream stream{ file, std::ios::in };
        std::stringstream ss;

        ss << stream.rdbuf();
        text = ss.str().c_str();

        // notify the filewatcher
        send(core->root->events, "watch-file", file.string());

        // SEND IT HOME, SON
        return std::move(text);
    }

    //-----------------------------------//

    /**
     *	This is here basically because windows sends multiple events for the same file action, so you
     *  want some way of filter out doubly disptached events.
     */
    void App::OnFileWatch(WatchID watchid, const String& dir, const String& filename, Action action)
    {
        // static helper data
        static std::array<String, 3> actionNames{ { "ADDED", "DELETED", "MODIFIED" } };

        // format event message
        String msg;
        mini::format(msg, "file watch - %0 \"%1%2\"", actionNames[(uint32) action].c_str(), dir.c_str(), filename.c_str());

        // prevent double-dispatched events
        auto eventHash = MurmurHash64(msg.c_str(), msg.size(), 0);
        if (watchEvents.find(eventHash) != watchEvents.end())
            return;

        watchEvents.insert(eventHash);
        log(msg.c_str());

#pragma NOTE("21) App::OnFileWatch() ..... Change what kinds of files you want to watch for changes.")
        // determine if this file needs to be reloaded
        if (action == Actions::Modified)
        {
            fs::path file{ (dir + filename).c_str() };
            if (file.extension() == ".glsl")
            {
                auto key = MurmurHash64(file.string().c_str(), file.string().size(), 0);
                auto base = loadedShaders.get(key, String{});
                if (!base.empty())
                    LoadShader(base);
            }
        }
    }

    //-----------------------------------//

#pragma NOTE("}")

#pragma endregion
}