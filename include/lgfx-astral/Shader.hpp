#pragma once
#include "allocators.hpp"
#include "lgfx/lgfx.h"
#include "denseset.hpp"
#include "vector.hpp"
#include "Json.hpp"

namespace AstralCanvas
{
    struct ShaderMaterialExportParam
    {
        string name;
        usize size;
    };
    struct ShaderMaterialExport
    {
        string name;
        collections::Array<ShaderMaterialExportParam> params;

        inline void deinit()
        {
            name.deinit();
            for (usize i = 0; i < params.length; i++)
            {
                params.data[i].name.deinit();
            }
            params.deinit();
        }
    };
    struct ShaderResource
    {
        string nameStr;
        LGFXShaderResource resource;
        collections::vector<LGFXFunctionVariable> states;

        void deinit();
    };

    typedef collections::denseset<AstralCanvas::ShaderResource> ShaderVariables;

    struct Shader
    {
        IAllocator allocator;
        LGFXDevice device;
        LGFXFunction gpuFunction;
        LGFXFunctionType functionType;
        ShaderVariables uniforms;

        usize descriptorForThisDrawCall;
        collections::vector<LGFXFunctionVariableBatch> variableBatches;
        collections::Array<ShaderMaterialExport> usedMaterials;

        Shader();
        Shader(IAllocator allocator);
        void deinit();

        i32 GetVariableBinding(text variableName);
        void CheckDescriptorSetAvailability(bool forceAddNewDescriptor = false);
        void SyncUniformsWithGPU(LGFXCommandBuffer commandBuffer, bool pushToUsedShaderStack = true);

        void SetShaderVariable(const char* variableName, void* ptr, usize size);
        void SetShaderVariableTexture(const char* variableName, LGFXTexture texture);
        void SetShaderVariableTextures(const char* variableName, LGFXTexture *textures, usize count);
        void SetShaderVariableSampler(const char* variableName, LGFXSamplerState sampler);
        void SetShaderVariableSamplers(const char* variableName, LGFXSamplerState *samplers, usize count);
        void SetShaderVariableComputeBuffer(const char* variableName, LGFXBuffer computeBuffer);
    };

    u32 ParseShaderVariables(Json::JsonElement *json, ShaderVariables *results, LGFXShaderInputAccessFlags accessedByShaderOfType);

    usize CreateShaderFromString(LGFXDevice device, IAllocator allocator, string jsonString, Shader *result);

    extern collections::vector<Shader *> allUsedShaders;
}