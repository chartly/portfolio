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

    class Camera
    {
    public:
        Camera() = default;
        Camera(const float3& pos, float scale, float fov, float near, float far, float viewAspect)
            : _position(pos), _scale(scale), _fov(fov), _nearPlane(near), _farPlane(far), _viewAspectRatio(viewAspect)
        {}

        const float3& position() const;
        float3& position();

        const float& scale() const;
        float& scale();

        const float& fov() const;
        float& fov();

        const float& nearPlane() const;
        float& nearPlane();
        const float& farPlane() const;
        float& farPlane();

        const float4x4& rotation() const;
        float4x4& rotation();
        const float4x4& inverse() const;
        float4x4& inverse();

        const float& aspect() const;
        float& aspect();

        float3 forward() const;
        float3 right() const;
        float3 up() const;

        float4x4 viewProjection() const;
        float4x4 projection() const;
        float4x4 view() const;
        float4x4 world() const;
        float4x4 model() const;

    private:
        float3 _position{ 0.f, 0.f, 0.f };
        float4x4 _rotation{ 1.f };
        float4x4 _rotationInv{ 1.f };
        float _scale = 1.f;
        float _fov = 90.f;
        float _nearPlane = .001f;
        float _farPlane = 1000.f;
        float _viewAspectRatio = 16.f / 9.f;
    };
}