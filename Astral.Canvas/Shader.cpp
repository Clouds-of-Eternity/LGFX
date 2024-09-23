#include "lgfx-astral/Shader.hpp"

using namespace Json;

namespace LGFX
{
    Shader::Shader()
    {
        this->allocator = IAllocator{};
        gpuFunction = NULL;
        shaderState = NULL;
        uniforms = ShaderVariables();

        this->descriptorForThisDrawCall = 0;
        this->descriptorSets = collections::vector<void *>();
    }
    Shader::Shader(IAllocator allocator)
    {
        this->allocator = allocator;
        gpuFunction = NULL;
        shaderState = NULL;
        uniforms = ShaderVariables(allocator, ShaderResource{});

        this->descriptorForThisDrawCall = 0;
        this->descriptorSets = collections::vector<void *>(allocator);
    }

    void ParseShaderVariables(Json::JsonElement *json, ShaderVariables *results, LGFXShaderInputAccessFlags accessedByShaderOfType)
    {
        JsonElement *uniforms = json->GetProperty("uniforms");
        if (uniforms != NULL)
        {
            for (usize i = 0; i < uniforms->arrayElements.length; i++)
            {
                string name = uniforms->arrayElements.data[i].GetProperty("name")->GetString(results->allocator);
                u32 stride = uniforms->arrayElements.data[i].GetProperty("stride")->GetUint32();
                u32 set = uniforms->arrayElements.data[i].GetProperty("set")->GetUint32();
                u32 binding = uniforms->arrayElements.data[i].GetProperty("binding")->GetUint32();

                LGFX::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && resource->resource.variableName != NULL)
                {
                    resource->resource.accessedBy = (LGFXShaderInputAccessFlags)((u32)resource->resource.accessedBy | accessedByShaderOfType);
                    name.deinit();
                }
                else
                {
                    ShaderResource newResource;
                    newResource.resource.binding = binding;
                    newResource.resource.set = set;
                    newResource.resource.variableName = name.buffer;
                    newResource.resource.size = stride;
                    newResource.resource.accessedBy = accessedByShaderOfType;
                    newResource.resource.arrayLength = 0;
                    newResource.resource.type = LGFXShaderResourceType_Uniform;

                    newResource.nameStr = name;
                    newResource.states = collections::vector<ShaderStagingMutableState>(results->allocator);
                    
                    results->Insert((usize)binding, newResource);
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
                
                LGFX::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && resource->resource.variableName != NULL)
                {
                    resource->resource.accessedBy = (LGFXShaderInputAccessFlags)((u32)resource->resource.accessedBy | accessedByShaderOfType);
                    name.deinit();
                }
                else
                {
                    ShaderResource newResource;
                    newResource.resource.binding = binding;
                    newResource.resource.set = set;
                    newResource.resource.variableName = name.buffer;
                    newResource.resource.arrayLength = arrayLength;
                    newResource.resource.accessedBy = accessedByShaderOfType;
                    newResource.resource.type = LGFXShaderResourceType_Texture;
                    newResource.resource.size = 0;

                    newResource.nameStr = name;
                    newResource.states = collections::vector<ShaderStagingMutableState>(results->allocator);
                    
                    results->Insert((usize)binding, newResource);
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

                LGFX::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && resource->resource.variableName != NULL)
                {
                    resource->resource.accessedBy = (LGFXShaderInputAccessFlags)((u32)resource->resource.accessedBy | accessedByShaderOfType);
                    name.deinit();
                }
                else
                {
                    ShaderResource newResource;
                    newResource.resource.binding = binding;
                    newResource.resource.set = set;
                    newResource.resource.variableName = name.buffer;
                    newResource.resource.arrayLength = arrayLength;
                    newResource.resource.accessedBy = accessedByShaderOfType;
                    newResource.resource.type = LGFXShaderResourceType_Sampler;
                    newResource.resource.size = 0;

                    newResource.nameStr = name;
                    newResource.states = collections::vector<ShaderStagingMutableState>(results->allocator);
                    results->Insert((usize)binding, newResource);
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

                LGFX::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && resource->resource.variableName != NULL)
                {
                    resource->resource.accessedBy = (LGFXShaderInputAccessFlags)((u32)resource->resource.accessedBy | accessedByShaderOfType);
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
                    newResource.resource.accessedBy = accessedByShaderOfType;
                    newResource.resource.type = LGFXShaderResourceType_InputAttachment;
                    newResource.resource.size = 0;

                    newResource.nameStr = name;
                    newResource.states = collections::vector<ShaderStagingMutableState>(results->allocator);
                    results->Insert((usize)binding, newResource);
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
                u32 mslBinding = storageBuffers->arrayElements.data[i].GetProperty("mslBinding")->GetUint32();
            
                LGFX::ShaderResource *resource = results->Get(binding);
                if (resource != NULL && resource->resource.variableName != NULL)
                {
                    resource->resource.accessedBy = (LGFXShaderInputAccessFlags)((u32)resource->resource.accessedBy | accessedByShaderOfType);
                    name.deinit();
                }
                else
                {
                    ShaderResource newResource;
                    newResource.resource.binding = binding;
                    newResource.resource.set = set;
                    newResource.resource.variableName = name.buffer;
                    newResource.resource.arrayLength = 0;
                    newResource.resource.accessedBy = accessedByShaderOfType;
                    newResource.resource.type = LGFXShaderResourceType_InputAttachment;
                    newResource.resource.size = 0;

                    newResource.nameStr = name;
                    newResource.states = collections::vector<ShaderStagingMutableState>(results->allocator);
                    results->Insert((usize)binding, newResource);
                }
            }
        }
    }
}