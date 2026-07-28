#include "AstralCanvasHPP/ShaderFunction.hpp"
#include <math.h>
#include <stdio.h>
#include "Scope.hpp"

namespace AstralCanvas
{
    ShaderFunction::ShaderFunction()
    {
        this->allocator = IAllocator{};
        device = NULL;
        gpuFunction = NULL;
        resourceSets = collections::Array<ShaderResourceSet>();
        functionType = LGFXFunctionType_Unknown;
        baseName = string();
    }
    ShaderFunction::ShaderFunction(IAllocator allocator, LGFXDevice device)
    {
        this->allocator = allocator;
        this->device = device;
        gpuFunction = NULL;
        resourceSets = collections::Array<ShaderResourceSet>();
        functionType = LGFXFunctionType_Unknown;
        baseName = string();
    }
    void ShaderFunction::deinit()
    {
        LGFXDestroyFunction(gpuFunction);
        if (resourceSets.data != NULL)
        {
            for (u32 i = 0; i < resourceSets.length; i++)
            {
                for (u32 j = 0; j < resourceSets[i].resourcesCount; j++)
                {
                    resourceSets[i].resources[j].deinit();
                }
                allocator.Free(resourceSets[i].resources);
            }
            resourceSets.deinit();
        }
        baseName.deinit();
    }

    void ShaderFunctionState::SetComputeBuffer(const char* variableName, LGFXBuffer buffer)
    {
        CheckDescriptorSetAvailability();
        for (usize i = 0; i < resourceStates.length; i++)
        {
            if (resourceStates[i].data.name.buffer == NULL)
            {
                break;
            }
            
            if (resourceStates[i].data.name == variableName)
            {
                ((LGFXBuffer *)resourceStates[i].variableSlots.ptr[currentGroup].currentValues)[0] = buffer;
                break;
            }
        }
    }
    void ShaderFunctionState::SetUniform(const char* variableName, void* ptr, usize size)
    {
        CheckDescriptorSetAvailability();
        for (usize i = 0; i < resourceStates.length; i++)
        {
            if (resourceStates[i].data.name.buffer == NULL)
            {
                break;
            }
            
            if (resourceStates[i].data.name == variableName)
            {
                LGFXSetBufferDataFast(((LGFXBuffer *)resourceStates[i].variableSlots.ptr[currentGroup].currentValues)[0], (u8*)ptr, 0, size);
                return;
                //uniforms.data[i].states.ptr[descriptorForThisDrawCall].ub.SetData(ptr, size);
            }
        }
        fprintf(stderr, "Shader does not possess a variable of name %s\n", variableName);
    }
    void ShaderFunctionState::SetTextures(const char* variableName, LGFXTexture*textures, usize count)
    {
        CheckDescriptorSetAvailability();
        for (usize i = 0; i < resourceStates.length; i++)
        {
            if (resourceStates[i].data.name.buffer == NULL)
            {
                break;
            }
            if (resourceStates[i].data.name == variableName)
            {
                LGFXFunctionVariable *mutableState = &resourceStates[i].variableSlots.ptr[currentGroup];
                for (usize j = 0; j < count; j++)
                {
                   ((LGFXTexture *)mutableState->currentValues)[j] = textures[j];
                }
                return;
            }
        }
        fprintf(stderr, "Shader does not possess a variable of name %s\n", variableName);
    }
    void ShaderFunctionState::SetTexture(const char* variableName, LGFXTexture texture)
    {
        this->SetTextures(variableName, &texture, 1);
    }
    void ShaderFunctionState::SetSamplers(const char* variableName, LGFXSamplerState *samplers, usize count)
    {
        CheckDescriptorSetAvailability();
        for (usize i = 0; i < resourceStates.length; i++)
        {
            if (resourceStates[i].data.name.buffer == NULL)
            {
                break;
            }
            if (resourceStates[i].data.name == variableName)
            {
                LGFXFunctionVariable *mutableState = &resourceStates[i].variableSlots.ptr[currentGroup];
                for (usize j = 0; j < count; j++)
                {
                   ((LGFXSamplerState *)mutableState->currentValues)[j] = samplers[j];
                }
                return;
            }
        }
        fprintf(stderr, "Shader does not possess a variable of name %s\n", variableName);
    }
    void ShaderFunctionState::SetSampler(const char* variableName, LGFXSamplerState sampler)
    {
        this->SetSamplers(variableName, &sampler, 1);
    }

