/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "Model.h"

namespace dit {

    //-----------------------------------//

    Model::Model(BasicMeshPtr mesh_, GlMeshPtr drawable_, float3 pos_, float3 scale_, float theta_ /* = 0.f */)
        : meshData(mesh_)
        , drawable(drawable_)
        , pos(pos_)
        , scale(scale_)
        , theta(theta_)
    {}

    //-----------------------------------//

    float4x4 Model::world() const
    {
        float4x4 t = translate(-1.f * pos);
        float4x4 r = rotate(theta, rotationAxis);
        float4x4 m = glm::scale(scale);

        return m * r * t;
    }

    //-----------------------------------//

}