/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "CS350/API.h"
#include "CS350/GL/GLApp.h"
#include "CS350/GL/GLWindow.h"
#include "CS350/GL/HalfEdgeMesh.h"
#include "Engine/Input/Keyboard.h"
#include "Engine/Input/Mouse.h"
#include "Engine/Resources/Text.h"
#include "Core/Math/Hash.h"
#include "Core/Timer.h"
#include "Core/Archive.h"

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <numeric>
#include <fstream>

namespace boost {
    void throw_exception( std::exception const & e )
    {
        throw e;
    }
}

using namespace std;

namespace dit {

    //////////////////////////////////////////////////////////////////////////

    GLApp* fldApp()
    {
        return GLApp::instance();
    }

    GLApp* GLApp::s_instance = nullptr;

    GLApp::GLApp()
        : platform()
        , engine(&platform)
    {
        assert(s_instance == nullptr);
        s_instance = this;
    }

    GLApp::~GLApp()
    {
        assert(s_instance == this);
        s_instance = nullptr;
    }

    void GLApp::init()
    {
        engine.init();
        archive = Allocate(AllocatorGetHeap(), ArchiveDirectory, "./");

        // hook up keyboard input
        auto kb = platform.input->keyboard;
        kb->onKeyPress.Connect(this, &GLApp::onKeyPress);
        kb->onKeyRelease.Connect(this, &GLApp::onKeyRelease);
    }

    void GLApp::onLoad()
    {
        initTweakbar();

        // print out ogl version
        auto glv = glGetString(GL_VERSION);
        auto glslv = glGetString(GL_SHADING_LANGUAGE_VERSION);
        String msg;
        mini::format(msg, "GL Version: %0\tGLSL Version: %1", (const char*)glv, (const char*)glslv);

        LogInfo(msg.c_str());

        // set up the camera
        camera.position() = vec3{ 0.f, 0.f, 100.f };

        // set up the resource manager
        auto res = fldCore()->resourceManager;
        res->setArchive(archive);
        res->setAsynchronousLoading(false);

        // load shaders
        shaders.init();
        lineShader = shaders.load("./line.vs.glsl", "./line.fs.glsl");
        modelShader = shaders.load("./model.vs.glsl", "./model.fs.glsl");

        // crosshair & camera debugging stuff
        crosshair.buffer.create();
        camDebugClock.reset();
    }

    void GLApp::shutdown()
    {
        for (auto& cf : camFrames)
            cf.buffer.destroy();
        crosshair.buffer.destroy();

        TwTerminate();

        auto kb = platform.input->keyboard;
        kb->onKeyRelease.Disconnect(this, &GLApp::onKeyRelease);
        kb->onKeyPress.Disconnect(this, &GLApp::onKeyPress);

        if(context)
            Deallocate(context);

        Deallocate(archive);
    }

    void GLApp::update()
    {
        engine.update();

        // input
        auto mouse = platform.input->mouse->getMouseInfo();
        auto kb = platform.input->keyboard;
        auto mousePos = vec2{ (float)mouse.x, (float)mouse.y };
        auto delta = .005f * (mousePos - lastMousePos);

        if (fpsCam == 1)
        {
            // fps mouse controls
            camera.rotate(vec3{ 0.f, 1.f, 0.f }, delta.x);
            camera.rotate(vec3{ 1.f, 0.f, 0.f }, delta.y);

            // fps wasd controls
            auto fwd = camera.forward();
            auto right = camera.right();
            auto delta = zero<vec3>();

            if (kb->isKeyPressed(Keys::W))
                delta += fwd;
            if (kb->isKeyPressed(Keys::S))
                delta -= fwd;
            if (kb->isKeyPressed(Keys::A))
                delta -= right;
            if (kb->isKeyPressed(Keys::D))
                delta += right;

            if (length(delta) > 0.f)
                delta = normalize(delta);
            delta *= 5.f;
            camera.position() += delta;
        }

        lastMousePos = mousePos;
        lastCamKey = (kb->isKeyPressed(Keys::LShift)) ? 1 : 0;

        // camera debugging stuff
        if (camDebugClock.getElapsed() > 1.f)
        {
            auto cf = Crosshair{};
            cf.vtx = { {
                camera.position(), camera.right(),
                camera.position(), camera.up(),
                camera.position(), camera.forward()
            } };
            cf.buffer.create();
            
            camFrames.pushBack(cf);

            camDebugClock.reset();
        }

        // kick out
        if(glfwWindowShouldClose(window->window))
            shouldClose = true;
    }

