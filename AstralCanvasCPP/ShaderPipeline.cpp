#include "AstralCanvasHPP/ShaderPipeline.hpp"

namespace AstralCanvas
{
    ShaderPipeline::ShaderPipeline()
    {
        this->device = NULL;
        this->createInfo = {};
        this->zoneToInstance = collections::HashMap<AstralCanvas::ShaderPipelineBindZone, LGFXShaderPipeline>();
    }
    ShaderPipeline::ShaderPipeline(IAllocator allocator, LGFXDevice device, LGFXShaderPipelineCreateInfo createInfo)
    {
        this->createInfo = createInfo;
        this->zoneToInstance = collections::HashMap<AstralCanvas::ShaderPipelineBindZone, LGFXShaderPipeline>(allocator, &ShaderPipelineBindZoneHash, &ShaderPipelineBindZoneEql);
        this->device = device;
    }
    void ShaderPipeline::deinit()
    {
        if (this->createInfo.vertexDeclarations != NULL)
        {
            free(this->createInfo.vertexDeclarations);
        }
        auto iterator = this->zoneToInstance.GetIterator();
        foreach (kvp, iterator)
        {
            LGFXDestroyShaderPipeline(kvp->value);
        }
        this->zoneToInstance.deinit();
    }
    
    /// Retrieves or creates an instance of this pipeline for use in the given render program and pass.
    LGFXShaderPipeline ShaderPipeline::GetOrCreateFor(LGFXRenderProgram program, u32 renderPassToUse)
    {
        ShaderPipelineBindZone zone;
        zone.renderProgram = program;
        zone.renderPass = renderPassToUse;

        LGFXShaderPipeline state = this->zoneToInstance.GetCopyOr(zone, NULL);
        if (state == NULL)
        {
            this->createInfo.forRenderProgram = program;
            this->createInfo.forRenderPass = renderPassToUse;
            state = LGFXCreateShaderPipeline(this->device, &this->createInfo);
            this->zoneToInstance.Add(zone, state);
        }
        return state;
    }
}