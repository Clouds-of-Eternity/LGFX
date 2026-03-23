#pragma once
#include "Linxc.h"
#include "HashMap.hpp"
#include "lgfx/lgfx.h"

namespace AstralCanvas
{
    //Because the creation of a ShaderState handles requires a render program and pass,
    //we should cache and reuse ShaderState handles wherever possible, like when
    //using the same pipeline for the same pass
    struct ShaderPipelineBindZone
    {
        LGFXRenderProgram renderProgram;
        u32 renderPass;
    };
    inline u32 ShaderPipelineBindZoneHash(ShaderPipelineBindZone zone)
    {
        u32 hash = 7;
        hash = hash * 31 + (u32)(usize)zone.renderProgram;
        hash = hash * 31 + zone.renderPass;
        return hash;
    }
    inline bool ShaderPipelineBindZoneEql(ShaderPipelineBindZone A, ShaderPipelineBindZone B)
    {
        return A.renderProgram == B.renderProgram && A.renderPass == B.renderPass;
    }

    struct ShaderPipeline
    {
        LGFXShaderPipelineCreateInfo createInfo;
        collections::HashMap<ShaderPipelineBindZone, LGFXShaderPipeline> zoneToInstance;
        LGFXDevice device;

        ShaderPipeline();
        ShaderPipeline(IAllocator allocator, LGFXDevice device, LGFXShaderPipelineCreateInfo createInfo);
        void deinit();
        
        /// Retrieves or creates an instance of this state for use in the given render program and pass.
        LGFXShaderPipeline GetOrCreateFor(LGFXRenderProgram program, u32 renderPassToUse);
    };
}