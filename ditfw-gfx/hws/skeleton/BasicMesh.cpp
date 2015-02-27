/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "BasicMesh.h"

namespace dit {

    using namespace mesh;

#pragma region Basic structures

    //-----------------------------------//

    VertexFaceNode::VertexFaceNode(uint32 next_, uint32 faceIdx_)
        : next(next_)
        , faceIdx(faceIdx_)
    {}

    //-----------------------------------//

    Face::Face(const std::array<uint32, 3>& vtx_)
        : vtx(vtx_)
    {}

#pragma endregion

    //-----------------------------------//

#pragma region MeshIterator implementation

    MeshIterator::MeshIterator(Context ctx_, uint32 idx_, BasicMesh* mesh_)
        : ctx(ctx_)
        , idx(idx_)
        , mesh(std::move(mesh_))
    {}

    //-----------------------------------//

    MeshIterator MeshIterator::nextVertex(const MeshIterator*) const
    {
        return MeshIterator{};
    }

    //-----------------------------------//

    MeshIterator MeshIterator::nextFace(const MeshIterator*) const
    {
        return MeshIterator{};
    }

#pragma endregion
    
    //-----------------------------------//

#pragma region VertexIterator implementation

    VertexIterator::VertexIterator(Context ctx, uint32 idx, BasicMesh* mesh)
        : MeshIterator(ctx, idx, std::move(mesh))
    {}

    //-----------------------------------//

    float3& VertexIterator::pos()
    {
        return mesh->vertexPositions[idx];
    }

    //-----------------------------------//

    const float3& VertexIterator::pos() const
    {
        return mesh->vertexPositions[idx];
    }

    //-----------------------------------//

    float& VertexIterator::pos(uint32 i)
    {
        assert(i < 3);
        return mesh->vertexPositions[idx][i];
    }

    //-----------------------------------//

    const float& VertexIterator::pos(uint32 i) const
    {
        assert(i < 3);
        return mesh->vertexPositions[idx][i];
    }

    //-----------------------------------//

    float3& VertexIterator::normal()
    {
        return mesh->vertexNormals[idx];
    }

    //-----------------------------------//

    const float3& VertexIterator::normal() const
    {
        return mesh->vertexNormals[idx];
    }

    //-----------------------------------//

    MeshIterator VertexIterator::nextVertex(const MeshIterator* callee) const
    {
        assert(callee != nullptr);
        assert(ctx.data != MeshNullIdx);

        if (idx != MeshNullIdx)
            return ctx.parent->nextVertex(this);

        auto nextIdx = callee->idx + 1;
        if (nextIdx < ctx.data)
            return MeshIterator{ this, nextIdx, mesh };

        return MeshIterator{};
    }

    //-----------------------------------//

    MeshIterator VertexIterator::nextFace(const MeshIterator* callee) const
    {
        assert(callee != nullptr);

        auto nodeIdx = mesh->vfNodes[callee->idx].next;
        auto callee_ = (MeshIterator*)callee;
        callee_->idx = nodeIdx;

        return MeshIterator{ callee, mesh->vfNodes[nodeIdx].faceIdx, mesh };
    }

#pragma endregion

    //-----------------------------------//

#pragma region FaceIterator implementation

    FaceIterator::FaceIterator(Context ctx, uint32 idx, BasicMesh* mesh)
        : MeshIterator(ctx, idx, std::move(mesh))
    {}

    //-----------------------------------//

    Face& FaceIterator::value()
    {
        return mesh->faceVertices[idx];
    }

    //-----------------------------------//

    const Face& FaceIterator::value() const
    {
        return mesh->faceVertices[idx];
    }

    //-----------------------------------//

    float3& FaceIterator::vtx(uint32 vIdx)
    {
        assert(vIdx < 3);
        return mesh->vertexPositions[value().vtx[vIdx]];
    }

    //-----------------------------------//

    const float3& FaceIterator::vtx(uint32 vIdx) const
    {
        assert(vIdx < 3);
        return mesh->vertexPositions[value().vtx[vIdx]];
    }

    //-----------------------------------//

    float3& FaceIterator::normal(uint32 vIdx)
    {
        assert(vIdx < 3);
        return mesh->vertexNormals[value().vtx[vIdx]];
    }

    //-----------------------------------//

    const float3& FaceIterator::normal(uint32 vIdx) const
    {
        assert(vIdx < 3);
        return mesh->vertexNormals[value().vtx[vIdx]];
    }

    //-----------------------------------//

    float3 FaceIterator::normal() const
    {
        float3 n = normal(0) + normal(1) + normal(2);
        n = normalize(n);

        return n;
    }

    //-----------------------------------//

