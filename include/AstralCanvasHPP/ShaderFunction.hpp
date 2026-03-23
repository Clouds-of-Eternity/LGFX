#pragma once
#include "Allocators.hpp"
#include "lgfx/lgfx.h"
#include "DenseSet.hpp"
#include "List.hpp"
#include "ByteStreamOps.hpp"

#ifdef ASTRALCANVAS_JSON_SHADER
#include "Json.hpp"
#endif

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
    struct ShaderResource
    {
        string nameStr;
        LGFXShaderResource resource;

        void deinit();
    };
    struct ShaderResourceState
    {
        collections::List<LGFXFunctionVariable> variableSlots;
    };

    struct ShaderFunction
    {
        IAllocator allocator;
        LGFXDevice device;
        LGFXFunction gpuFunction;
        LGFXFunctionType functionType;
        collections::Array<ShaderResource> uniforms;

        ShaderFunction();
        ShaderFunction(IAllocator allocator, usize arrayLength);
        void deinit();

        i32 GetVariableBinding(text variableName);
    };

    struct ShaderFunctionState
    {
        IAllocator allocator;
        AstralCanvas::ShaderFunction *shader;
        u32 currentGroup;

        ShaderResourceState *shaderResourceStates;
        collections::List<LGFXFunctionVariableBatch> variableSlotGroups;

        ShaderFunctionState();
        ShaderFunctionState(IAllocator allocator, ShaderFunction *forShader);
        void deinit();

        void CheckDescriptorSetAvailability(bool forceAddNewDescriptor = false);
        void SyncUniformsWithGPU(LGFXCommandBuffer commandBuffer, bool pushToUsedShaderStack = true);

        void SetUniform(const char* variableName, void* ptr, usize size);
        void SetTexture(const char* variableName, LGFXTexture texture);
        void SetTextures(const char* variableName, LGFXTexture *textures, usize count);
        void SetSampler(const char* variableName, LGFXSamplerState sampler);
        void SetSamplers(const char* variableName, LGFXSamplerState *samplers, usize count);
        void SetComputeBuffer(const char* variableName, LGFXBuffer computeBuffer);
        inline void Clear()
        {
            currentGroup = 0;
        }
    };

    #ifdef ASTRALCANVAS_JSON_SHADER
    //u32 ParseShaderVariables(Json::JsonElement *json, ShaderVariables *results, LGFXShaderInputAccessFlags accessedByShaderOfType);

    usize CreateShaderFromString2(LGFXDevice device, IAllocator allocator, string jsonString, ShaderFunction *result);
    //usize CreateShaderFromString(LGFXDevice device, IAllocator allocator, string jsonString, ShaderFunction *result);
    #endif

    usize CreateShaderFromSFNFilePath(LGFXDevice device, IAllocator allocator, const char *name, u32 numExtraBatchTypes, LGFXFunctionVariableBatchTemplate *extraBatchTypes, ShaderFunction *result);
    usize CreateShaderFromSFNBytes(LGFXDevice device, IAllocator allocator, const u8 *bytes, u32 numExtraBatchTypes, LGFXFunctionVariableBatchTemplate *extraBatchTypes, ShaderFunction *result);
    usize CreateShaderFromSFN(LGFXDevice device, IAllocator allocator, IDataStream input, u32 numExtraBatchTypes, LGFXFunctionVariableBatchTemplate *extraBatchTypes, ShaderFunction *result);
}