    i32 ShaderFunction::GetVariableBinding(i32 set, text variableName)
    {
        if (set < 0 || set >= resourceSets.length)
        {
            return -1;
        }
        for (i32 i = 0; i < resourceSets[set].resourcesCount; i++)
        {
            if (resourceSets[set].resources[i].name == variableName)
            {
                return i;
            }
        }
        return -1;
    }
    bool ShaderFunction::GetVariableBindingAndSet(text variableName, i32 &outputSetIndex, i32 &outputBindingIndex)
    {
        for (i32 i = 0; i < resourceSets.length; i++)
        {
            for (i32 j = 0; j < resourceSets[i].resourcesCount; j++)
            {
                if (resourceSets[i].resources[j].name == variableName)
                {
                    outputSetIndex = i;
                    outputBindingIndex = j;
                    return true;
                }
            }
        }
        return false;
    }

    ShaderFunctionState::ShaderFunctionState()
    {
        currentGroup = 0;
        this->ownsBatchTemplate = false;
        stagingVariables = NULL;
        resourceStates = collections::Array<ShaderResourceState>();
        variableSlotGroups = collections::List<LGFXFunctionVariableBatch>();
    }

    ShaderFunctionState ShaderFunctionState::FromShader(IAllocator allocator, LGFXDevice device, const ShaderFunction *function, u32 setIndex)
    {
        return FromVariablesAndBatchTemplateSource(allocator, device, NULL, function->resourceSets.data[setIndex].resources, function->resourceSets.data[setIndex].resourcesCount);
    }
    ShaderFunctionState ShaderFunctionState::FromShaderAndBatchTemplateSource(IAllocator allocator, LGFXDevice device, BatchTemplateStore *templateStoreSource, const ShaderFunction *function, u32 setIndex)
    {
        return FromVariablesAndBatchTemplateSource(allocator, device, templateStoreSource, function->resourceSets.data[setIndex].resources, function->resourceSets.data[setIndex].resourcesCount);
    }
    