    void GLApp::draw()
    {
        glClearColor(.15f,.15f,.15f,1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto wvp = camera.viewProjection();

        glUseProgram(lineShader->id);
        lineShader->uniforms[0].set(wvp);

        glBindVertexArray(crosshair.buffer.id);
        glDrawArrays(GL_LINES, 0, crosshair.vtx.size());

        for (const auto& cf : camFrames)
        {
            glBindVertexArray(cf.buffer.id);
            glDrawArrays(GL_LINES, 0, cf.vtx.size());
        }

        TwDraw();

        glfwSwapBuffers(window->window);
    }

    void GLApp::start()
    {
        // initialize everything
        init();

        // create the window
        window = (GLWindow *)platform.windows->createWindow({ 1280, 720, "Flood Engine", WindowStyles::None });

        // call anything that needs to be init'd after the window is created
        onLoad();

        // game loop
        while(!shouldClose)
        {
            update();
            draw();
        }

        shutdown();
    }

    void GLApp::onKeyPress(KeyEvent const & e)
    {
        if(e.keyCode == Keys::Escape)
            shouldClose = true;
    }

    void GLApp::onKeyRelease(KeyEvent const & e)
    {}

    void GLApp::onReloadButton(void* data)
    {
        String msg;
        mini::format(msg, "loading \"%0\"", meshNames[meshIdx].c_str());
        LogInfo(msg.c_str());

        reloadMesh();
    }

    //////////////////////////////////////////////////////////////////////////

    void GLApp::initTweakbar()
    {
        int32 w, h;
        glfwGetWindowSize(window->window, &w, &h);

        TwInit(TW_OPENGL, nullptr);
        TwWindowSize(w, h);
        tweakbar = TwNewBar("flood");

        TwAddVarRO(tweakbar, "fps cam", TW_TYPE_INT32, &fpsCam, nullptr);

        TwAddSeparator(tweakbar, nullptr, nullptr);

        uint32 val = 0;
        TwAddVarCB(tweakbar, "Mesh", TW_TYPE_INT32, [](const void* value, void*)
            {
                auto val = (int32*)value;
                fldApp()->meshIdx = *val % fldApp()->meshNames.size();
            }, [](void* value, void*) {
                auto& val = *(int32*)value;
                val = fldApp()->meshIdx;
            }, nullptr, nullptr);
        TwAddVarCB(tweakbar, "Vertices", TW_TYPE_UINT32, nullptr, [](void* value, void*) 
            {
                auto& val = *((uint32*)value);
                val = 0;
                if (fldApp()->mesh)
                    val = fldApp()->mesh->vertexPositions.size();
            }, nullptr, nullptr);
        TwAddVarCB(tweakbar, "Faces", TW_TYPE_UINT32, nullptr, [](void* value, void*)
        {
            auto& val = *((uint32*)value);
            val = 0;
            if (fldApp()->mesh)
                val = fldApp()->mesh->faceVertices.size();
        }, nullptr, nullptr);

        TwAddSeparator(tweakbar, nullptr, nullptr);

        TwAddButton(tweakbar, "reload", [](void* data){ fldApp()->onReloadButton(data); }, nullptr, nullptr);
    }

    //////////////////////////////////////////////////////////////////////////

    void GLApp::reloadMesh()
    {
        // allocate zero size mesh
        mesh = Allocate(AllocatorGetHeap(), HalfEdgeMesh, {});

        // load assignment models
        if (meshIdx != 0)
        {
            String file;
            mini::format(file, "./%0", meshNames[meshIdx].c_str());
            mesh = parseObj(file);
        }

        // set up vertex/index buffer
    }

    HalfEdgeMesh* parseObj(const String& filename)
    {
        using namespace mesh;

        // Vertex meta data
        Array<vec3> vertices;
        Array<VertexFace> vertexFaces;
        Array<VertexFaceNode> vfNodes;

        // Face meta data
        Array<Face> faces;
        Array<FaceEdge> faceEdges;
        Array<FaceEdgeNode> feNodes;

        // Edge meta data
        HashMap<uint32> edgeTraversals;
        Array<HalfEdge> edges;

        auto mesh = Allocate(AllocatorGetHeap(), HalfEdgeMesh, {});

        String msg;

        std::ifstream in(filename, std::ios::in);
        if (!in)
        {
            mini::format(msg, "could not open file: %0", filename.c_str());
            LogError(msg.c_str());
        }

        fldApp()->loadClock.reset();

        // read the file in and create the vertex & face lists
        std::string line;
        while (std::getline(in, line))
        {
            if (line[0] == '#') continue;   // comment
            if (line.size() < 3) continue;   // blank line

            // parse the data using spaces as a delimiter
            boost::char_separator<char> sep(" \t\r\n");
            boost::tokenizer<boost::char_separator<char>> tokens(line, sep);
            auto tok = tokens.begin();

#pragma region vertex parsing
            // vertex data
            if (*tok == "v") {
                vec3 v; ++tok;
                v.x = boost::lexical_cast<float>(*tok); ++tok;
                v.y = boost::lexical_cast<float>(*tok); ++tok;
                v.z = boost::lexical_cast<float>(*tok); ++tok;

                assert(tok == tokens.end());

                vertices.pushBack(v);

                VertexFace vf;
                vf.nodeIdx = MeshNullIdx;
                vertexFaces.pushBack(vf);

            }
#pragma endregion
#pragma region triangle parsing
            else if (*tok == "f") { // triangle data
                ++tok;
                auto a = boost::lexical_cast<uint32>(*tok); ++tok;
                auto b = boost::lexical_cast<uint32>(*tok); ++tok;
                auto c = boost::lexical_cast<uint32>(*tok); ++tok;

                assert(tok == tokens.end());

                // create the face
                a--; b--; c--;

                auto f = Face{};
                f.vtx = { { a, b, c } };

                auto fi = (uint32)faces.size();
                faces.pushBack(f);

                // insert the face into each vertex's face list
                auto vfi = (uint32)vfNodes.size();

                vfNodes.pushBack(VertexFaceNode{ vertexFaces[a].nodeIdx, fi });
                vfNodes.pushBack(VertexFaceNode{ vertexFaces[b].nodeIdx, fi });
                vfNodes.pushBack(VertexFaceNode{ vertexFaces[c].nodeIdx, fi });

                vertexFaces[a].nodeIdx = vfi;
                vertexFaces[b].nodeIdx = vfi + 1;
                vertexFaces[c].nodeIdx = vfi + 2;

#pragma endregion
#pragma region connectivity information
                // generate the edge data
                FaceEdge fe;

                for (size_t j = 2, k = 0; k < 2; j = k, k++)
                {
                    auto v0 = faces[fi].vtx[j];
                    auto v1 = faces[fi].vtx[k];

                    if (v0 < v1)
                        std::swap(v0, v1);

                    // compute hash for edge
                    HalfEdgeHash hash;
                    hash.v[0] = v0;
                    hash.v[1] = v1;

                    auto ei = edgeTraversals.get(hash.key, MeshNullIdx);

                    // if it doesn't exist, it hasn't been traversed yet, so insert it
                    if (ei == MeshNullIdx)
                    {
                        ei = edges.size();
                        edges.pushBack(HalfEdge{ MeshNullIdx, fi, { { v0, v1 } } });
                        edgeTraversals.set(hash.key, ei);
                    }
                    else // otherwise, we're traversing the twin
                    {
                        // compute the twin's hash and insert it
                        HalfEdgeHash thash;
                        thash.v[0] = v1;
                        thash.v[1] = v0;

                        auto ti = edges.size();
                        edges.pushBack(HalfEdge{ ei, fi, { { v1, v0 } } });
                        edgeTraversals.set(thash.key, ti);

                        // update the edge's twin pointer
                        edges[ei].twinIdx = ti;

                        // this is so I can undo the ordering I enforced earlier for below..
                        ei = ti;
                    }

                    auto nei = feNodes.size();
                    feNodes.pushBack({ fe.nodeIdx[k], ei });
                    fe.nodeIdx[k] = nei;
                }

                // insert the edge into the face's edge list
                auto fei = faceEdges.size();
                faceEdges.pushBack(fe);

                faceEdges[fi].nodeIdx = { { fei, fei, fei } };
            }
        }

#pragma endregion

        mesh->vertexPositions = std::move(vertices);
        mesh->vertexFaces = std::move(vertexFaces);
        mesh->vfNodes = std::move(vfNodes);
        mesh->faceVertices = std::move(faces);
        mesh->faceEdges = std::move(faceEdges);
        mesh->halfEdges = std::move(edges);
        
#pragma region calculate normals
        mesh->vertexNormals.reserve(mesh->vertexPositions.capacity());
        mesh->vertexNormals.resize(mesh->vertexPositions.size());

        walk(mesh::faces(mesh), [](const FaceIterator& face) -> FaceIterator
        {
            std::array<vec3, 3> vtx = { {
                face.mesh->vertexPositions[face.value().vtx[0]]
                , face.mesh->vertexPositions[face.value().vtx[1]]
                , face.mesh->vertexPositions[face.value().vtx[2]] } };

            auto u = vtx[1] - vtx[0];
            auto v = vtx[2] - vtx[0];
            auto faceNormal = cross(u, v);
            faceNormal = normalize(faceNormal);

            auto verts = mesh::vertices(face);
            walk(begin(verts), [=](VertexIterator& vtx) -> VertexIterator
            {
                auto& vn = vtx.normal();
                vn += faceNormal;
                vn = normalize(vn);

                return next(vtx);
            });

            return next(face);
        });
#pragma endregion

        auto time = fldApp()->loadClock.getElapsed();
        mini::format(msg, "load time: %0s", time);
        LogInfo(msg.c_str());

        return mesh;
    }

    //////////////////////////////////////////////////////////////////////////
}