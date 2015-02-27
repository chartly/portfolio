/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include <glload/gl_4_0.hpp>
#include <glload/gl_load.hpp>

#include "Hash.h"
#include "Shader.h"

#include <fstream>

namespace dit
{
    //-----------------------------------//

    void Uniform::set(float v)
    {
        gl::Uniform1f(location, v);
    }

    //-----------------------------------//

    void Uniform::set(float2 v)
    {
        gl::Uniform2f(location, v[0], v[1]);
    }

    //-----------------------------------//

    void Uniform::set(float3 v)
    {
        gl::Uniform3f(location, v[0], v[1], v[2]);
    }

    //-----------------------------------//

    void Uniform::set(float4 v)
    {
        gl::Uniform4f(location, v[0], v[1], v[2], v[3]);
    }

    //-----------------------------------//

    void Uniform::set(const float4x4& m)
    {
        gl::UniformMatrix4fv(location, 1, gl::FALSE_, &m[0][0]);
    }

    //-----------------------------------//

    void Uniform::set(const std::vector<float4x4>& ms)
    {
        gl::UniformMatrix4fv(location, ms.size(), gl::FALSE_, &(ms.front())[0][0]);
    }

    //-----------------------------------//

    void Uniform::set(const float3x3& m)
    {
        gl::UniformMatrix3fv(location, 1, gl::FALSE_, &m[0][0]);
    }

    //-----------------------------------//
    
    void Uniform::set(const std::vector<float3x3>& ms)
    {
        gl::UniformMatrix3fv(location, ms.size(), gl::FALSE_, &(ms.front())[0][0]);
    }

    //-----------------------------------//
    
    void Uniform::set(const std::vector<float4>& vs)
    {
        gl::Uniform4fv(location, vs.size(), &(vs.front())[0]);
    }

    //-----------------------------------//
    
    void Uniform::set(const std::vector<float3>& vs)
    {
        gl::Uniform3fv(location, vs.size(), &(vs.front())[0]);
    }

    //-----------------------------------//
   
    void Uniform::set(const std::vector<float2>& vs)
    {
        gl::Uniform2fv(location, vs.size(), &(vs.front())[0]);
    }

    //-----------------------------------//

    void Uniform::set(const std::vector<float>& vs)
    {
        gl::Uniform1fv(location, vs.size(), vs.data());
    }

    //-----------------------------------//

    Shader::Shader(String name_, uint32 id_)
        : name(std::move(name_))
        , id(id_)
    {
        // extract active uniforms
        int32 count, len, written;
        gl::GetProgramiv(id, gl::ACTIVE_UNIFORM_MAX_LENGTH, &len);
        gl::GetProgramiv(id, gl::ACTIVE_UNIFORMS, &count);

        std::vector<char> buffer;
        buffer.resize(len);

        data.reserve(count);
        metadata.reserve(count);
        uniforms.reserve(count);

        for (int32 i = 0; i < count; ++i)
        {
            UniformMetadata umd;
            Uniform u;

            gl::GetActiveUniform(id, i, len, &written, &umd.size, &umd.type, buffer.data());
            u.location = gl::GetUniformLocation(id, buffer.data());
            umd.name = buffer.data();

            data.push_back(u);
            metadata.push_back(umd);

            uint64 hash = MurmurHash64(umd.name.c_str(), umd.name.size(), 0);
            uniforms.set(hash, i);
        }
    }

    //-----------------------------------//
}