    MeshIterator FaceIterator::nextVertex(const MeshIterator* callee) const
    {
        assert(callee != nullptr);

        auto nextIdx = callee->idx + 1;
        if (nextIdx > 2)
            return MeshIterator{};

        auto callee_ = (MeshIterator*)callee;
        callee_->idx = nextIdx;

        return MeshIterator{ callee, mesh->faceVertices[idx].vtx[nextIdx], mesh };
    }

    //-----------------------------------//

    MeshIterator FaceIterator::nextFace(const MeshIterator* callee) const
    {
        assert(callee != nullptr);
        assert(ctx.data != MeshNullIdx);

        if (idx != MeshNullIdx)
            return ctx.parent->nextFace(this);

        auto nextIdx = callee->idx + 1;
        if (nextIdx < ctx.data)
            return MeshIterator{ this, nextIdx, mesh };

        return MeshIterator{};
    }

#pragma endregion

    //-----------------------------------//

#pragma region DSEL functions

    namespace mesh {

        //-----------------------------------//

        VertexIterator vertices(BasicMesh& mesh)
        {
            return VertexIterator{ mesh.vertices(), MeshNullIdx, &mesh };
        }

        //-----------------------------------//

        VertexIterator vertices(const FaceIterator& face)
        {
            return VertexIterator{ &face, face.mesh->faceVertices[face.idx].vtx[0], face.mesh };
        }

        //-----------------------------------//

        FaceIterator faces(BasicMesh& mesh)
        {
            return FaceIterator{ mesh.triangles(), MeshNullIdx, &mesh };
        }

        //-----------------------------------//

        FaceIterator faces(const VertexIterator& vtx)
        {
            auto nodeIdx = vtx.mesh->vertexFaces[vtx.idx].nodeIdx;
            return FaceIterator{ &vtx, nodeIdx, vtx.mesh };
        }

        //-----------------------------------//

        VertexIterator begin(VertexIterator& verts)
        {
            uint32 idx = 0;
            if (verts.idx != MeshNullIdx)
            {
                idx = verts.idx;
                verts.idx = 0;
            }

            return VertexIterator{ &verts, idx, verts.mesh };
        }

        //-----------------------------------//

        FaceIterator begin(FaceIterator& faces)
        {
            uint32 idx = 0;
            if (faces.idx != MeshNullIdx)
            {
                auto nodeIdx = faces.idx;
                faces.idx = faces.mesh->vfNodes[nodeIdx].next;
                idx = faces.mesh->vfNodes[nodeIdx].faceIdx;
            }

            return FaceIterator{ &faces, idx, faces.mesh };
        }

        //-----------------------------------//

        VertexIterator next(const VertexIterator& vtx)
        {
            if (vtx.ctx.data == MeshNullIdx)
                return end(vtx);

            auto nxt = vtx.ctx.parent->nextVertex(&vtx);
            return VertexIterator{ nxt.ctx, nxt.idx, nxt.mesh };
        }

        //-----------------------------------//

        FaceIterator next(const FaceIterator& face)
        {
            if (face.ctx.data == MeshNullIdx)
                return end(face);

            auto nxt = face.ctx.parent->nextFace(&face);
            return FaceIterator{ nxt.ctx, nxt.idx, nxt.mesh };
        }

        //-----------------------------------//

        const MeshIterator EndIter = MeshIterator{ MeshNullIdx, MeshNullIdx, nullptr };

        //-----------------------------------//

        bool null(const VertexIterator& vtx)
        {
            return (vtx.ctx.data == EndIter.ctx.data && vtx.mesh == EndIter.mesh && vtx.idx == EndIter.idx);
        }

        //-----------------------------------//

        bool null(const FaceIterator& face)
        {
            return (face.ctx.data == EndIter.ctx.data && face.mesh == EndIter.mesh && face.idx == EndIter.idx);
        }

        //-----------------------------------//

        VertexIterator end(const VertexIterator&)
        {
            return VertexIterator{ MeshNullIdx, MeshNullIdx, nullptr };
        }

        //-----------------------------------//

        FaceIterator end(const FaceIterator&)
        {
            return FaceIterator{ MeshNullIdx, MeshNullIdx, nullptr };
        }

        //-----------------------------------//

        void walk(VertexIterator vertices, VertexProcessor func)
        {
            auto vtx = begin(vertices);
            while (!null(vtx))
                vtx = func(vtx);
        }

        //-----------------------------------//

        void walk(FaceIterator faces, FaceProcessor func)
        {
            auto face = begin(faces);
            while (!null(face))
                face = func(face);
        }

        //-----------------------------------//
    }
#pragma endregion

    //-----------------------------------//

}