    ShaderFunctionState ShaderFunctionState::FromVariablesAndBatchTemplateSource(IAllocator allocator, LGFXDevice device, BatchTemplateStore *templateStoreSource, AstralCanvas::ShaderResource *variables, u32 variablesCount)
    {
        ShaderFunctionState result;
        result.device = device;

        if (templateStoreSource == NULL)
        {
            result.ownsBatchTemplate = true;

            LGFXFunctionVariableMetadata *varMetas = (LGFXFunctionVariableMetadata *)malloc(sizeof(LGFXFunctionVariableMetadata) * variablesCount);
            
            result.resourceStates = collections::Array<ShaderResourceState>(allocator, variablesCount);
            for (u32 i = 0; i < variablesCount; i++)
            {
                varMetas[i] = variables[i].resource;
                result.resourceStates[i].data = variables[i].Clone(allocator);
                result.resourceStates[i].variableSlots = collections::List<LGFXFunctionVariable>(allocator);
            }

            LGFXFunctionVariableBatchTemplateCreateInfo createInfo = {};
            createInfo.variablesCount = variablesCount;
            createInfo.variables = varMetas;

            result.batchTemplate = LGFXCreateFunctionVariableBatchTemplate(device, &createInfo);
            
            free(varMetas);
        }
        else
        {
            result.ownsBatchTemplate = false;

            result.resourceStates = collections::Array<ShaderResourceState>(allocator, variablesCount);
            for (u32 i = 0; i < variablesCount; i++)
            {
                result.resourceStates[i].data = variables[i].Clone(allocator);
                result.resourceStates[i].variableSlots = collections::List<LGFXFunctionVariable>(allocator);
            }
            result.batchTemplate = templateStoreSource->GetOrCreate(variables, variablesCount);
        }
        
        result.currentGroup = 0;
        result.stagingVariables = (LGFXFunctionVariable *)allocator.Allocate(sizeof(LGFXFunctionVariable) * variablesCount);
        result.variableSlotGroups = collections::List<LGFXFunctionVariableBatch>(allocator);

        return result;
    }
    void ShaderFunctionState::deinit()
    {
        if (ownsBatchTemplate && batchTemplate != NULL)
        {
            LGFXDestroyFunctionVariableBatchTemplate(device, batchTemplate);
        }
        for (u32 i = 0; i < resourceStates.length; i++)
        {
            resourceStates[i].deinit();
        }
        resourceStates.deinit();

        variableSlotGroups.deinit();
    }
    void ShaderFunctionState::CheckDescriptorSetAvailability(bool forceAddNewDescriptor)
    {
        bool added = false;
        for (u32 i = 0; i < resourceStates.length; i++)
        {
            if (resourceStates[i].data.name.buffer == NULL)
            {
                break;
            }

            if (forceAddNewDescriptor || currentGroup >= variableSlotGroups.count)
            {
                LGFXFunctionVariable newVarSlot = LGFXCreateFunctionVariableSlot(device, batchTemplate, resourceStates[i].data.resource.binding);
                resourceStates[i].variableSlots.Add(newVarSlot);
                added = true;
            }
        }
        if (added)
        {
            LGFXFunctionVariableBatch batch = LGFXCreateFunctionVariableBatchFromTemplate(device, batchTemplate);
            variableSlotGroups.Add(batch);
        }
    }
    void ShaderFunctionState::SyncUniformsWithGPU(LGFXCommandBuffer commandBuffer, ShaderFunction *useForFunction, u32 setIndex)
    {
        u32 variablesCount = 0;
        for (usize i = 0; i < resourceStates.length; i++)
        {
            if (resourceStates[i].data.name.buffer == NULL)
            {
                break;
            }
            stagingVariables[i] = resourceStates[i].variableSlots.ptr[this->currentGroup];
            variablesCount++;
        }
        LGFXFunctionVariableBatch batch = this->variableSlotGroups.ptr[this->currentGroup];
        LGFXFunctionSendVariablesToGPU(device, batch, stagingVariables, variablesCount);
        if (useForFunction != NULL)
        {
            LGFXUseFunctionVariables(commandBuffer, batch, useForFunction->gpuFunction, setIndex);
        }
        currentGroup += 1;
    }

