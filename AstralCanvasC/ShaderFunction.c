#include "AstralCanvas/ShaderFunction.h"
#include "ByteStreamOps.h"
#include "Application_c.h"
#include "FileUtils.h"

typedef struct ShaderResource
{
    string nameStr;
    LGFXShaderResource resource;

} ShaderResource;
typedef struct ShaderFunctionImpl
{
    LGFXFunction gpuFunction;
    LGFXFunctionType functionType;

    ShaderResource *shaderResources;
    size_t numShaderResources;

} ShaderFunctionImpl;

typedef struct ShaderResourceState
{
    List variableSlots;
} ShaderResourceState;
typedef struct ShaderFunctionStateImpl
{
    ShaderFunction shader;
    uint32_t currentGroup;

    ShaderResourceState *shaderResourceStates;
    List variableSlotGroups;
} ShaderFunctionStateImpl;

ShaderFunctionState ShaderFunction_CreateUseState(ShaderFunction function)
{
    ShaderFunctionState state = (ShaderFunctionState)malloc(sizeof(ShaderFunctionStateImpl));
    state->shader = function;
    state->currentGroup = 0;
    state->shaderResourceStates = (ShaderResourceState *)malloc(sizeof(ShaderResourceState) * function->numShaderResources);
    for (uint32_t i = 0; i < function->numShaderResources; i++)
    {
        state->shaderResourceStates[i].variableSlots = List_Create(GetCAllocator(), sizeof(LGFXFunctionVariable));
    }
    state->variableSlotGroups = List_Create(GetCAllocator(), sizeof(LGFXFunctionVariableBatch));

    return state;
}
void ShaderFunctionState_Deinit(ShaderFunctionState self)
{
    for (uint32_t i = 0; i < self->shader->numShaderResources; i++)
    {
        ShaderResourceState resourceState = self->shaderResourceStates[i];

        for (uint32_t i = 0; i < resourceState.variableSlots.count; i++)
        {
            LGFXFunctionVariable variableSlot = LIST_GET(&resourceState.variableSlots, LGFXFunctionVariable, i);
            LGFXDestroyFunctionVariable(variableSlot);
        }
        List_Deinit(&resourceState.variableSlots);
    }
    List_Deinit(&self->variableSlotGroups);
    free(self);
}

void ShaderFunctionState_CheckVariableBatchAvailability(ShaderFunctionState self, bool forceCreateNewBatch)
{
    bool added = false;
    for (uint32_t i = 0; i < self->shader->numShaderResources; i++)
    {
        if (self->shader->shaderResources[i].nameStr.buffer == NULL)
        {
            break;
        }

        if (forceCreateNewBatch || self->currentGroup >= self->variableSlotGroups.count)
        {
            LGFXFunctionVariable newVarSlot = LGFXCreateFunctionVariableSlot(self->shader->gpuFunction, self->shader->shaderResources[i].resource.binding);
            List_Add(&self->shaderResourceStates[i].variableSlots, &newVarSlot);
            added = true;
        }
    }

    if (added)
    {
        LGFXFunctionVariableBatch batch = LGFXCreateFunctionVariableBatch(self->shader->gpuFunction);
        List_Add(&self->variableSlotGroups, &batch);
    }
}

