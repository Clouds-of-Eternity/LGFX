#include "AstralCanvas/ShaderPipeline.h"
#include "Hashmap.h"

typedef struct ShaderPipelineBindZone
{
    LGFXRenderProgram renderProgram;
    uint32_t renderPass;
} ShaderPipelineBindZone;

inline uint32_t ShaderPipelineBindZoneHash(const void *self)
{
    const ShaderPipelineBindZone zone = *(const ShaderPipelineBindZone *)self;
    uint32_t hash = 7;
    hash = hash * 31 + (uint32_t)(size_t)zone.renderProgram;
    hash = hash * 31 + zone.renderPass;
    return hash;
}
inline bool ShaderPipelineBindZoneEql(const void *ptr1, const void *ptr2)
{
    const ShaderPipelineBindZone A = *(const ShaderPipelineBindZone *)ptr1;
    const ShaderPipelineBindZone B = *(const ShaderPipelineBindZone *)ptr2;
    return A.renderProgram == B.renderProgram && A.renderPass == B.renderPass;
}

typedef struct ShaderPipelineCacheImpl
{
    LGFXShaderStateCreateInfo createInfo;
    HashMap zoneToInstance;
    LGFXDevice device;
} ShaderPipelineCacheImpl;

ShaderPipelineCache ShaderPipelineCache_Create(LGFXDevice device, LGFXShaderStateCreateInfo createInfo)
{
    ShaderPipelineCache result = (ShaderPipelineCache)malloc(sizeof(ShaderPipelineCacheImpl));

    result->createInfo = createInfo;
    result->device = device;
    result->zoneToInstance = HashMap_Create(sizeof(ShaderPipelineBindZone), sizeof(LGFXShaderState), GetCAllocator(), &ShaderPipelineBindZoneHash, &ShaderPipelineBindZoneEql);
    
    return result;
}
void ShaderPipelineCache_Clear(ShaderPipelineCache self)
{
    HashMapIterator iterator = HashMapIterator_From(&self->zoneToInstance);
    foreach (kvp, iterator)
    {
        LGFXShaderState state = *HMENTRY_GETVAL(kvp, LGFXShaderState, &self->zoneToInstance);
        LGFXDestroyShaderState(state);
    }
    HashMap_Clear(&self->zoneToInstance);
}
void ShaderPipelineCache_Deinit(ShaderPipelineCache self)
{
    HashMapIterator iterator = HashMapIterator_From(&self->zoneToInstance);
    foreach (kvp, iterator)
    {
        LGFXShaderState state = *HMENTRY_GETVAL(kvp, LGFXShaderState, &self->zoneToInstance);
        LGFXDestroyShaderState(state);
    }
    HashMap_Deinit(&self->zoneToInstance);
    free(self);
}
bool ShaderPipelineCache_HasCacheFor(const ShaderPipelineCache self, LGFXRenderProgram program, uint32_t renderPass)
{
    ShaderPipelineBindZone zone;
    zone.renderProgram = program;
    zone.renderPass = renderPass;

    return HashMap_Contains(&self->zoneToInstance, &zone);
}
LGFXShaderState ShaderPipelineCache_GetOrCreateFor(ShaderPipelineCache self, LGFXRenderProgram program, uint32_t renderPass)
{
    ShaderPipelineBindZone zone;
    zone.renderProgram = program;
    zone.renderPass = renderPass;

    LGFXShaderState state = HM_GET(LGFXShaderState, &self->zoneToInstance, &zone);//this->zoneToInstance.GetCopyOr(zone, NULL);
    if (state == NULL)
    {
        LGFXShaderStateCreateInfo createInfo = self->createInfo;
        createInfo.forRenderProgram = program;
        createInfo.forRenderPass = renderPass;
        state = LGFXCreateShaderState(self->device, &createInfo);

        HashMap_Add(&self->zoneToInstance, &zone, &state);
    }
    return state;
}