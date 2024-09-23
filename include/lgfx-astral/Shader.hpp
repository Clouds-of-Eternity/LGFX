#pragma once
#include "allocators.hpp"
#include "lgfx/lgfx.h"
#include "denseset.hpp"
#include "vector.hpp"
#include "Json.hpp"

namespace LGFX
{
    struct ShaderStagingMutableState
    {
        union
        {
            struct
            {
                LGFXBuffer ub;
                bool ownsUniformBuffer;
            };
            LGFXBuffer computeBuffer;
            struct
            {
                collections::Array<LGFXTexture> textures;
                void *imageInfos;
            };
            struct
            {
                collections::Array<LGFXSamplerState> samplers;
                void *samplerInfos;
            };
        };
        bool mutated;
    };
    struct ShaderResource
    {
        string nameStr;
        LGFXShaderResource resource;
        collections::vector<ShaderStagingMutableState> states;
    };

    typedef collections::denseset<LGFX::ShaderResource> ShaderVariables;

    struct Shader
    {
        IAllocator allocator;
        LGFXFunction gpuFunction;
        LGFXShaderState shaderState;
        ShaderVariables uniforms;

        usize descriptorForThisDrawCall;
        collections::vector<void *> descriptorSets;

        Shader();
        Shader(IAllocator allocator);

        i32 GetVariableBinding(text variableName);
        void CheckDescriptorSetAvailability(bool forceAddNewDescriptor = false);
        void SyncUniformsWithGPU(void *commandEncoder);

        void SetShaderVariable(const char* variableName, void* ptr, usize size);
        void SetShaderVariableTexture(const char* variableName, LGFXTexture texture);
        void SetShaderVariableTextures(const char* variableName, LGFXTexture *textures, usize count);
        void SetShaderVariableSampler(const char* variableName, LGFXSamplerState sampler);
        void SetShaderVariableSamplers(const char* variableName, LGFXSamplerState *samplers, usize count);
        void SetShaderVariableComputeBuffer(const char* variableName, LGFXBuffer computeBuffer);
    };

    void ParseShaderVariables(Json::JsonElement *json, ShaderVariables *results, LGFXShaderInputAccessFlags accessedByShaderOfType);

    i32 CreateShaderFromString(IAllocator allocator, string jsonString, Shader *result);
}