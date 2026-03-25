#pragma once
#include "string.hpp"
#include "HashMap.hpp"
#include "Hash.hpp"
#include "lgfx/lgfx.h"

#define SIGNATURE_STACK_ARRAY_MAX 8

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
        string name;
        LGFXFunctionVariableMetadata resource;

        inline void deinit()
        {
            name.deinit();
        }
        inline ShaderResource Clone(IAllocator allocator) const
        {
            ShaderResource result;
            result.name = name.Clone(allocator);
            result.resource = resource;
            return result;
        }
    };
    struct ShaderResourceSet
    {
        ShaderResource *resources;
        u32 resourcesCount;
        u32 setIndex;
    };
    struct ShaderResourceState
    {
        ShaderResource data;
        collections::List<LGFXFunctionVariable> variableSlots;

        inline void deinit()
        {
            for (u32 i = 0; i < variableSlots.count; i++)
            {
                LGFXDestroyFunctionVariable(variableSlots[i]);
            }
            data.deinit();
            variableSlots.deinit();
        }
    };

    struct BatchTemplateSignature
    {
        uint32_t numVariables;
        union
        {
            struct
            {
                IAllocator allocator;
                LGFXFunctionVariableMetadata *ptr;
            } hashAsPtr;
            LGFXFunctionVariableMetadata hashAsArray[SIGNATURE_STACK_ARRAY_MAX];
        };
    };

    inline uint32_t BatchTemplateSignatureHash(BatchTemplateSignature self)
    {
        const LGFXFunctionVariableMetadata *vars;
        if (self.numVariables <= SIGNATURE_STACK_ARRAY_MAX)
        {
            vars = self.hashAsArray;
        }
        else
        {
            vars = self.hashAsPtr.ptr;
        }

        return Murmur3((const uint8_t *)vars, self.numVariables * sizeof(LGFXFunctionVariableMetadata));
    }
    inline bool BatchTemplateSignatureEqls(BatchTemplateSignature A, BatchTemplateSignature B)
    {
        if (A.numVariables != B.numVariables)
        {
            return false;
        }
        const LGFXFunctionVariableMetadata *vars1;
        const LGFXFunctionVariableMetadata *vars2;
        
        if (A.numVariables <= SIGNATURE_STACK_ARRAY_MAX)
        {
            vars1 = A.hashAsArray;
            vars2 = B.hashAsArray;
        }
        else
        {
            vars1 = A.hashAsPtr.ptr;
            vars2 = B.hashAsPtr.ptr;
        }

        return memcmp(vars1, vars2, sizeof(LGFXFunctionVariableMetadata) * A.numVariables) == 0;
    }

    struct BatchTemplateStore
    {
        LGFXDevice device;
        collections::HashMap<BatchTemplateSignature, LGFXFunctionVariableBatchTemplate> allTemplates;
        collections::List<LGFXFunctionVariableMetadata> tempMetadataStore;

        BatchTemplateStore();
        BatchTemplateStore(IAllocator allocator, LGFXDevice device);

        LGFXFunctionVariableBatchTemplate GetOrCreate(LGFXFunctionVariableMetadata *variablesRequired, uint32_t numVariablesRequired);
        LGFXFunctionVariableBatchTemplate GetOrCreate(ShaderResource *variablesRequired, uint32_t numVariablesRequired);
    };

    extern BatchTemplateStore globalTemplateStore;
}