void ShaderFunctionState_SetUniform(ShaderFunctionState self, const char* variableName, void* ptr, size_t size)
{
    ShaderFunctionState_CheckVariableBatchAvailability(self, false);
    for (size_t i = 0; i < self->shader->numShaderResources; i++)
    {
        ShaderResource *resource = &self->shader->shaderResources[i];

        if (resource->nameStr.buffer == NULL)
        {
            break;
        }
        
        if (strcmp(resource->nameStr.buffer, variableName) == 0)
        {
            ShaderResourceState *resourceState = &self->shaderResourceStates[i];
            LGFXFunctionVariable *toSet = LIST_GETREF(&resourceState->variableSlots, LGFXFunctionVariable, self->currentGroup);
            
            LGFXSetBufferDataFast(((LGFXBuffer *)toSet->currentValues)[0], (uint8_t*)ptr, size);
            return;
        }
    }
}
void ShaderFunctionState_SetTexture(ShaderFunctionState self, const char* variableName, LGFXTexture texture)
{
    ShaderFunctionState_SetTextures(self, variableName, &texture, 1);
}
void ShaderFunctionState_SetTextures(ShaderFunctionState self, const char* variableName, LGFXTexture *textures, size_t count)
{
    ShaderFunctionState_CheckVariableBatchAvailability(self, false);
    for (size_t i = 0; i < self->shader->numShaderResources; i++)
    {
        ShaderResource *resource = &self->shader->shaderResources[i];

        if (resource->nameStr.buffer == NULL)
        {
            break;
        }
        
        if (strcmp(resource->nameStr.buffer, variableName) == 0)
        {
            ShaderResourceState *resourceState = &self->shaderResourceStates[i];
            LGFXFunctionVariable *toSet = LIST_GETREF(&resourceState->variableSlots, LGFXFunctionVariable, self->currentGroup);
            
            for (size_t j = 0; j < count; j++)
            {
                ((LGFXTexture *)toSet->currentValues)[j] = textures[j];
            }
            return;
        }
    }
}
void ShaderFunctionState_SetSampler(ShaderFunctionState self, const char* variableName, LGFXSamplerState sampler)
{
    ShaderFunctionState_SetSamplers(self, variableName, &sampler, 1);
}
void ShaderFunctionState_SetSamplers(ShaderFunctionState self, const char* variableName, LGFXSamplerState *samplers, size_t count)
{
    ShaderFunctionState_CheckVariableBatchAvailability(self, false);
    for (size_t i = 0; i < self->shader->numShaderResources; i++)
    {
        ShaderResource *resource = &self->shader->shaderResources[i];

        if (resource->nameStr.buffer == NULL)
        {
            break;
        }
        
        if (strcmp(resource->nameStr.buffer, variableName) == 0)
        {
            ShaderResourceState *resourceState = &self->shaderResourceStates[i];
            LGFXFunctionVariable *toSet = LIST_GETREF(&resourceState->variableSlots, LGFXFunctionVariable, self->currentGroup);
            
            for (size_t j = 0; j < count; j++)
            {
                ((LGFXSamplerState *)toSet->currentValues)[j] = samplers[j];
            }
            return;
        }
    }
}
void ShaderFunctionState_SetComputeBuffer(ShaderFunctionState self, const char* variableName, LGFXBuffer computeBuffer)
{
    ShaderFunctionState_CheckVariableBatchAvailability(self, false);
    for (size_t i = 0; i < self->shader->numShaderResources; i++)
    {
        ShaderResource *resource = &self->shader->shaderResources[i];

        if (resource->nameStr.buffer == NULL)
        {
            break;
        }
        
        if (strcmp(resource->nameStr.buffer, variableName) == 0)
        {
            ShaderResourceState *resourceState = &self->shaderResourceStates[i];
            LGFXFunctionVariable *toSet = LIST_GETREF(&resourceState->variableSlots, LGFXFunctionVariable, self->currentGroup);
            
            ((LGFXBuffer *)toSet->currentValues)[0] = computeBuffer;
            return;
        }
    }
}
void ShaderFunctionState_SyncWithGPU(ShaderFunctionState self, LGFXCommandBuffer commandBuffer)
{
    LGFXFunctionVariable variables[32];
    uint32_t variablesCount = 0;
    for (size_t i = 0; i < self->shader->numShaderResources; i++)
    {
        ShaderResource *resource = &self->shader->shaderResources[i];
        if (resource->nameStr.buffer == NULL)
        {
            break;
        }
        variables[i] = LIST_GET(&self->shaderResourceStates[i].variableSlots, LGFXFunctionVariable, self->currentGroup);
        variablesCount++;
    }
    LGFXFunctionVariableBatch batch = LIST_GET(&self->variableSlotGroups, LGFXFunctionVariableBatch, self->currentGroup);
    LGFXFunctionSendVariablesToGPU(instance.device, batch, variables, variablesCount);
    LGFXUseFunctionVariables(commandBuffer, batch, self->shader->gpuFunction, 0);

    self->currentGroup += 1;
}
void ShaderFunctionState_Clear(ShaderFunctionState self)
{
    self->currentGroup = 0;
}
ShaderFunction ShaderFunctionState_GetUnderlyingShaderFunction(const ShaderFunctionState self)
{
    return self->shader;
}
LGFXFunctionVariableBatch ShaderFunctionState_GetCurrentVariableGroup(const ShaderFunctionState self)
{
    if (self->currentGroup >= self->variableSlotGroups.count)
    {
        return NULL;
    }
    return LIST_GET(&self->variableSlotGroups, LGFXFunctionVariableBatch, self->currentGroup);
}

