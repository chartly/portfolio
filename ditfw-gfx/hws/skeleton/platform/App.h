/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "Event.h"
#include "Core.h"
#include "Model.h"
#include "Camera.h"
#include "Shader.h"
#include "AppWindow.h"

#include <wx/wx.h>
#include <boost/filesystem/path.hpp>

#include <vector>
#include <unordered_map>
#include <unordered_set>

//-----------------------------------//
// Forward decls

namespace glmesh {

    class VertexFormat;
    class StreamBuffer;

}

//-----------------------------------//

namespace dit {

    //-----------------------------------//
    // Forward decls

    struct MouseMoveEvent;
    struct MouseDragEvent;
    struct MouseWheelEvent;

    class WxWindow;
    class WxInputBackend;

    //-----------------------------------//

    class SkeletonWindow : public AppWindow
    {
    public:
        SkeletonWindow(wxWindow* parent = nullptr, WxInputBackend* input = nullptr)
            : AppWindow(parent)
        {}

    public:

        virtual void OnOpenFile(wxCommandEvent& event);
        virtual void OnExit(wxCommandEvent& event);
        virtual void OnPropGridChanging(wxPropertyGridEvent& event);
        virtual void OnFlagSolidMesh(wxCommandEvent& event);
        virtual void OnFlagCoordFrame(wxCommandEvent& event);
        virtual void OnFlagVertNormals(wxCommandEvent& event);
        virtual void OnFlagFaceNormals(wxCommandEvent& event);

    public:
        WxInputBackend* input = nullptr;
    };

    //-----------------------------------//

    class App : public wxApp
    {
    public:
        // wx overrides
        virtual bool OnInit();
        virtual void CleanUp();
        virtual int32 FilterEvent(wxEvent& event);

    public:
        // common interface
        void Init();
        void Shutdown();
        void Update();
        void Draw();

    private:
        // helpers
        void InitGfx();
        void InitEvents();

        void DrawVertNormals(BasicMeshPtr mesh);
        void DrawFaceNormals(BasicMeshPtr mesh);

        GlMeshPtr BuildMesh(BasicMeshPtr mesh);
        String LoadTextFromFile(boost::filesystem::path file);
        ShaderPtr LoadShader(const String& baseName);
        uint32 CreateShader(const String& vtxShader, const String& fragShader);

        void OnMouseDrag(const MouseDragEvent& event);
        void OnMouseWheel(const MouseWheelEvent& event);
        void OnKeys();
        void OnFileWatch(WatchID watchid, const String& dir, const String& filename, Action action);

    public:
        // data
        Core*      core = nullptr;
        WxWindow*  window = nullptr;
        WxInputBackend* input = nullptr;
        Camera* camera = nullptr;

        EventMap appEvents;
        std::vector<ModelPtr> models;
        std::unordered_map<String, ShaderPtr> shaders;
        String currShader = "model";

        glmesh::VertexFormat* vtxFmt = nullptr;
        glmesh::StreamBuffer* stream = nullptr;

        enum class Flags : uint32
        {
            SolidMesh
            , DrawHoles
            , TriangleStrips
            , CoordFrame
            , VertNormals
            , FaceNormals
        };

        std::array<bool, 6> flags;

    private:
        HashMap<String> loadedShaders;
        boost::filesystem::path shaderBinPath{"../hws/skeleton/shaders/"};
        std::unordered_set<uint64> watchEvents;

        float2 lastMousePos{ 0.f, 0.f };
    };

    //-----------------------------------//

    DECLARE_APP(App)

    //-----------------------------------//
}