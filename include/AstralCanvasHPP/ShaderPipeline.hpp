#pragma once
#include "Linxc.h"
#include "Hash.hpp"
#include "HashMap.hpp"
#include "lgfx/lgfx.h"

namespace AstralCanvas
{
    struct ShaderPipelineState
    {
        const LGFXRenderProgram forRenderProgram;
        u32 forRenderPass;

        LGFXBlendState blendState;
        bool depthTest;
        bool depthWrite;
    };

    u32 ShaderPipelineStateHash(ShaderPipelineState state);
    bool ShaderPipelineStateEql(ShaderPipelineState A, ShaderPipelineState B);

    struct ShaderPipeline
    {
        LGFXShaderPipelineCreateInfo createInfo;
        collections::HashMap<ShaderPipelineState, LGFXShaderPipeline> stateToInstance;
        LGFXDevice device;

        ShaderPipeline();
        ShaderPipeline(IAllocator allocator, LGFXDevice device, LGFXShaderPipelineCreateInfo createInfo);
        void deinit();
        
        /// Retrieves or creates an instance of this state for use in the given render program and pass.
        LGFXShaderPipeline GetOrCreateFor(ShaderPipelineState state);
    };
}