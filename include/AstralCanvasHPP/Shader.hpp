#pragma once
#include "Allocators.hpp"
#include "lgfx/lgfx.h"
#include "DenseSet.hpp"
#include "List.hpp"
#include "Json.hpp"
#include "DataStream.hpp"

namespace AstralCanvas
{
    enum ShaderFunctionResourceType
    {
        ShaderFunctionResourceType_Uniform,
        ShaderFunctionResourceType_Sampler,
        ShaderFunctionResourceType_Texture,
        ShaderFunctionResourceType_StructuredBuffer,
        ShaderFunctionResourceType_InputAttachment,
        ShaderFunctionResourceType_StorageTexture,
        ShaderFunctionResourceType_Unknown = 0xFFFFFFFF
    };
    enum ShaderFunctionStage
    {
        ShaderFunctionStage_Vertex,
        ShaderFunctionStage_Fragment,
        ShaderFunctionStage_Compute
    };
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
        collections::List<LGFXFunctionVariable> states;

        void deinit();
    };

    typedef collections::List<AstralCanvas::ShaderResource> ShaderVariables;

    struct Shader
    {
        IAllocator allocator;
        LGFXDevice device;
        LGFXFunction gpuFunction;
        LGFXFunctionType functionType;
        ShaderVariables uniforms;

        usize descriptorForThisDrawCall;
        collections::List<LGFXFunctionVariableBatch> variableBatches;
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

    u32 ParseShaderVariables2(IAllocator allocator, Json::JsonElement *variables, collections::List<AstralCanvas::ShaderResource> &result);
    u32 ParseShaderVariables(Json::JsonElement *json, ShaderVariables *results, LGFXShaderInputAccessFlags accessedByShaderOfType);

    usize CreateShaderFromString2(LGFXDevice device, IAllocator allocator, string jsonString, Shader *result);
    usize CreateShaderFromString(LGFXDevice device, IAllocator allocator, string jsonString, Shader *result);

    usize CreateShaderFromSFNFilePath(LGFXDevice device, IAllocator allocator, const char *name, Shader *result);
    usize CreateShaderFromSFNBytes(LGFXDevice device, IAllocator allocator, const u8 *bytes, Shader *result);
    usize CreateShaderFromSFN(LGFXDevice device, IAllocator allocator, IDataStream input, Shader *result);

    extern collections::List<Shader *> allUsedShaders;
}