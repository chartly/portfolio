/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "MathExt.h"

#include <memory>

//-----------------------------------//

namespace glmesh { class Mesh; }

//-----------------------------------//

namespace dit {

    //-----------------------------------//

    class BasicMesh;
    class Model;

    typedef std::shared_ptr<BasicMesh> BasicMeshPtr;
    typedef std::shared_ptr<glmesh::Mesh> GlMeshPtr;
    typedef std::shared_ptr<Model> ModelPtr;

    //-----------------------------------//

    class Model
    {
    public:
        Model() = default;
        Model(BasicMeshPtr mesh, GlMeshPtr drawable, float3 pos, float3 scale, float theta = 0.f);

        float4x4 world() const;

    public:
        BasicMeshPtr meshData;
        GlMeshPtr drawable;

        float3 pos{ zero<float3>() };
        float3 scale{ one<float3>() };
        float3 rotationAxis{ 0.f, 1.f, 0.f };
        float theta = 0.f;
    };

    //-----------------------------------//

}