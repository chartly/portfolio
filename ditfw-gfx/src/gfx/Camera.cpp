/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "Camera.h"

namespace dit {

    const float3& Camera::position() const
    {
        return _position;
    }

    float3& Camera::position()
    {
        return _position;
    }

    const float& Camera::scale() const
    {
        return _scale;
    }

    float& Camera::scale()
    {
        return _scale;
    }

    const float& Camera::fov() const
    {
        return _fov;
    }

    float& Camera::fov()
    {
        return _fov;
    }

    const float& Camera::nearPlane() const
    {
        return _nearPlane;
    }

    float& Camera::nearPlane()
    {
        return _nearPlane;
    }

    const float& Camera::farPlane() const
    {
        return _farPlane;
    }

    float& Camera::farPlane()
    {
        return _farPlane;
    }

    const float4x4& Camera::rotation() const
    {
        return _rotation;
    }

    float4x4& Camera::rotation()
    {
        return _rotation;
    }

    const float4x4& Camera::inverse() const
    {
        return _rotationInv;
    }

    float4x4& Camera::inverse()
    {
        return _rotationInv;
    }

    const float& Camera::aspect() const
    {
        return _viewAspectRatio;
    }

    float& Camera::aspect()
    {
        return _viewAspectRatio;
    }

    float3 Camera::forward() const
    {
        float4 f = _rotationInv * float4(0.f, 0.f, -1.f, 0.f);
        return float3(f.x, f.y, f.z);
    }

    float3 Camera::right() const
    {
        float4 r = _rotationInv * float4(1.f, 0.f, 0.f, 0.f);
        return float3(r.x, r.y, r.z);
    }

    float3 Camera::up() const
    {
        float4 u = _rotationInv * float4(0.f, 1.f, 0.f, 0.f);
        return float3(u.x, u.y, u.z);
    }

    float4x4 Camera::viewProjection() const
    {
        return projection() * view();
    }

    float4x4 Camera::projection() const
    {
        float4x4 proj = perspective<float>(_fov, _viewAspectRatio, _nearPlane, _farPlane);
        return proj;
    }

    float4x4 Camera::view() const
    {
        return _rotation;
    }

    float4x4 Camera::world() const
    {
        float4x4 w = translate<float>(-_position.x, -_position.y, -_position.z);
        return w;
    }
    
    float4x4 Camera::model() const
    {
        float4x4 m = glm::scale(float3{ _scale, _scale, _scale });
        return m;
    }
}