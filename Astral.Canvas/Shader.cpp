#include "lgfx-astral/Shader.hpp"
#include <math.h>
#include <stdio.h>
#include "scope.hpp"

using namespace Json;

namespace AstralCanvas
{
    collections::vector<Shader *> allUsedShaders = collections::vector<Shader *>(GetCAllocator());

    Shader::Shader()
    {
        this->allocator = IAllocator{};
        gpuFunction = NULL;
        uniforms = ShaderVariables();

        this->descriptorForThisDrawCall = 0;
        this->variableBatches = collections::vector<LGFXFunctionVariableBatch>();
    }
    Shader::Shader(IAllocator allocator)
    {
        this->allocator = allocator;
        gpuFunction = NULL;
        uniforms = ShaderVariables(allocator);

        this->descriptorForThisDrawCall = 0;
        this->variableBatches = collections::vector<LGFXFunctionVariableBatch>(allocator);
    }
    void ShaderResource::deinit()
    {
        nameStr.deinit();
        LGFXShaderResourceType resourceType = resource.type;
        for (u32 j = 0; j < states.count; j++)
        {
            LGFXDestroyFunctionVariable(states.ptr[j]);
        }
        states.deinit();
    }
    void Shader::deinit()
    {
        LGFXDestroyFunction(gpuFunction);
        if (uniforms.ptr != NULL)
        {
            for (u32 i = 0; i < uniforms.count; i++)
            {
                if (uniforms.ptr[i].nameStr.buffer == NULL)
                {
                    continue;
                }
                uniforms.ptr[i].deinit();
            }
            uniforms.deinit();
        }
        if (usedMaterials.data != NULL)
        {
            for (usize i = 0; i < usedMaterials.length; i++)
            {
                usedMaterials.data[i].deinit();
            }
            usedMaterials.deinit();
        }
    }

    void Shader::SetShaderVariableComputeBuffer(const char* variableName, LGFXBuffer buffer)
    {
        CheckDescriptorSetAvailability();
        for (usize i = 0; i < uniforms.count; i++)
        {
            if (uniforms.ptr[i].nameStr.buffer == NULL)
            {
                break;
            }
            
            if (uniforms.ptr[i].nameStr == variableName)
            {
                //uniformsHasBeenSet = true;
                uniforms.ptr[i].states.ptr[descriptorForThisDrawCall].mutated = true;
                ((LGFXBuffer *)uniforms.ptr[i].states.ptr[descriptorForThisDrawCall].currentValues)[0] = buffer;
                break;
            }
        }
    }
    void Shader::SetShaderVariable(const char* variableName, void* ptr, usize size)
    {
        CheckDescriptorSetAvailability();
        for (usize i = 0; i < uniforms.count; i++)
        {
            if (uniforms.ptr[i].nameStr.buffer == NULL)
            {
                break;
            }
            
            if (uniforms.ptr[i].nameStr == variableName)
            {
                uniforms.ptr[i].states.ptr[descriptorForThisDrawCall].mutated = true;
                LGFXSetBufferDataFast(((LGFXBuffer *)uniforms.ptr[i].states.ptr[descriptorForThisDrawCall].currentValues)[0], (u8*)ptr, size);
                return;
                //uniforms.ptr[i].states.ptr[descriptorForThisDrawCall].ub.SetData(ptr, size);
            }
        }
        fprintf(stderr, "Shader does not possess a variable of name %s\n", variableName);
    }
    void Shader::SetShaderVariableTextures(const char* variableName, LGFXTexture*textures, usize count)
    {
        CheckDescriptorSetAvailability();
        for (usize i = 0; i < uniforms.count; i++)
        {
            if (uniforms.ptr[i].nameStr.buffer == NULL)
            {
                break;
            }
            if (uniforms.ptr[i].nameStr == variableName)
            {
                LGFXFunctionVariable *mutableState = &uniforms.ptr[i].states.ptr[descriptorForThisDrawCall];
                mutableState->mutated = true;
                for (usize j = 0; j < count; j++)
                {
                   ((LGFXTexture *)mutableState->currentValues)[j] = textures[j];
                }
                return;
            }
        }
        fprintf(stderr, "Shader does not possess a variable of name %s\n", variableName);
    }
    void Shader::SetShaderVariableTexture(const char* variableName, LGFXTexture texture)
    {
        this->SetShaderVariableTextures(variableName, &texture, 1);
    }
    void Shader::SetShaderVariableSamplers(const char* variableName, LGFXSamplerState *samplers, usize count)
    {
        CheckDescriptorSetAvailability();
        for (usize i = 0; i < uniforms.count; i++)
        {
            if (uniforms.ptr[i].nameStr.buffer == NULL)
            {
                break;
            }
            if (uniforms.ptr[i].nameStr == variableName)
            {
                LGFXFunctionVariable *mutableState = &uniforms.ptr[i].states.ptr[descriptorForThisDrawCall];
                mutableState->mutated = true;
                for (usize j = 0; j < count; j++)
                {
                   ((LGFXSamplerState *)mutableState->currentValues)[j] = samplers[j];
                }
                return;
            }
        }
        fprintf(stderr, "Shader does not possess a variable of name %s\n", variableName);
    }
    void Shader::SetShaderVariableSampler(const char* variableName, LGFXSamplerState sampler)
    {
        this->SetShaderVariableSamplers(variableName, &sampler, 1);
    }

