/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "MathExt.h"

#include <array>
#include <vector>
#include <numeric>
#include <functional>

namespace dit {

    //-----------------------------------//
    // Global values

    const uint32 MeshNullIdx = std::numeric_limits<uint32>::max();

    //-----------------------------------//
    // Half-edge data structures

    struct VertexFace
    {
        VertexFace() = default;
        VertexFace(uint32 node) : nodeIdx(node) {}

        uint32 nodeIdx = MeshNullIdx;
    };

    //-----------------------------------//

    struct VertexFaceNode
    {
        VertexFaceNode() = default;
        VertexFaceNode(uint32 next, uint32 faceIdx);

        uint32 next = MeshNullIdx;
        uint32 faceIdx = MeshNullIdx;
    };

    //-----------------------------------//

    struct Face
    {
        Face() { vtx = { { MeshNullIdx, MeshNullIdx, MeshNullIdx } }; }
        Face(const std::array<uint32, 3>& vtx);

        std::array<uint32, 3> vtx;
    };


    //-----------------------------------//
    // Basic mesh structure

    class BasicMesh
    {
    public:
        BasicMesh() { exts = { { float3{ 0.f }, float3{ 0.f } } }; }
        BasicMesh(const BasicMesh&) = delete;
        BasicMesh(BasicMesh&& rhs);

        uint32 triangles() const { return faceVertices.size(); }
        uint32 vertices() const { return vertexPositions.size(); }

        // vertex data
        std::vector<float3> vertexPositions;
        std::vector<float3> vertexNormals;
        std::vector<VertexFace> vertexFaces;
        std::vector<VertexFaceNode> vfNodes;

        // face data
        std::vector<Face> faceVertices;

        // meta data
        std::array<float3, 2> exts;
    };

    //-----------------------------------//
    // Iterator patterns

    class MeshIterator
    {
    public:
        struct Context
        {
            union
            {
                const MeshIterator* parent;
                uint32 data;
            };

            Context() : data(MeshNullIdx) {}
            Context(uint32 val) : data(val) {}
            Context(const MeshIterator* ptr) : parent(ptr) {}
        };

    public:
        MeshIterator() = default;
        MeshIterator(Context ctx, uint32 idx, BasicMesh* mesh);

        virtual MeshIterator nextVertex(const MeshIterator*) const;
        virtual MeshIterator nextFace(const MeshIterator*) const;

    public:
        Context ctx;
        uint32  idx = MeshNullIdx;
        BasicMesh* mesh = nullptr;
    };

    //-----------------------------------//

    class VertexIterator : public MeshIterator
    {
    public:
        VertexIterator(Context ctx, uint32 idx, BasicMesh* mesh);

        float3& pos();
        const float3& pos() const;

        float& pos(uint32 idx);
        const float& pos(uint32 idx) const;

        float3& normal();
        const float3& normal() const;

        float& normal(uint32 idx);
        const float& normal(uint32 idx) const;

    private:
        virtual MeshIterator nextVertex(const MeshIterator* callee) const;
        virtual MeshIterator nextFace(const MeshIterator* callee) const;
    };

    //-----------------------------------//

    class FaceIterator : public MeshIterator
    {
    public:
        FaceIterator(Context ctx, uint32 idx, BasicMesh* mesh);

        Face& value();
        const Face& value() const;

        float3& vtx(uint32 idx);
        const float3& vtx(uint32 idx) const;

        float3& normal(uint32 idx);
        const float3& normal(uint32 idx) const;

        float3 normal() const;

    private:
        virtual MeshIterator nextVertex(const MeshIterator* callee) const;
        virtual MeshIterator nextFace(const MeshIterator* callee) const;
    };

    //-----------------------------------//
    // mesh traversal dsel

    namespace mesh {

        //-----------------------------------//

        VertexIterator vertices(BasicMesh& mesh);
        VertexIterator vertices(const FaceIterator& face);

        //-----------------------------------//

        FaceIterator faces(BasicMesh& mesh);
        FaceIterator faces(const VertexIterator& vtx);

        //-----------------------------------//

        VertexIterator begin(VertexIterator& vertices);
        FaceIterator begin(FaceIterator& faces);

        //-----------------------------------//

        VertexIterator next(const VertexIterator& vtx);
        FaceIterator next(const FaceIterator& face);

        //-----------------------------------//

        bool null(const VertexIterator& vtx);
        bool null(const FaceIterator& face);

        //-----------------------------------//

        VertexIterator end(const VertexIterator&);
        FaceIterator end(const FaceIterator&);

        //-----------------------------------//

        typedef std::function<VertexIterator(VertexIterator&)> VertexProcessor;
        typedef std::function<FaceIterator(FaceIterator&)> FaceProcessor;

        void walk(VertexIterator vertices, VertexProcessor func);
        void walk(FaceIterator faces, FaceProcessor func);
    }
}