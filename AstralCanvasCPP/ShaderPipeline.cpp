#include "AstralCanvasHPP/ShaderPipeline.hpp"

namespace AstralCanvas
{
    u32 ShaderPipelineStateHash(ShaderPipelineState state)
    {
        return Murmur3((const u8 *)&state, sizeof(ShaderPipelineState));
    };
    bool ShaderPipelineStateEql(ShaderPipelineState A, ShaderPipelineState B)
    {
        return memcmp(&A, &B, sizeof(ShaderPipelineState)) == 0;
    }

    ShaderPipeline::ShaderPipeline()
    {
        this->device = NULL;
        this->createInfo = {};
        this->stateToInstance = collections::HashMap<AstralCanvas::ShaderPipelineState, LGFXShaderPipeline>();
    }
    ShaderPipeline::ShaderPipeline(IAllocator allocator, LGFXDevice device, LGFXShaderPipelineCreateInfo createInfo)
    {
        this->createInfo = createInfo;
        this->stateToInstance = collections::HashMap<AstralCanvas::ShaderPipelineState, LGFXShaderPipeline>(allocator, &ShaderPipelineStateHash, &ShaderPipelineStateEql);
        this->device = device;
    }
    void ShaderPipeline::deinit()
    {
        if (this->createInfo.vertexDeclarations != NULL)
        {
            free(this->createInfo.vertexDeclarations);
        }
        auto iterator = this->stateToInstance.GetIterator();
        foreach (kvp, iterator)
        {
            LGFXDestroyShaderPipeline(kvp->value);
        }
        this->stateToInstance.deinit();
    }
    
    /// Retrieves or creates an instance of this pipeline for use in the given render program and pass.
    LGFXShaderPipeline ShaderPipeline::GetOrCreateFor(ShaderPipelineState state)
    {
        LGFXShaderPipeline pipeline = this->stateToInstance.GetCopyOr(state, NULL);
        if (pipeline == NULL)
        {
            LGFXShaderPipelineCreateInfo info = this->createInfo;
            info.forRenderProgram = state.forRenderProgram;
            info.forRenderPass = state.forRenderPass;
            info.blendState = state.blendState;
            info.depthTest = state.depthTest;
            info.depthWrite = state.depthWrite;

            pipeline = LGFXCreateShaderPipeline(this->device, &info);
            this->stateToInstance.Add(state, pipeline);
        }
        return pipeline;
    }
}