    i32 Shader::GetVariableBinding(text variableName)
    {
        for (i32 i = 0; i < this->uniforms.count; i++)
        {
            if (uniforms.ptr[i].nameStr == variableName)
            {
                return i;
            }
        }
        return -1;
    }
    void Shader::CheckDescriptorSetAvailability(bool forceAddNewDescriptor)
    {
        bool added = false;
        for (u32 i = 0; i < this->uniforms.count; i++)
        {
            if (uniforms.ptr[i].nameStr.buffer == NULL)
            {
                break;
            }

            if (forceAddNewDescriptor || this->descriptorForThisDrawCall >= this->variableBatches.count)
            {
                LGFXFunctionVariable newVarSlot = LGFXCreateFunctionVariableSlot(this->gpuFunction, this->uniforms[i].resource.binding);
                this->uniforms.ptr[i].states.Add(newVarSlot);
                added = true;
            }
        }
        if (added)
        {
            LGFXFunctionVariableBatch batch = LGFXCreateFunctionVariableBatch(this->gpuFunction);
            this->variableBatches.Add(batch);
        }
    }
    void Shader::SyncUniformsWithGPU(LGFXCommandBuffer commandBuffer, bool pushToUsedShaderStack)
    {
        LGFXFunctionVariable variables[32];
        u32 variablesCount = 0;
        for (usize i = 0; i < this->uniforms.count; i++)
        {
            if (this->uniforms.ptr[i].nameStr.buffer == NULL)
            {
                break;
            }
            variables[i] = this->uniforms.ptr[i].states.ptr[this->descriptorForThisDrawCall];
            variablesCount++;
        }
        LGFXFunctionVariableBatch batch = this->variableBatches.ptr[this->descriptorForThisDrawCall];
        LGFXFunctionSendVariablesToGPU(this->device, batch, variables, variablesCount);
        LGFXUseFunctionVariables(commandBuffer, batch, gpuFunction, 0);

        descriptorForThisDrawCall += 1;

        allUsedShaders.Add(this);
    }

    u32 ParseShaderVariables2(IAllocator allocator, Json::JsonElement *variables, collections::vector<AstralCanvas::ShaderResource> &result)//, LGFXShaderInputAccessFlags accessedBy)
    {
        u32 maxBinding = 0;
        for (usize i = 0; i < variables->arrayElements.length; i++)
        {
            Json::JsonElement *elem = &variables->arrayElements.data[i];
            u32 set = elem->GetProperty("set")->GetUint32();

            u32 binding = elem->GetProperty("binding")->GetUint32();
            if (binding + 1 > maxBinding)
            {
                maxBinding = binding + 1;
            }

            /*AstralCanvas::ShaderResource *existingResource = result.Get(binding);
            if (existingResource != NULL && existingResource->nameStr.buffer != NULL)
            {
                existingResource->resource.accessedBy = (LGFXShaderInputAccessFlags)(existingResource->resource.accessedBy | accessedBy);
            }
            else
            {*/
            AstralCanvas::ShaderResource newResource = {};
            newResource.resource.set = set;
            newResource.nameStr = elem->GetProperty("name")->GetStringRaw(allocator);
            newResource.resource.variableName = newResource.nameStr.buffer;
            newResource.resource.binding = binding;
            newResource.resource.set = elem->GetProperty("set")->GetUint32();
            //newResource.resource.accessedBy = accessedBy;
            newResource.states = collections::vector<LGFXFunctionVariable>(allocator);

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

            result.Add(newResource);
            //result.Insert(newResource.resource.binding, newResource);
            //}
        }
        return maxBinding;
    }