size_t ShaderFunction_FromStream(LGFXDevice device, IAllocator allocator, IDataStream *stream, uint32_t numExtraBatchTypes, LGFXFunctionVariableBatchTemplate *extraBatchTypes, ShaderFunction *outputResult)
{
    const uint32_t fileVersion = IDataStream_ReadU32(stream);
    if (fileVersion == 1)
    {
        const ShaderFunctionImpl empty = {};
        ShaderFunction result = (ShaderFunction)malloc(sizeof(ShaderFunctionImpl));
        *result = empty;
        ArenaAllocator arena = ArenaAllocator_Create(GetCAllocator());
        IAllocator arenaAlloc = ArenaAllocator_AsAllocator(&arena);

        const uint32_t shaderType = IDataStream_ReadU32(stream);
        const uint32_t paramCount = IDataStream_ReadU32(stream);
        LGFXShaderResource *inputResources = NULL;
        LGFXFunctionCreateInfo info = {};

        result->numShaderResources = paramCount;
        result->shaderResources = (ShaderResource *)IAllocator_Allocate(allocator, sizeof(ShaderResource) * paramCount);
        for (uint32_t i = 0; i < paramCount; i++)
        {
            string str = IDataStream_ReadString(stream, allocator);
            uint32_t bindingSpace = IDataStream_ReadU32(stream);
            uint32_t bindingIndex = IDataStream_ReadU32(stream);

            ShaderResource newResource = {};
            newResource.nameStr = str;
            newResource.resource.variableName = newResource.nameStr.buffer;
            newResource.resource.set = bindingSpace;
            newResource.resource.binding = bindingIndex;
            //newResource.states = collections::List<LGFXFunctionVariable>(allocator);
            
            newResource.resource.arrayLength = IDataStream_ReadU32(stream);

            ShaderFunctionResourceType resourceType = (ShaderFunctionResourceType)IDataStream_ReadU32(stream);
            if (resourceType == ShaderFunctionResourceType_Uniform)
            {
                newResource.resource.type = LGFXShaderResourceType_Uniform;
                newResource.resource.size = IDataStream_ReadU32(stream);
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
                ArenaAllocator_Deinit(&arena);
                return 1;
            }
            
            result->shaderResources[i] = newResource;
        }
        
        inputResources = (LGFXShaderResource *)IAllocator_Allocate(arenaAlloc, sizeof(LGFXShaderResource) * paramCount);
        for (size_t i = 0; i < paramCount; i++)
        {
            inputResources[result->shaderResources[i].resource.binding] = result->shaderResources[i].resource;
        }

        info.uniformsCount = paramCount;
        info.uniforms = inputResources;

        LGFXFunctionType funcType;
        if (shaderType == 0)
        {
            //vertex-fragment
            funcType = (LGFXFunctionType)(LGFXFunctionType_Vertex | LGFXFunctionType_Fragment);

            ShaderFunctionStage stage1 = (ShaderFunctionStage)IDataStream_ReadU32(stream);
            if (stage1 != ShaderFunctionStage_Vertex)
            {
                ArenaAllocator_Deinit(&arena);
                return 1;
            }
            size_t lenBytes = IDataStream_ReadU32(stream);
            info.module1DataLength = lenBytes / 4;
            info.module1Data = (uint32_t *)IAllocator_Allocate(arenaAlloc, lenBytes);
            IDataStream_ReadByteArray(stream, info.module1Data, lenBytes);
            
            ShaderFunctionStage stage2 = (ShaderFunctionStage)IDataStream_ReadU32(stream);
            if (stage2 != ShaderFunctionStage_Fragment)
            {
                ArenaAllocator_Deinit(&arena);
                return 1;
            }
            lenBytes = IDataStream_ReadU32(stream);
            info.module2DataLength = lenBytes / 4;
            info.module2Data = (uint32_t *)IAllocator_Allocate(arenaAlloc, lenBytes);
            IDataStream_ReadByteArray(stream, info.module2Data, lenBytes);
        }
        else if (shaderType == 1)
        {
            //compute
            funcType = LGFXFunctionType_Compute;

            ShaderFunctionStage stage1 = (ShaderFunctionStage)IDataStream_ReadU32(stream);
            if (stage1 != ShaderFunctionStage_Compute)
            {
                ArenaAllocator_Deinit(&arena);
                return 1;
            }
            size_t lenBytes = IDataStream_ReadU32(stream);

            info.module1DataLength = lenBytes / 4;
            info.module1Data = (uint32_t *)IAllocator_Allocate(arenaAlloc, lenBytes);
            IDataStream_ReadByteArray(stream, info.module1Data, lenBytes);
        }
        else 
        {
            ArenaAllocator_Deinit(&arena);
            return 1;
        }

        info.type = funcType;
        info.extraFunctionVariableBatchTypes = extraBatchTypes;
        info.extraFunctionVariableBatchTypesCount = numExtraBatchTypes;

        result->gpuFunction = LGFXCreateFunction(device, &info);
        result->functionType = funcType;
        ArenaAllocator_Deinit(&arena);

        *outputResult = result;
    }
    else
    {
        return 1;
    }
    return 0;
}
size_t ShaderFunction_FromBytes(LGFXDevice device, const uint8_t* bytes, uint32_t numExtraBatchTypes, LGFXFunctionVariableBatchTemplate *extraBatchTypes, ShaderFunction *outputResult)
{
    ByteStreamReader reader = ByteStreamReader_Create(bytes, 0, 0xFFFFFFFF);
    IDataStream dataStream = ByteStreamReaderToStream(&reader);
    return ShaderFunction_FromStream(device, GetCAllocator(), &dataStream, numExtraBatchTypes, extraBatchTypes, outputResult);
}
size_t ShaderFunction_FromFile(LGFXDevice device, const char *filePath, uint32_t numExtraBatchTypes, LGFXFunctionVariableBatchTemplate *extraBatchTypes, ShaderFunction *outputResult)
{
    FILE *fs = fopen(filePath, "rb");
    if (fs == NULL)
    {
        return 1;
    }
    IDataStream ds = GetFileDataStream(fs);
    size_t errorCode = ShaderFunction_FromStream(device, GetCAllocator(), &ds, numExtraBatchTypes, extraBatchTypes, outputResult);
    fclose(fs);
    return errorCode;
}
LGFXFunction ShaderFunction_GetFunc(const ShaderFunction self)
{
    return self->gpuFunction;
}
LGFXFunctionType ShaderFunction_GetType(const ShaderFunction self)
{
    return self->functionType;
}
uint32_t ShaderFunction_GetResourcesCount(const ShaderFunction self)
{
    return self->numShaderResources;
}
const char *ShaderFunction_GetResourceName(const ShaderFunction self, uint32_t index)
{
    if (index >= self->numShaderResources)
    {
        return NULL;
    }
    return self->shaderResources[index].nameStr.buffer;
}
const LGFXShaderResource *ShaderFunction_GetResource(const ShaderFunction self, uint32_t index)
{
    if (index >= self->numShaderResources)
    {
        return NULL;
    }
    return &self->shaderResources[index].resource;
}
void ShaderFunction_Deinit(ShaderFunction self)
{
    if (self->numShaderResources > 0)
    {
        for (uint32_t i = 0; i < self->numShaderResources; i++)
        {
            String_Deinit(&self->shaderResources[i].nameStr);
        }
        free(self->shaderResources);
    }
    LGFXDestroyFunction(self->gpuFunction);
    free(self);
}