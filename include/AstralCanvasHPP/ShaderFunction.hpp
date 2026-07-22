#pragma once
#include "Allocators.hpp"
#include "lgfx/lgfx.h"
#include "DenseSet.hpp"
#include "List.hpp"
#include "ByteStreamOps.hpp"
#include "AstralCanvasHPP/BatchTemplateStore.hpp"

namespace AstralCanvas
{

    struct ShaderFunction
    {
        IAllocator allocator;
        LGFXDevice device;
        LGFXFunction gpuFunction;
        LGFXFunctionType functionType;
        collections::Array<ShaderResourceSet> resourceSets;
        string baseName;

        ShaderFunction();
        ShaderFunction(IAllocator allocator, LGFXDevice device);
        void deinit();

        i32 GetVariableBinding(i32 set, text variableName);
        bool GetVariableBindingAndSet(text variableName, i32 &outputSetIndex, i32 &outputBindingIndex);
    };

    struct ShaderFunctionState
    {
        LGFXFunctionVariableBatchTemplate batchTemplate;
        LGFXFunctionVariable *stagingVariables;

        collections::Array<ShaderResourceState> resourceStates;
        collections::List<LGFXFunctionVariableBatch> variableSlotGroups;
        LGFXDevice device;
        u32 currentGroup;
        bool ownsBatchTemplate;

        ShaderFunctionState();

        static ShaderFunctionState FromShader(IAllocator allocator, LGFXDevice device, const ShaderFunction *function, u32 setIndex);
        static ShaderFunctionState FromShaderAndBatchTemplateSource(IAllocator allocator, LGFXDevice device, BatchTemplateStore *templateStoreSource, const ShaderFunction *function, u32 setIndex);
        static ShaderFunctionState FromVariablesAndBatchTemplateSource(IAllocator allocator, LGFXDevice device, BatchTemplateStore *templateStoreSource, AstralCanvas::ShaderResource *variables, u32 variablesCount);
        
        void deinit();

        void CheckDescriptorSetAvailability(bool forceAddNewDescriptor = false);
        void SyncUniformsWithGPU(LGFXCommandBuffer commandBuffer, ShaderFunction *useForFunction = NULL, u32 setIndex = 0);

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

    usize CreateShaderFromSFNFilePath(LGFXDevice device, IAllocator allocator, const char *name, ShaderFunction *result);
    usize CreateShaderFromSFNBytes(LGFXDevice device, IAllocator allocator, const u8 *bytes, ShaderFunction *result);
    usize CreateShaderFromSFN(LGFXDevice device, IAllocator allocator, IDataStream input, ShaderFunction *result);
}