    u32 ParseShaderVariables(Json::JsonElement *json, ShaderVariables *results, LGFXShaderInputAccessFlags accessedByShaderOfType)
    {
        i32 length = -1;
        JsonElement *uniforms = json->GetProperty("uniforms");
        if (uniforms != NULL)
        {
            for (usize i = 0; i < uniforms->arrayElements.length; i++)
            {
                string name = uniforms->arrayElements.data[i].GetProperty("name")->GetString(results->allocator);
                u32 stride = uniforms->arrayElements.data[i].GetProperty("stride")->GetUint32();
                u32 set = uniforms->arrayElements.data[i].GetProperty("set")->GetUint32();
                u32 binding = uniforms->arrayElements.data[i].GetProperty("binding")->GetUint32();

                if ((i32)binding > length)
                {
                    length = binding;
                }

                AstralCanvas::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && binding < results->count && resource->resource.variableName != NULL && resource->nameStr == name)
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

                    newResource.nameStr = name;
                    newResource.states = collections::vector<LGFXFunctionVariable>(results->allocator);

                    results->Add(newResource); //->Insert((usize)binding, newResource);
                }
                //results->uniforms.Add(binding, {name, set, binding, stride});
            }
        }
        JsonElement *textures = json->GetProperty("images");
        if (textures != NULL)
        {
            for (usize i = 0; i < textures->arrayElements.length; i++)
            {
                string name = textures->arrayElements.data[i].GetProperty("name")->GetString(results->allocator);
                u32 arrayLength = textures->arrayElements.data[i].GetProperty("arrayLength")->GetUint32();
                u32 set = textures->arrayElements.data[i].GetProperty("set")->GetUint32();
                u32 binding = textures->arrayElements.data[i].GetProperty("binding")->GetUint32();
                
                if ((i32)binding > length)
                {
                    length = binding;
                }

                AstralCanvas::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && binding < results->count && resource->resource.variableName != NULL && resource->nameStr == name)
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

                    newResource.nameStr = name;
                    newResource.states = collections::vector<LGFXFunctionVariable>(results->allocator);
                    
                    results->Add(newResource);
                }
            }
        }
        JsonElement *samplers = json->GetProperty("samplers");
        if (samplers != NULL)
        {
            for (usize i = 0; i < samplers->arrayElements.length; i++)
            {
                string name = samplers->arrayElements.data[i].GetProperty("name")->GetString(results->allocator);
                u32 arrayLength = samplers->arrayElements.data[i].GetProperty("arrayLength")->GetUint32();
                u32 set = samplers->arrayElements.data[i].GetProperty("set")->GetUint32();
                u32 binding = samplers->arrayElements.data[i].GetProperty("binding")->GetUint32();

                if ((i32)binding > length)
                {
                    length = binding;
                }

                AstralCanvas::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && binding < results->count && resource->resource.variableName != NULL && resource->nameStr == name)
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

                    newResource.nameStr = name;
                    newResource.states = collections::vector<LGFXFunctionVariable>(results->allocator);
                    results->Add(newResource);
                }
            }
        }
        JsonElement *inputAttachments = json->GetProperty("inputAttachments");
        if (inputAttachments != NULL)
        {
            for (usize i = 0; i < inputAttachments->arrayElements.length; i++)
            {
                string name = inputAttachments->arrayElements.data[i].GetProperty("name")->GetString(results->allocator);
                u32 index = inputAttachments->arrayElements.data[i].GetProperty("index")->GetUint32();
                u32 set = inputAttachments->arrayElements.data[i].GetProperty("set")->GetUint32();
                u32 binding = inputAttachments->arrayElements.data[i].GetProperty("binding")->GetUint32();

                if ((i32)binding > length)
                {
                    length = binding;
                }

                AstralCanvas::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && binding < results->count && resource->resource.variableName != NULL && resource->nameStr == name)
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

                    newResource.nameStr = name;
                    newResource.states = collections::vector<LGFXFunctionVariable>(results->allocator);
                    results->Add(newResource);
                }
            }
        }
        JsonElement *storageBuffers = json->GetProperty("storageBuffers");
        if (storageBuffers != NULL)
        {
            for (usize i = 0; i < storageBuffers->arrayElements.length; i++)
            {
                string name = storageBuffers->arrayElements.data[i].GetProperty("name")->GetString(results->allocator);
                u32 set = storageBuffers->arrayElements.data[i].GetProperty("set")->GetUint32();
                u32 binding = storageBuffers->arrayElements.data[i].GetProperty("binding")->GetUint32();

                if ((i32)binding > length)
                {
                    length = binding;
                }

                AstralCanvas::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && binding < results->count && resource->resource.variableName != NULL && resource->nameStr == name)
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

                    newResource.nameStr = name;
                    newResource.states = collections::vector<LGFXFunctionVariable>(results->allocator);
                    results->Add(newResource);
                }
            }
        }
        JsonElement *storageTextures = json->GetProperty("storageTextures");
        if (storageTextures != NULL)
        {
            for (usize i = 0; i < storageTextures->arrayElements.length; i++)
            {
                string name = storageTextures->arrayElements.data[i].GetProperty("name")->GetString(results->allocator);
                u32 set = storageTextures->arrayElements.data[i].GetProperty("set")->GetUint32();
                u32 binding = storageTextures->arrayElements.data[i].GetProperty("binding")->GetUint32();

                if ((i32)binding > length)
                {
                    length = binding;
                }

                AstralCanvas::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && binding < results->count && resource->resource.variableName != NULL && resource->nameStr == name)
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

                    newResource.nameStr = name;
                    newResource.states = collections::vector<LGFXFunctionVariable>(results->allocator);
                    results->Add(newResource);
                }
            }
        }

        return (u32)(length + 1);
    }

    usize CreateShaderFromString2(LGFXDevice device, IAllocator allocator, string jsonString, Shader *result)
    {
        *result = Shader(allocator);
        result->device = device;
        ArenaAllocator localArena = ArenaAllocator(GetCAllocator());
        Scope(ArenaAllocator, localArena);

        JsonElement root;
        usize parseJsonResult = ParseJsonDocument(localArena.AsAllocator(), jsonString, &root);
        if (parseJsonResult != 0)
        {
            return parseJsonResult;
        }

        LGFXFunctionType type = LGFXFunctionType_Vertex;
        JsonElement *typeElem = root.GetProperty("type");
        if (typeElem != NULL)
        {
            if (typeElem->AsString() == "Vertex-Fragment")
            {
                type = LGFXFunctionType_Vertex;
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

        JsonElement *uniforms = root.GetProperty("uniforms");
        if (uniforms != NULL)
        {
            u32 uniformsCount = ParseShaderVariables2(allocator, uniforms, result->uniforms);

            inputResources = (LGFXShaderResource *)malloc(sizeof(LGFXShaderResource) * uniformsCount);
            for (usize i = 0; i < uniformsCount; i++)
            {
                inputResources[result->uniforms.ptr[i].resource.binding] = result->uniforms.ptr[i].resource;
            }

            info.uniformsCount = uniformsCount;
            info.uniforms = inputResources;
        }

        JsonElement *spvElem = root.GetProperty("spv");
        if (spvElem != NULL)
        {
            collections::Array<u32> spirvData = collections::Array<u32>(localArena.AsAllocator(), spvElem->arrayElements.length);
            for (usize i = 0; i < spvElem->arrayElements.length; i++)
            {
                spirvData.data[i] = spvElem->arrayElements.data[i].GetUint32();
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
            JsonElement *vertElem = root.GetProperty("vertex");
            JsonElement *fragElem = root.GetProperty("fragment");

            if (vertElem != NULL && fragElem != NULL)
            {
                collections::Array<u32> vertData = collections::Array<u32>(localArena.AsAllocator(), vertElem->arrayElements.length);
                for (usize i = 0; i < vertElem->arrayElements.length; i++)
                {
                    vertData.data[i] = vertElem->arrayElements.data[i].GetUint32();
                }
                collections::Array<u32> fragData = collections::Array<u32>(localArena.AsAllocator(), fragElem->arrayElements.length);
                for (usize i = 0; i < fragElem->arrayElements.length; i++)
                {
                    fragData.data[i] = fragElem->arrayElements.data[i].GetUint32();
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
    usize CreateShaderFromString(LGFXDevice device, IAllocator allocator, string jsonString, Shader* result)
    {
        *result = Shader(allocator);
        result->device = device;
        ArenaAllocator localArena = ArenaAllocator(GetCAllocator());
        
        JsonElement root;
        usize parseJsonResult = ParseJsonDocument(localArena.AsAllocator(), jsonString, &root);
        if (parseJsonResult != 0)
        {
            localArena.deinit();
            return parseJsonResult;
        }

        JsonElement *computeElement = root.GetProperty("compute");

        Json::JsonElement *materialsElement = root.GetProperty("materials");
        if (materialsElement != NULL)
        {
            result->usedMaterials = collections::Array<ShaderMaterialExport>(result->allocator, materialsElement->childObjects.count);
            u32 materialIndex = 0;
            for (usize i = 0; i < materialsElement->childObjects.bucketsCount; i++)
            {
                if (materialsElement->childObjects.buckets[i].initialized)
                {
                    for (usize j = 0; j < materialsElement->childObjects.buckets[i].entries.count; j++)
                    {
                        Json::JsonElement *materialElement = &materialsElement->childObjects.buckets[i].entries.ptr[j].value;
                        result->usedMaterials.data[materialIndex].name = string(result->allocator, materialsElement->childObjects.buckets[i].entries.ptr[j].key.buffer);
                        result->usedMaterials.data[materialIndex].params = collections::Array<ShaderMaterialExportParam>(result->allocator, materialElement->childObjects.count);
                        u32 paramIndex = 0;
                        for (usize c = 0; c < materialElement->childObjects.bucketsCount; c++)
                        {
                            if (materialElement->childObjects.buckets[c].initialized)
                            {
                                for (usize d = 0; d < materialElement->childObjects.buckets[c].entries.count; d++)
                                {
                                    result->usedMaterials.data[materialIndex].params.data[paramIndex].name = string(result->allocator, materialElement->childObjects.buckets[c].entries.ptr[d].key.buffer);
                                    result->usedMaterials.data[materialIndex].params.data[paramIndex].size = materialElement->childObjects.buckets[c].entries.ptr[d].value.GetUint32();
                                    paramIndex++;
                                }
                            }
                        }
                        materialIndex++;
                    }
                }
            }
        }

        if (computeElement != NULL)
        {
            u32 maxBinding = ParseShaderVariables(computeElement, &result->uniforms, LGFXShaderInputAccess_Compute);
            u32 uniformsCount = result->uniforms.count;
            JsonElement *computeSpirv = computeElement->GetProperty("spirv");
            collections::Array<u32> computeSpirvData = collections::Array<u32>(localArena.AsAllocator(), computeSpirv->arrayElements.length);
            for (usize i = 0; i < computeSpirv->arrayElements.length; i++)
            {
                computeSpirvData.data[i] = computeSpirv->arrayElements.data[i].GetUint32();
            }

            LGFXShaderResource *inputResources = (LGFXShaderResource *)malloc(sizeof(LGFXShaderResource) * uniformsCount);
            for (usize i = 0; i < uniformsCount; i++)
            {
                inputResources[result->uniforms.ptr[i].resource.binding] = result->uniforms.ptr[i].resource;
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
            JsonElement *vertexElement = root.GetProperty("vertex");
            JsonElement *fragmentElement = root.GetProperty("fragment");

            if (vertexElement != NULL && fragmentElement != NULL)
            {
                u32 max1 = ParseShaderVariables(vertexElement, &result->uniforms, LGFXShaderInputAccess_Vertex);
                u32 max2 = ParseShaderVariables(fragmentElement, &result->uniforms, LGFXShaderInputAccess_Fragment);
                u32 uniformsCount = max1 > max2 ? max1 : max2;

                JsonElement *vertexSpirv = vertexElement->GetProperty("spirv");
                JsonElement *fragmentSpirv = fragmentElement->GetProperty("spirv");

                collections::Array<u32> vertexSpirvData = collections::Array<u32>(localArena.AsAllocator(), vertexSpirv->arrayElements.length);
                collections::Array<u32> fragmentSpirvData = collections::Array<u32>(localArena.AsAllocator(), fragmentSpirv->arrayElements.length);

                for (usize i = 0; i < vertexSpirv->arrayElements.length; i++)
                {
                    vertexSpirvData.data[i] = vertexSpirv->arrayElements.data[i].GetUint32();
                }
                for (usize i = 0; i < fragmentSpirv->arrayElements.length; i++)
                {
                    fragmentSpirvData.data[i] = fragmentSpirv->arrayElements.data[i].GetUint32();
                }

                LGFXShaderResource *inputResources = (LGFXShaderResource *)malloc(sizeof(LGFXShaderResource) * uniformsCount);
                for (usize i = 0; i < uniformsCount; i++)
                {
                    inputResources[result->uniforms.ptr[i].resource.binding] = result->uniforms.ptr[i].resource;
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
    }
}