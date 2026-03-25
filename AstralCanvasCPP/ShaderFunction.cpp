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
    }
    ShaderFunction::ShaderFunction(IAllocator allocator, LGFXDevice device)
    {
        this->allocator = allocator;
        this->device = device;
        gpuFunction = NULL;
        resourceSets = collections::Array<ShaderResourceSet>();
        functionType = LGFXFunctionType_Unknown;
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
                LGFXSetBufferDataFast(((LGFXBuffer *)resourceStates[i].variableSlots.ptr[currentGroup].currentValues)[0], (u8*)ptr, size);
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

    #ifdef ASTRALCANVAS_JSON_SHADER
    /*u32 ParseShaderVariables(Json::JsonElement *json, ShaderVariables *results, LGFXShaderInputAccessFlags accessedByShaderOfType)
    {
        i32 length = -1;
        Json::JsonElement *uniforms = json->GetProperty("uniforms");
        if (uniforms != NULL)
        {
            for (usize i = 0; i < uniforms->arrayElements.length; i++)
            {
                string name = uniforms->arrayElements.data[i].value.GetProperty("name")->GetString(results->allocator);
                u32 stride = uniforms->arrayElements.data[i].value.GetProperty("stride")->GetUint32();
                u32 set = uniforms->arrayElements.data[i].value.GetProperty("set")->GetUint32();
                u32 binding = uniforms->arrayElements.data[i].value.GetProperty("binding")->GetUint32();

                if ((i32)binding > length)
                {
                    length = binding;
                }

                AstralCanvas::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && binding < results->count && resource->resource.variableName != NULL && resource->name == name)
                {
                    //resource->resource.accessedBy = (LGFXShaderInputAccessFlags)((u32)resource->resource.accessedBy | accessedByShaderOfType);
                    name.deinit();
                }
                else
                {
                    ShaderResource newResource;
                    newResource.resource.binding = binding;
                    newResource.resource.set = set;
                    newResource.resource.variableName = name.buffer;
                    newResource.resource.size = stride;
                    //newResource.resource.accessedBy = accessedByShaderOfType;
                    newResource.resource.arrayLength = 0;
                    newResource.resource.type = LGFXShaderResourceType_Uniform;

                    newResource.name = name;
                    newResource.states = collections::List<LGFXFunctionVariable>(results->allocator);

                    results->Add(newResource); //->Insert((usize)binding, newResource);
                }
                //results->uniforms.Add(binding, {name, set, binding, stride});
            }
        }
        Json::JsonElement *textures = json->GetProperty("images");
        if (textures != NULL)
        {
            for (usize i = 0; i < textures->arrayElements.length; i++)
            {
                string name = textures->arrayElements.data[i].value.GetProperty("name")->GetString(results->allocator);
                u32 arrayLength = textures->arrayElements.data[i].value.GetProperty("arrayLength")->GetUint32();
                u32 set = textures->arrayElements.data[i].value.GetProperty("set")->GetUint32();
                u32 binding = textures->arrayElements.data[i].value.GetProperty("binding")->GetUint32();
                
                if ((i32)binding > length)
                {
                    length = binding;
                }

                AstralCanvas::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && binding < results->count && resource->resource.variableName != NULL && resource->name == name)
                {
                    //resource->resource.accessedBy = (LGFXShaderInputAccessFlags)((u32)resource->resource.accessedBy | accessedByShaderOfType);
                    name.deinit();
                }
                else
                {
                    ShaderResource newResource;
                    newResource.resource.binding = binding;
                    newResource.resource.set = set;
                    newResource.resource.variableName = name.buffer;
                    newResource.resource.arrayLength = arrayLength;
                    //newResource.resource.accessedBy = accessedByShaderOfType;
                    newResource.resource.type = LGFXShaderResourceType_Texture;
                    newResource.resource.size = 0;

                    newResource.name = name;
                    newResource.states = collections::List<LGFXFunctionVariable>(results->allocator);
                    
                    results->Add(newResource);
                }
            }
        }
        Json::JsonElement *samplers = json->GetProperty("samplers");
        if (samplers != NULL)
        {
            for (usize i = 0; i < samplers->arrayElements.length; i++)
            {
                string name = samplers->arrayElements.data[i].value.GetProperty("name")->GetString(results->allocator);
                u32 arrayLength = samplers->arrayElements.data[i].value.GetProperty("arrayLength")->GetUint32();
                u32 set = samplers->arrayElements.data[i].value.GetProperty("set")->GetUint32();
                u32 binding = samplers->arrayElements.data[i].value.GetProperty("binding")->GetUint32();

                if ((i32)binding > length)
                {
                    length = binding;
                }

                AstralCanvas::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && binding < results->count && resource->resource.variableName != NULL && resource->name == name)
                {
                    //resource->resource.accessedBy = (LGFXShaderInputAccessFlags)((u32)resource->resource.accessedBy | accessedByShaderOfType);
                    name.deinit();
                }
                else
                {
                    ShaderResource newResource;
                    newResource.resource.binding = binding;
                    newResource.resource.set = set;
                    newResource.resource.variableName = name.buffer;
                    newResource.resource.arrayLength = arrayLength;
                    //newResource.resource.accessedBy = accessedByShaderOfType;
                    newResource.resource.type = LGFXShaderResourceType_Sampler;
                    newResource.resource.size = 0;

                    newResource.name = name;
                    newResource.states = collections::List<LGFXFunctionVariable>(results->allocator);
                    results->Add(newResource);
                }
            }
        }
        Json::JsonElement *inputAttachments = json->GetProperty("inputAttachments");
        if (inputAttachments != NULL)
        {
            for (usize i = 0; i < inputAttachments->arrayElements.length; i++)
            {
                string name = inputAttachments->arrayElements.data[i].value.GetProperty("name")->GetString(results->allocator);
                u32 index = inputAttachments->arrayElements.data[i].value.GetProperty("index")->GetUint32();
                u32 set = inputAttachments->arrayElements.data[i].value.GetProperty("set")->GetUint32();
                u32 binding = inputAttachments->arrayElements.data[i].value.GetProperty("binding")->GetUint32();

                if ((i32)binding > length)
                {
                    length = binding;
                }

                AstralCanvas::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && binding < results->count && resource->resource.variableName != NULL && resource->name == name)
                {
                    //resource->resource.accessedBy = (LGFXShaderInputAccessFlags)((u32)resource->resource.accessedBy | accessedByShaderOfType);
                    name.deinit();
                }
                else
                {
                    ShaderResource newResource;
                    newResource.resource.binding = binding;
                    newResource.resource.set = set;
                    newResource.resource.variableName = name.buffer;
                    newResource.resource.arrayLength = 0;
                    newResource.resource.inputAttachmentIndex = index;
                    //newResource.resource.accessedBy = accessedByShaderOfType;
                    newResource.resource.type = LGFXShaderResourceType_InputAttachment;
                    newResource.resource.size = 0;

                    newResource.name = name;
                    newResource.states = collections::List<LGFXFunctionVariable>(results->allocator);
                    results->Add(newResource);
                }
            }
        }
        Json::JsonElement *storageBuffers = json->GetProperty("storageBuffers");
        if (storageBuffers != NULL)
        {
            for (usize i = 0; i < storageBuffers->arrayElements.length; i++)
            {
                string name = storageBuffers->arrayElements.data[i].value.GetProperty("name")->GetString(results->allocator);
                u32 set = storageBuffers->arrayElements.data[i].value.GetProperty("set")->GetUint32();
                u32 binding = storageBuffers->arrayElements.data[i].value.GetProperty("binding")->GetUint32();

                if ((i32)binding > length)
                {
                    length = binding;
                }

                AstralCanvas::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && binding < results->count && resource->resource.variableName != NULL && resource->name == name)
                {
                    //resource->resource.accessedBy = (LGFXShaderInputAccessFlags)((u32)resource->resource.accessedBy | accessedByShaderOfType);
                    name.deinit();
                }
                else
                {
                    ShaderResource newResource;
                    newResource.resource.binding = binding;
                    newResource.resource.set = set;
                    newResource.resource.variableName = name.buffer;
                    newResource.resource.arrayLength = 0;
                    //newResource.resource.accessedBy = accessedByShaderOfType;
                    newResource.resource.type = LGFXShaderResourceType_StructuredBuffer;
                    newResource.resource.size = 0;

                    newResource.name = name;
                    newResource.states = collections::List<LGFXFunctionVariable>(results->allocator);
                    results->Add(newResource);
                }
            }
        }
        Json::JsonElement *storageTextures = json->GetProperty("storageTextures");
        if (storageTextures != NULL)
        {
            for (usize i = 0; i < storageTextures->arrayElements.length; i++)
            {
                string name = storageTextures->arrayElements.data[i].value.GetProperty("name")->GetString(results->allocator);
                u32 set = storageTextures->arrayElements.data[i].value.GetProperty("set")->GetUint32();
                u32 binding = storageTextures->arrayElements.data[i].value.GetProperty("binding")->GetUint32();

                if ((i32)binding > length)
                {
                    length = binding;
                }

                AstralCanvas::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && binding < results->count && resource->resource.variableName != NULL && resource->name == name)
                {
                    //resource->resource.accessedBy = (LGFXShaderInputAccessFlags)((u32)resource->resource.accessedBy | accessedByShaderOfType);
                    name.deinit();
                }
                else
                {
                    ShaderResource newResource;
                    newResource.resource.binding = binding;
                    newResource.resource.set = set;
                    newResource.resource.variableName = name.buffer;
                    newResource.resource.arrayLength = 0;
                    //newResource.resource.accessedBy = accessedByShaderOfType;
                    newResource.resource.type = LGFXShaderResourceType_StorageTexture;
                    newResource.resource.size = 0;

                    newResource.name = name;
                    newResource.states = collections::List<LGFXFunctionVariable>(results->allocator);
                    results->Add(newResource);
                }
            }
        }

        return (u32)(length + 1);
    }

    usize CreateShaderFromString(LGFXDevice device, IAllocator allocator, string jsonString, ShaderFunction* result)
    {
        *result = ShaderFunction(allocator, 0);
        result->device = device;

        ArenaAllocator localArena = ArenaAllocator(GetCAllocator());
        
        Json::JsonElement root;
        usize parseJsonResult = ParseJsonDocument(localArena.AsAllocator(), jsonString, &root);
        if (parseJsonResult != 0)
        {
            localArena.deinit();
            return parseJsonResult;
        }

        Json::JsonElement *computeElement = root.GetProperty("compute");

        if (computeElement != NULL)
        {
            u32 maxBinding = ParseShaderVariables(computeElement, &result->uniforms, LGFXShaderInputAccess_Compute);
            u32 uniformsCount = result->uniforms.length;
            Json::JsonElement *computeSpirv = computeElement->GetProperty("spirv");
            collections::Array<u32> computeSpirvData = collections::Array<u32>(localArena.AsAllocator(), computeSpirv->arrayElements.length);
            for (usize i = 0; i < computeSpirv->arrayElements.length; i++)
            {
                computeSpirvData.data[i] = computeSpirv->arrayElements.data[i].value.GetUint32();
            }

            LGFXShaderResource *inputResources = (LGFXShaderResource *)malloc(sizeof(LGFXShaderResource) * uniformsCount);
            for (usize i = 0; i < uniformsCount; i++)
            {
                inputResources[result->uniforms.data[i].resource.binding] = result->uniforms.data[i].resource;
            }

            LGFXFunctionCreateInfo info = {};
            info.module1Data = computeSpirvData.data;
            info.module1DataLength = computeSpirvData.length;
            info.module2Data = NULL;
            info.module2DataLength = 0;
            info.uniformsCount = uniformsCount;
            info.uniforms = inputResources;
            info.type = LGFXFunctionType_Compute;
            result->gpuFunction = LGFXCreateFunction(device, &info);
            result->functionType = LGFXFunctionType_Compute;

            free(inputResources);
        }
        else
        {
            Json::JsonElement *vertexElement = root.GetProperty("vertex");
            Json::JsonElement *fragmentElement = root.GetProperty("fragment");

            if (vertexElement != NULL && fragmentElement != NULL)
            {
                u32 max1 = ParseShaderVariables(vertexElement, &result->uniforms, LGFXShaderInputAccess_Vertex);
                u32 max2 = ParseShaderVariables(fragmentElement, &result->uniforms, LGFXShaderInputAccess_Fragment);
                u32 uniformsCount = max1 > max2 ? max1 : max2;

                Json::JsonElement *vertexSpirv = vertexElement->GetProperty("spirv");
                Json::JsonElement *fragmentSpirv = fragmentElement->GetProperty("spirv");

                collections::Array<u32> vertexSpirvData = collections::Array<u32>(localArena.AsAllocator(), vertexSpirv->arrayElements.length);
                collections::Array<u32> fragmentSpirvData = collections::Array<u32>(localArena.AsAllocator(), fragmentSpirv->arrayElements.length);

                for (usize i = 0; i < vertexSpirv->arrayElements.length; i++)
                {
                    vertexSpirvData.data[i] = vertexSpirv->arrayElements.data[i].value.GetUint32();
                }
                for (usize i = 0; i < fragmentSpirv->arrayElements.length; i++)
                {
                    fragmentSpirvData.data[i] = fragmentSpirv->arrayElements.data[i].value.GetUint32();
                }

                LGFXShaderResource *inputResources = (LGFXShaderResource *)malloc(sizeof(LGFXShaderResource) * uniformsCount);
                for (usize i = 0; i < uniformsCount; i++)
                {
                    inputResources[result->uniforms.data[i].resource.binding] = result->uniforms.data[i].resource;
                }

                LGFXFunctionCreateInfo info = {};
                info.module1Data = vertexSpirvData.data;
                info.module1DataLength = vertexSpirvData.length;
                info.module2Data = fragmentSpirvData.data;
                info.module2DataLength = fragmentSpirvData.length;
                info.uniformsCount = uniformsCount;
                info.uniforms = inputResources;
                info.type = (LGFXFunctionType)(LGFXFunctionType_Fragment | LGFXFunctionType_Vertex);
                result->gpuFunction = LGFXCreateFunction(device, &info);
                result->functionType = (LGFXFunctionType)(LGFXFunctionType_Vertex | LGFXFunctionType_Fragment);

                free(inputResources);
            }
            else
            {
                fprintf(stderr, "Failed to detect shader type\n");
                localArena.deinit();
                return 1;
            }
        }

        localArena.deinit();
        return 0;
    }*/
    usize CreateShaderFromString2(LGFXDevice device, IAllocator allocator, string jsonString, ShaderFunction *result)
    {
        ArenaAllocator localArena = ArenaAllocator(GetCAllocator());
        Scope(ArenaAllocator, localArena);

        Json::JsonElement root;
        usize parseJsonResult = ParseJsonDocument(localArena.AsAllocator(), jsonString, &root);
        if (parseJsonResult != 0)
        {
            return parseJsonResult;
        }

        LGFXFunctionType type = LGFXFunctionType_Vertex;
        Json::JsonElement *typeElem = root.GetProperty("type");
        if (typeElem != NULL)
        {
            if (typeElem->AsString() == "Vertex-Fragment")
            {
                type = (LGFXFunctionType)(LGFXFunctionType_Vertex | LGFXFunctionType_Fragment);
            }
            else if (typeElem->AsString() == "Compute")
            {
                type = LGFXFunctionType_Compute;
            }
            else
            {
                return 1;
            }
        }

        LGFXShaderResource *inputResources = NULL;
        LGFXFunctionCreateInfo info = {};
        info.type = type;

        Json::JsonElement *uniforms = root.GetProperty("uniforms");
        usize len = 0;
        if (uniforms != NULL)
        {
            len = uniforms->arrayElements.length;
        }
        *result = ShaderFunction(allocator, len);
        result->device = device;
        if (uniforms != NULL)
        {
            u32 uniformsCount = 0;
            for (usize i = 0; i < uniforms->arrayElements.length; i++)
            {
                Json::JsonElement *elem = &uniforms->arrayElements.data[i].value;
                u32 set = elem->GetProperty("set")->GetUint32();

                u32 binding = elem->GetProperty("binding")->GetUint32();
                if (binding + 1 > uniformsCount)
                {
                    uniformsCount = binding + 1;
                }

                AstralCanvas::ShaderResource newResource = {};
                newResource.name = elem->GetProperty("name")->GetStringRaw(allocator);
                newResource.resource.variableName = newResource.name.buffer;
                newResource.resource.binding = binding;
                newResource.resource.set = set;
                newResource.states = collections::List<LGFXFunctionVariable>(allocator);

                Json::JsonElement *optElem = elem->GetProperty("arrayLength");
                if (optElem != NULL)
                {
                    newResource.resource.arrayLength = optElem->GetUint32();
                }
                optElem = elem->GetProperty("size");
                if (optElem != NULL)
                {
                    newResource.resource.size = optElem->GetUint32();
                }

                string typeName = elem->GetProperty("type")->AsString();
                if (typeName == "UniformBuffer")
                {
                    newResource.resource.type = LGFXShaderResourceType_Uniform;
                }
                else if (typeName == "Image")
                {
                    newResource.resource.type = LGFXShaderResourceType_Texture;
                }
                else if (typeName == "Sampler")
                {
                    newResource.resource.type = LGFXShaderResourceType_Sampler;
                }
                else if (typeName == "StorageBuffer")
                {
                    newResource.resource.type = LGFXShaderResourceType_StructuredBuffer;
                }
                else if (typeName == "StorageImage")
                {
                    newResource.resource.type = LGFXShaderResourceType_StorageTexture;
                }
                else if (typeName == "InputAttachment")
                {
                    newResource.resource.type = LGFXShaderResourceType_InputAttachment;
                }

                result->uniforms.Add(newResource);
            }

            inputResources = (LGFXShaderResource *)malloc(sizeof(LGFXShaderResource) * uniformsCount);
            for (usize i = 0; i < uniformsCount; i++)
            {
                inputResources[result->uniforms.data[i].resource.binding] = result->uniforms.data[i].resource;
            }

            info.uniformsCount = uniformsCount;
            info.uniforms = inputResources;
        }

        Json::JsonElement *spvElem = root.GetProperty("spv");
        if (spvElem != NULL)
        {
            collections::Array<u32> spirvData = collections::Array<u32>(localArena.AsAllocator(), spvElem->arrayElements.length);
            for (usize i = 0; i < spvElem->arrayElements.length; i++)
            {
                spirvData.data[i] = spvElem->arrayElements.data[i].value.GetUint32();
            }
            info.module1Data = spirvData.data;
            info.module1DataLength = spirvData.length;

            if (type == LGFXFunctionType_Vertex)
            {
                info.module2Data = spirvData.data;
                info.module2DataLength = spirvData.length;
            }
        }
        else
        {
            Json::JsonElement *vertElem = root.GetProperty("vertex");
            Json::JsonElement *fragElem = root.GetProperty("fragment");

            if (vertElem != NULL && fragElem != NULL)
            {
                collections::Array<u32> vertData = collections::Array<u32>(localArena.AsAllocator(), vertElem->arrayElements.length);
                for (usize i = 0; i < vertElem->arrayElements.length; i++)
                {
                    vertData.data[i] = vertElem->arrayElements.data[i].value.GetUint32();
                }
                collections::Array<u32> fragData = collections::Array<u32>(localArena.AsAllocator(), fragElem->arrayElements.length);
                for (usize i = 0; i < fragElem->arrayElements.length; i++)
                {
                    fragData.data[i] = fragElem->arrayElements.data[i].value.GetUint32();
                }

                info.module1Data = vertData.data;
                info.module1DataLength = vertData.length;

                info.module2Data = fragData.data;
                info.module2DataLength = fragData.length;
            }
            else
            {
                return 1;
            }
        }

        result->gpuFunction = LGFXCreateFunction(device, &info);
        result->functionType = type;

        if (inputResources != NULL)
        {
            free(inputResources);
        }

        return 0;
    }
    #endif

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
        if (fileVersion == 1)
        {
            ArenaAllocator arena = ArenaAllocator(GetCAllocator());
            Scope(ArenaAllocator, arena);

            const u32 shaderType = input.Read<u32>();
            LGFXFunctionCreateInfo info = {};

            *result = ShaderFunction(allocator, device);
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