    usize CreateShaderFromSFNFilePath(LGFXDevice device, IAllocator allocator, const char *name, ShaderFunction *result)
    {
        FILE *fs = fopen(name, "rb");
        if (fs == NULL)
        {
            return 1;   
        }
        usize errorCode = CreateShaderFromSFN(device, allocator, GetFileDataStream(fs), result);
        fclose(fs);
        return errorCode;
    }
    usize CreateShaderFromSFNBytes(LGFXDevice device, IAllocator allocator, const u8 *bytes, ShaderFunction *result)
    {
        ByteStreamReader reader = ByteStreamReader(bytes, 0xFFFFFFFF, 0);
        usize errorCode = CreateShaderFromSFN(device, allocator, reader.ToDataStream(), result);
        return errorCode;
    }
    usize CreateShaderFromSFN(LGFXDevice device, IAllocator allocator, IDataStream input, ShaderFunction *result)
    {
        const u32 fileVersion = input.Read<u32>();
        if (fileVersion == 1 || fileVersion == 2)
        {
            ArenaAllocator arena = ArenaAllocator(GetCAllocator());
            Scope(ArenaAllocator, arena);

            const u32 shaderType = input.Read<u32>();
            LGFXFunctionCreateInfo info = {};

            *result = ShaderFunction(allocator, device);
            
            if (fileVersion == 2)
            {
                result->baseName = input.ReadString(allocator);
            }

            const u32 maxSets = input.Read<u32>();
            result->resourceSets = collections::Array<ShaderResourceSet>(allocator, maxSets);
            //LGFXFunctionVariableBatchTemplate templates[16];
            for (u32 j = 0; j < maxSets; j++)
            {
                const u32 paramCount = input.Read<u32>();
                ShaderResource *resources = (ShaderResource *)allocator.Allocate(sizeof(ShaderResource) * paramCount);

                for (u32 i = 0; i < paramCount; i++)
                {
                    string str = input.ReadString(allocator);
                    u32 bindingIndex = input.Read<u32>();

                    AstralCanvas::ShaderResource newResource = {};
                    newResource.name = str;
                    newResource.resource.set = j;
                    newResource.resource.binding = bindingIndex;
                    
                    newResource.resource.arrayLength = input.Read<u32>();

                    ShaderFunctionResourceType resourceType = (ShaderFunctionResourceType)input.Read<u32>();
                    if (resourceType == ShaderFunctionResourceType_Uniform)
                    {
                        newResource.resource.type = LGFXShaderResourceType_Uniform;
                        newResource.resource.size = input.Read<u32>();
                    }
                    else if (resourceType == ShaderFunctionResourceType_Sampler)
                    {
                        newResource.resource.type = LGFXShaderResourceType_Sampler;
                    }
                    else if (resourceType == ShaderFunctionResourceType_Texture)
                    {
                        newResource.resource.type = LGFXShaderResourceType_Texture;
                    }
                    else if (resourceType == ShaderFunctionResourceType_StorageTexture)
                    {
                        newResource.resource.type = LGFXShaderResourceType_StorageTexture;
                    }
                    else if (resourceType == ShaderFunctionResourceType_StructuredBuffer)
                    {
                        newResource.resource.type = LGFXShaderResourceType_StructuredBuffer;
                    }
                    else if (resourceType == ShaderFunctionResourceType_InputAttachment)
                    {
                        newResource.resource.type = LGFXShaderResourceType_InputAttachment;
                    }
                    else
                    {
                        return 1;
                    }
                    
                    resources[i] = newResource;
                }
                result->resourceSets[j].resources = resources;
                result->resourceSets[j].resourcesCount = paramCount;
                result->resourceSets[j].setIndex = j;
            }

            LGFXFunctionType funcType;
            if (shaderType == 0)
            {
                //vertex-fragment
                funcType = (LGFXFunctionType)(LGFXFunctionType_Vertex | LGFXFunctionType_Fragment);

                ShaderFunctionStage stage1 = (ShaderFunctionStage)input.Read<u32>();
                if (stage1 != ShaderFunctionStage_Vertex)
                {
                    return 1;
                }
                usize lenBytes = input.Read<u32>();
                info.module1DataLength = lenBytes / 4;
                info.module1Data = (u32 *)arena.AsAllocator().Allocate(lenBytes);
                input.ReadByteArray((u8*)info.module1Data, lenBytes);
                
                ShaderFunctionStage stage2 = (ShaderFunctionStage)input.Read<u32>();
                if (stage2 != ShaderFunctionStage_Fragment)
                {
                    return 1;
                }
                lenBytes = input.Read<u32>();
                info.module2DataLength = lenBytes / 4;
                info.module2Data = (u32 *)arena.AsAllocator().Allocate(lenBytes);
                input.ReadByteArray((u8*)info.module2Data, lenBytes);
            }
            else if (shaderType == 1)
            {
                //compute
                funcType = LGFXFunctionType_Compute;

                ShaderFunctionStage stage1 = (ShaderFunctionStage)input.Read<u32>();
                if (stage1 != ShaderFunctionStage_Compute)
                {
                    return 1;
                }
                usize lenBytes = input.Read<u32>();

                info.module1DataLength = lenBytes / 4;
                info.module1Data = (u32 *)arena.AsAllocator().Allocate(lenBytes);
                input.ReadByteArray((u8*)info.module1Data, lenBytes);
            }
            else return 1;

            LGFXFunctionVariableBatchTemplate templates[16];

            for (u32 i = 0; i < result->resourceSets.length; i++)
            {
                templates[i] = AstralCanvas::globalTemplateStore.GetOrCreate(result->resourceSets[i].resources, result->resourceSets[i].resourcesCount);
            }

            info.type = funcType;
            info.variableBatchTemplates = templates;
            info.variableBatchTemplatesCount = result->resourceSets.length;

            result->gpuFunction = LGFXCreateFunction(device, &info);
            result->functionType = funcType;
        }
        else
        {
            return 1;
        }
        return 0;
    }
}