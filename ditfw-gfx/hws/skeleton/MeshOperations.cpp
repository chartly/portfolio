/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "HashMap.h"
#include "Core.h"
#include "MeshOperations.h"
#include "BasicMesh.h"
#include "Model.h"
#include "Plane.h"

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>

namespace dit {

    using namespace mesh;

    //-----------------------------------//

    float IntersectPlaneRay(const Plane& p, const float3& p0, const float3& p1)
    {
        return dot(p.normal, p.center - p0) / dot(p.normal, p1 - p0);
    }

    //-----------------------------------//

    float DistancePointPlane(const Plane& plane, const float3& pt)
    {
        return dot((pt - plane.center), plane.normal) / length(plane.normal);
    }

    //-----------------------------------//

    void ComputeConnectivity(BasicMesh* mesh)
    {
        // make pretty all the names
        auto& vertices = mesh->vertexPositions;
        auto& vertexFaces = mesh->vertexFaces;
        auto& vertexNormals = mesh->vertexNormals;
        auto& vfNodes = mesh->vfNodes;
        auto& faces = mesh->faceVertices;

        vertexFaces.resize(mesh->vertices());

        bool buildingNormals = vertexNormals.empty();
        if (buildingNormals)
            vertexNormals.resize(mesh->vertices());

        // walk each face
        walk(mesh::faces(*mesh), [&](FaceIterator& face) -> FaceIterator {

            // calculate the face's normal
            auto u = face.vtx(1) - face.vtx(0);
            auto v = face.vtx(2) - face.vtx(1);
            auto fn = cross(u, v);

            if (buildingNormals)
            {
                // if it is negative, reorder the verts
                if (length2(fn) < 0.f)
                {
                    std::swap(face.value().vtx[1], face.value().vtx[2]);

                    u = face.vtx(1) - face.vtx(0);
                    v = face.vtx(2) - face.vtx(1);

                    fn = cross(u, v);
                    assert(length2(fn) > 0.f);
                }
            }

            // walk the verts in cw order
            auto verts = mesh::vertices(face);
            walk(begin(verts), [&](VertexIterator& vtx) -> VertexIterator {

                // compute the vtx's normal
                if (buildingNormals)
                {
                    auto& vn = vtx.normal();
                    vn += fn;
                    vn = normalize(vn);
                }

                // insert the face into the vtx's face-list
                auto head = mesh->vertexFaces[vtx.idx];
                auto vfnIdx = vfNodes.size();
                vfNodes.push_back({ head.nodeIdx, face.idx });

                mesh->vertexFaces[vtx.idx] = VertexFace{ vfnIdx };

                // KEEP 'EM COMIN' SON
                return next(vtx);
            });

            return next(face);
        });
    }

    //-----------------------------------//

    BasicMesh* MeshFromOBJ(const String& file)
    {
        using namespace mesh;

        // Vertex meta data
        std::vector<float3> vertices;
        std::vector<float3> vertexNormals;
        std::vector<VertexFace> vertexFaces;
        std::vector<VertexFaceNode> vfNodes;

        // Face meta data
        std::vector<Face> faces;

        // check the file
        std::ifstream in(file.c_str(), std::ios::in);
        if (!in)
        {
            log("could not open file: %0", file.c_str());
            return nullptr;
        }

        // mesh data
        auto mesh = new BasicMesh{};

        std::array<float3, 2> exts{ { 
            float3{ std::numeric_limits<float>::max() }
            , float3{ std::numeric_limits<float>::min() } } };

        Timer loadClock;
        loadClock.Reset();

#pragma NOTE("Modify my OBJ loading code to support more attributes (see assignment handout).")

        // read the file in and create the vertex & face lists
        String line;
        try
        {
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
                    float3 v; ++tok;
                    v.x = boost::lexical_cast<float>(*tok); ++tok;
                    v.y = boost::lexical_cast<float>(*tok); ++tok;
                    v.z = boost::lexical_cast<float>(*tok); ++tok;

                    assert(tok == tokens.end());

                    vertices.push_back(v);

                    // determine the extents
                    exts[0] = float3{ std::min(v.x, exts[0].x), std::min(v.y, exts[0].y), std::min(v.z, exts[0].z) };
                    exts[1] = float3{ std::max(v.x, exts[1].x), std::max(v.y, exts[1].y), std::max(v.z, exts[1].z) };

                }
                else if (*tok == "vn") {
                    float3 vn; ++tok;
                    vn.x = boost::lexical_cast<float>(*tok); ++tok;
                    vn.y = boost::lexical_cast<float>(*tok); ++tok;
                    vn.z = boost::lexical_cast<float>(*tok); ++tok;

                    vertexNormals.push_back(vn);

                    assert(tok == tokens.end());
                }
                else if (*tok == "vf") {
                    log("[ %0 ] not supported { \"%1\" }", (*tok).c_str(), line.c_str());
                }
                else if (*tok == "vt") {
                    log("[ %0 ] not supported { \"%1\" }", (*tok).c_str(), line.c_str());
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

                    Face f;
                    f.vtx = { { a, b, c } };

                    auto fi = (uint32) faces.size();
                    faces.push_back(f);

#pragma endregion

                }
                else
                {
                    log("[ %0 ] not supported { \"%1\" }", (*tok).c_str(), line.c_str());
                }
            }

            // move the temp data into the mesh
            mesh->vertexPositions = std::move(vertices);
            mesh->vertexNormals = std::move(vertexNormals);
            mesh->vertexFaces = std::move(vertexFaces);
            mesh->vfNodes = std::move(vfNodes);
            mesh->faceVertices = std::move(faces);

            // build the topology graph
            ComputeConnectivity(mesh);

            mesh->exts = exts;
        }
        catch (const boost::bad_lexical_cast& e)
        {
            log(e.what());
        }
        catch (...)
        {
            return nullptr;
        }

#pragma endregion

        auto time = loadClock.GetElapsed();
        log("load time: %0s", time);

        return mesh;
    }

}