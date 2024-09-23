#pragma once
#include "allocators.hpp"
#include "lgfx/lgfx.h"
#include "denseset.hpp"
#include "vector.hpp"
#include "Json.hpp"

namespace LGFX
{
    struct ShaderResource
    {
        string nameStr;
        LGFXShaderResource resource;
        collections::vector<LGFXFunctionVariable> states;

        void deinit();
    };

    typedef collections::denseset<LGFX::ShaderResource> ShaderVariables;

    struct Shader
    {
        IAllocator allocator;
        LGFXDevice device;
        LGFXFunction gpuFunction;
        ShaderVariables uniforms;

        usize descriptorForThisDrawCall;
        collections::vector<void *> descriptorSets;

        Shader();
        Shader(IAllocator allocator);
        void deinit();

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

    u32 ParseShaderVariables(Json::JsonElement *json, ShaderVariables *results, LGFXShaderInputAccessFlags accessedByShaderOfType);

    usize CreateShaderFromString(LGFXDevice device, IAllocator allocator, string jsonString, Shader *result);
}