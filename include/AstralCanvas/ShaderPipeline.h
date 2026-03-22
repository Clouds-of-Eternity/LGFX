#pragma once
#include "AstralCanvas/ShaderFunction.h"

typedef struct ShaderPipelineCacheImpl *ShaderPipelineCache;

DynamicFunction ShaderPipelineCache ShaderPipelineCache_Create(LGFXDevice device, LGFXShaderStateCreateInfo createInfo);
DynamicFunction void ShaderPipelineCache_Deinit(ShaderPipelineCache self);
DynamicFunction void ShaderPipelineCache_Clear(ShaderPipelineCache self);
DynamicFunction bool ShaderPipelineCache_HasCacheFor(ShaderPipelineCache self, LGFXRenderProgram program, uint32_t renderPass);
DynamicFunction LGFXShaderState ShaderPipelineCache_GetOrCreateFor(ShaderPipelineCache self, LGFXRenderProgram program, uint32_t renderPass);