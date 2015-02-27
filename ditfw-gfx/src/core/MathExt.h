/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace dit {

    // fundamental math types
    using glm::float1;
    using glm::float2;
    using glm::float3;
    using glm::float4;
    using glm::float3x3;
    using glm::float4x4;

    // common math functions
    using glm::degrees;
    using glm::sqrt;
    using glm::inversesqrt;
    using glm::cross;
    using glm::dot;
    using glm::length;
    using glm::length2;
    using glm::normalize;
    using glm::translate;
    using glm::rotate;
    using glm::lookAt;
    using glm::perspective;
    using glm::scale;
    using glm::inverse;
    using glm::transpose;
    using glm::inverseTranspose;

    // math constants
    using glm::zero;
    using glm::one;
    using glm::pi;
}