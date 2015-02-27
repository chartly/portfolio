/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "MathExt.h"

namespace dit {

    //-----------------------------------//

    class Model;

    //-----------------------------------//

    class Plane
    {
    public:
        Plane() = default;
        Plane(const float3& p, const float3& n);

        void Transform(const float4x4& mtx);

        float DistanceToPoint(const float3& pt) const;

    public:
        float3 center           = zero<float3>();
        float3 normal           = float3(0.f, 1.f, 0.f);
        float3 pivot            = zero<float3>();

        float pivotRotation     = 0.f;
        float upRotation        = 0.f;
        bool  hasPivot          = false;
    };

    //-----------------------------------//

}