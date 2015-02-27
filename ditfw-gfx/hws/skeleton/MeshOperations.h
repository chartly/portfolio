/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "MathExt.h"
#include "StringExt.h"

#include <memory>
#include <array>
#include <vector>

namespace dit {

    //-----------------------------------//

    class Model;
    class Plane;
    class BasicMesh;

    //-----------------------------------//

    float IntersectPlaneRay(const Plane& plane, const float3& p0, const float3& p1);
    float DistancePointPlane(const Plane& plane, const float3& pt);

    void ComputeConnectivity(BasicMesh* mesh);

    //-----------------------------------//

    BasicMesh* MeshFromOBJ(const String& file);

    //-----------------------------------//

}