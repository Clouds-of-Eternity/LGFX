#pragma once
#include "Allocator.h"
#include "lgfx/lgfx.h"
#include "Strings.h"

typedef struct ShaderFunctionImpl *ShaderFunction;
typedef struct ShaderFunctionStateImpl *ShaderFunctionState;

DynamicFunction ShaderFunctionState ShaderFunction_CreateUseState(ShaderFunction function);
DynamicFunction void ShaderFunctionState_Deinit(ShaderFunctionState self);

DynamicFunction void ShaderFunctionState_CheckVariableBatchAvailability(ShaderFunctionState self, bool forceCreateNewBatch);
DynamicFunction void ShaderFunctionState_SetUniform(ShaderFunctionState self, const char* variableName, void* ptr, size_t size);
DynamicFunction void ShaderFunctionState_SetTexture(ShaderFunctionState self, const char* variableName, LGFXTexture texture);
DynamicFunction void ShaderFunctionState_SetTextures(ShaderFunctionState self, const char* variableName, LGFXTexture *textures, size_t count);
DynamicFunction void ShaderFunctionState_SetSampler(ShaderFunctionState self, const char* variableName, LGFXSamplerState sampler);
DynamicFunction void ShaderFunctionState_SetSamplers(ShaderFunctionState self, const char* variableName, LGFXSamplerState *samplers, size_t count);
DynamicFunction void ShaderFunctionState_SetComputeBuffer(ShaderFunctionState self, const char* variableName, LGFXBuffer computeBuffer);
DynamicFunction void ShaderFunctionState_Clear(ShaderFunctionState self);
DynamicFunction void ShaderFunctionState_SyncWithGPU(ShaderFunctionState self, LGFXCommandBuffer commandBuffer);
DynamicFunction ShaderFunction ShaderFunctionState_GetUnderlyingShaderFunction(const ShaderFunctionState self);
DynamicFunction LGFXFunctionVariableBatch ShaderFunctionState_GetCurrentVariableGroup(const ShaderFunctionState self);

DynamicFunction size_t ShaderFunction_FromString(LGFXDevice device, const char *jsonString, size_t stringLengthWithNullTerminator, ShaderFunction *outputResult);
DynamicFunction size_t ShaderFunction_FromFile(LGFXDevice device, const char *filePath, ShaderFunction *outputResult);