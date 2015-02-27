/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "Plane.h"

namespace dit {

    //-----------------------------------//

    using namespace glm;

    //-----------------------------------//

    Plane::Plane(const float3& pt, const float3& n)
        : center(pt)
        , normal(n)
        , pivot(0, 0, 0)
        , hasPivot(false)
    {}

    //-----------------------------------//

    void Plane::Transform(const float4x4& mtx)
    {
        float4 p = float4(center.x, center.y, center.z, 1.f) * mtx;
        center = float3(p.x, p.y, p.z);

        p = float4(normal.x, normal.y, normal.z, 0.f) * mtx;
        normal = float3(p.x, p.y, p.z);
    }

    //-----------------------------------//

}