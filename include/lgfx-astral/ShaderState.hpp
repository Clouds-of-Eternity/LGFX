#pragma once
#include "Linxc.h"
#include "hashmap.hpp"
#include "lgfx-astral/Shader.hpp"
#include "lgfx/lgfx.h"

namespace AstralCanvas
{
    //Because the creation of a ShaderState handles requires a render program and pass,
    //we should cache and reuse ShaderState handles wherever possible, like when
    //using the same pipeline for the same pass
    struct ShaderStateBindZone
    {
        LGFXRenderProgram renderProgram;
        u32 renderPass;
    };
    inline u32 ShaderStateBindZoneHash(ShaderStateBindZone zone)
    {
        u32 hash = 7;
        hash = hash * 31 + (u32)(usize)zone.renderProgram;
        hash = hash * 31 + zone.renderPass;
        return hash;
    }
    inline bool ShaderStateBindZoneEql(ShaderStateBindZone A, ShaderStateBindZone B)
    {
        return A.renderProgram == B.renderProgram && A.renderPass == B.renderPass;
    }

    struct ShaderState
    {
        LGFXShaderStateCreateInfo createInfo;
        collections::hashmap<ShaderStateBindZone, LGFXShaderState> zoneToInstance;
        LGFXDevice device;

        ShaderState();
        ShaderState(IAllocator allocator, LGFXDevice device, LGFXShaderStateCreateInfo createInfo);
        void deinit();
        
        /// Retrieves or creates an instance of this state for use in the given render program and pass.
        LGFXShaderState GetOrCreateFor(LGFXRenderProgram program, u32 renderPassToUse);
    };
}