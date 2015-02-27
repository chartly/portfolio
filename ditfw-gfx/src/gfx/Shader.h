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
#include "HashMap.h"

#include <memory>
#include <vector>

namespace dit {

    //-----------------------------------//

    class UniformMetadata
    {
    public:
        int32 size;
        uint32 type;
        String name;
    };

    //-----------------------------------//

    class Uniform
    {
    public:

        void set(float v);
        void set(float2 v);
        void set(float3 v);
        void set(float4 v);
        void set(const float4x4& m);
        void set(const float3x3& m);
        void set(const std::vector<float4x4>& ms);
        void set(const std::vector<float3x3>& ms);
        void set(const std::vector<float4>& vs);
        void set(const std::vector<float3>& vs);
        void set(const std::vector<float2>& vs);
        void set(const std::vector<float>& vs);

    public:
        uint32 location;
    };

    //-----------------------------------//

    class Shader
    {
    public:
        Shader() = default;
        Shader(const Shader&) = default;
        Shader(Shader&& rhs)
            : data(std::move(rhs.data))
            , metadata(std::move(rhs.metadata))
        {}

        Shader(String progName, uint32 progId);

    public:
        template<typename T>
        void set(const String& uniformName, T val)
        {
            auto hash = MurmurHash64(uniformName.c_str(), uniformName.size(), 0);
            if (!uniforms.has(hash))
            {
                log_err("invalid uniform [\"%0\"] { \"%1\" }", name.c_str(), uniformName.c_str());
                return;
            }

            auto idx = uniforms.get(hash, std::numeric_limits<uint32>::max());
            assert(idx != std::numeric_limits<uint32>::max());
            data[idx].set(std::forward<T>(val));
        }

    public:
        uint32 id;
        HashMap<uint32> uniforms;
        std::vector<Uniform> data;
        std::vector<UniformMetadata> metadata;
        String name;
    };

    //-----------------------------------//

    typedef std::shared_ptr<Shader> ShaderPtr;
    typedef std::vector<ShaderPtr> Shaders;
    typedef HashMap<ShaderPtr> ShaderMap;

    //-----------------------------------//

}