#include "AstralCanvasHPP/BatchTemplateStore.hpp"

namespace AstralCanvas
{
    BatchTemplateStore globalTemplateStore;
    
    BatchTemplateStore::BatchTemplateStore()
    {
        device = NULL;
        tempMetadataStore = collections::List<LGFXFunctionVariableMetadata>();
        allTemplates = collections::HashMap<BatchTemplateSignature, LGFXFunctionVariableBatchTemplate>();
    }
    BatchTemplateStore::BatchTemplateStore(IAllocator allocator, LGFXDevice device)
    {
        this->device = device;
        tempMetadataStore = collections::List<LGFXFunctionVariableMetadata>(allocator, 16);
        allTemplates = collections::HashMap<BatchTemplateSignature, LGFXFunctionVariableBatchTemplate>(allocator, &BatchTemplateSignatureHash, &BatchTemplateSignatureEqls);
    }

    void BatchTemplateStore::deinit()
    {
        auto iterator = allTemplates.GetIterator();
        foreach (kvp, iterator)
        {
            kvp->key.deinit();
            LGFXDestroyFunctionVariableBatchTemplate(device, kvp->value);
        }
    }
    LGFXFunctionVariableBatchTemplate BatchTemplateStore::GetOrCreate(LGFXFunctionVariableMetadata *variablesRequired, uint32_t numVariablesRequired)
    {
        BatchTemplateSignature signature = {};
        signature.numVariables = numVariablesRequired;
        const bool needsPtr = numVariablesRequired > SIGNATURE_STACK_ARRAY_MAX;
        if (needsPtr)
        {
            signature.hashAsPtr.ptr = variablesRequired;
            signature.hashAsPtr.allocator = this->allTemplates.allocator;
        }
        else
        {
            memcpy(signature.hashAsArray, variablesRequired, sizeof(LGFXFunctionVariableMetadata) * numVariablesRequired);
        }

        LGFXFunctionVariableBatchTemplate result = allTemplates.GetCopyOr(signature, NULL);
        if (result == NULL)
        {
            LGFXFunctionVariableBatchTemplateCreateInfo createInfo = {};
            createInfo.variablesCount = numVariablesRequired;

            if (needsPtr)
            {
                //take ownership of pointer
                signature.hashAsPtr.ptr = (LGFXFunctionVariableMetadata *)signature.hashAsPtr.allocator.Allocate(sizeof(LGFXFunctionVariableMetadata) * numVariablesRequired);
                memcpy(signature.hashAsPtr.ptr, variablesRequired, numVariablesRequired * sizeof(LGFXFunctionVariableMetadata));
                
                createInfo.variables = signature.hashAsPtr.ptr;
            }
            else
            {
                createInfo.variables = signature.hashAsArray;
            }

            result = LGFXCreateFunctionVariableBatchTemplate(device, &createInfo);

            allTemplates.Add(signature, result);
            if (needsPtr)
            {
                free(createInfo.variables);
            }
        }

        return result;
    }
    LGFXFunctionVariableBatchTemplate BatchTemplateStore::GetOrCreate(AstralCanvas::ShaderResource *variablesRequired, uint32_t numVariablesRequired)
    {
        for (u32 i = 0; i < numVariablesRequired; i++)
        {
            tempMetadataStore.Add(variablesRequired[i].resource);
        }
        LGFXFunctionVariableBatchTemplate result = this->GetOrCreate(tempMetadataStore.ptr, numVariablesRequired);
        tempMetadataStore.Clear();

        return result;
    }
}