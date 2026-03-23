#include "Linxc.h"
#include "AstralCanvasHPP/Application.hpp"
#include "AstralCanvasHPP/ShaderFunction.hpp"

LGFXRenderProgram rp;

LGFXVertexDeclaration vertexDecl;
LGFXBuffer vertexBuffer;
LGFXBuffer indexBuffer;

AstralCanvas::ShaderFunction shader;
LGFXShaderPipeline shaderPipeline;
bool initializedSuccessfully;

void Update(float deltaTime)
{

}
void Draw(float deltaTime, AstralCanvas::Window *window)
{
    if (initializedSuccessfully)
    {
        LGFXSetViewport(window->mainCommandBuffer, {0, 0, (float)window->frameBufferSize.X, (float)window->frameBufferSize.Y});
        LGFXSetClipArea(window->mainCommandBuffer, {0, 0, (u32)window->frameBufferSize.X, (u32)window->frameBufferSize.Y});

        LGFXBeginRenderProgramSwapchain(rp, window->mainCommandBuffer, window->swapchain, {128, 128, 128, 255}, true);

        LGFXUseVertexBuffer(window->mainCommandBuffer, &vertexBuffer, 1);
        LGFXUseIndexBuffer(window->mainCommandBuffer, indexBuffer, 0);
        LGFXUseShaderPipeline(window->mainCommandBuffer, shaderPipeline);

        LGFXDrawIndexed(window->mainCommandBuffer, 3, 1, 0, 0, 0);

        LGFXEndRenderProgram(rp, window->mainCommandBuffer);
    }
}
void PostEndDraw(float deltaTime)
{

}
struct VertexPositionColor
{
    Maths::Vec4 position;
    Maths::Vec4 color;

    inline VertexPositionColor(Maths::Vec3 position, Maths::Vec4 color)
    {
        this->position = Maths::Vec4(position, 0.0f);
        this->color = color;
    }
};

inline LGFXVertexDeclaration GetVertexPositionColorDecl()
{
    LGFXVertexElementFormat formats[2] = {
        LGFXVertexElementFormat_Vector4,
        LGFXVertexElementFormat_Vector4
    };
    return LGFXCreateVertexDeclaration(formats, 2, false, true);
}
void Init()
{
    LGFXDevice device = AstralCanvas::applicationInstance.device;

    //render program
    LGFXRenderAttachmentInfo attachments = {};
    attachments.clear = true;
    attachments.format = LGFXTextureFormat_BGRA8Unorm;
    attachments.outputType = LGFXRenderAttachmentOutput_ToScreen;
    attachments.samples = 1;

    i32 firstAttachment = 0;

    LGFXRenderPassInfo passes = {};
    passes.colorAttachmentIDs = &firstAttachment;
    passes.colorAttachmentsCount = 1;
    passes.depthAttachmentID = -1;
    passes.readAttachmentIDs = NULL;
    passes.readAttachmentsCount = 0;
    passes.resolveAttachmentID = -1;

    LGFXRenderProgramCreateInfo rpCreateInfo = {};
    rpCreateInfo.attachmentsCount = 1;
    rpCreateInfo.attachments = &attachments;
    rpCreateInfo.renderPassCount = 1;
    rpCreateInfo.renderPasses = &passes;
    rpCreateInfo.outputToBackbuffer = true;
    rpCreateInfo.maxBackbufferTexturesCount = LGFXSwapchainGetBackbufferTexturesCount(AstralCanvas::applicationInstance.windows.ptr[0]->swapchain);
    rp = LGFXCreateRenderProgram(device, &rpCreateInfo);

    //vertex buffer
    LGFXBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_VertexBuffer | LGFXBufferUsage_TransferDest);
    bufferCreateInfo.memoryUsage = LGFXMemoryUsage_GPU_ONLY;
    bufferCreateInfo.size = sizeof(VertexPositionColor) * 3;
    vertexBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);

    //index buffer
    bufferCreateInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_IndexBuffer | LGFXBufferUsage_TransferDest);
    bufferCreateInfo.size = sizeof(u32) * 3;
    indexBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);

    vertexDecl = GetVertexPositionColorDecl();

    //vertex data
    VertexPositionColor vertices[3] = {
        VertexPositionColor(Maths::Vec3(0.0, -1.0, 0.0), Maths::Vec4(1.0, 0.0, 0.0, 1.0)),
        VertexPositionColor(Maths::Vec3(-1.0, 1.0, 0.0), Maths::Vec4(0.0, 1.0, 0.0, 1.0)),
        VertexPositionColor(Maths::Vec3(1.0, 1.0, 0.0), Maths::Vec4(0.0, 0.0, 1.0, 1.0))
    };
    LGFXSetBufferDataOptimizedData(vertexBuffer, NULL, (u8*)vertices, sizeof(vertices));

    u32 indices[3] = {
        0, 1, 2
    };
    LGFXSetBufferDataOptimizedData(indexBuffer, NULL, (u8 *)indices, sizeof(indices));

    //shader
    usize errorCode = AstralCanvas::CreateShaderFromSFNFilePath(device, GetCAllocator(), "Triangle.sfn", &shader);
    if (errorCode != 0)
    {
        printf("Error loading shader binary\n");
        initializedSuccessfully = false;
    }

    //shader state
    LGFXShaderPipelineCreateInfo pipelineCreateInfo = {0};
    pipelineCreateInfo.blendState = ALPHA_BLEND;
    pipelineCreateInfo.cullMode = LGFXCullMode_None;
    pipelineCreateInfo.depthTest = false;
    pipelineCreateInfo.depthWrite = false;
    pipelineCreateInfo.primitiveType = LGFXPrimitiveType_TriangleList;
    pipelineCreateInfo.dynamicViewportScissor = true;
    pipelineCreateInfo.function = shader.gpuFunction;
    pipelineCreateInfo.vertexDeclarationCount = 1;
    pipelineCreateInfo.vertexDeclarations = &vertexDecl;
    pipelineCreateInfo.forRenderProgram = rp;
    pipelineCreateInfo.forRenderPass = 0;
    pipelineCreateInfo.entryPoint1Name = "VertexFunction";
    pipelineCreateInfo.entryPoint2Name = "FragmentFunction";
    shaderPipeline = LGFXCreateShaderPipeline(device, &pipelineCreateInfo);
}
void Deinit()
{
    LGFXDestroyShaderPipeline(shaderPipeline);
    shader.deinit();
    LGFXDestroyBuffer(vertexBuffer);
    LGFXDestroyBuffer(indexBuffer);
    LGFXDestroyRenderProgram(rp);
}

void FixedUpdate(float deltaTime)
{

}

i32 main()
{
    initializedSuccessfully = true;
    AstralCanvas::applicationInstance = AstralCanvas::Application(
        GetCAllocator(),
        "Triangle", 
        "Astral.Canvas",
        0, 0, 0.0f, false);

    AstralCanvas::applicationInstance.AddWindow("Triangle", 640, 480, true, false, false, NULL, 0, 0, LGFXSwapchainPresentationMode_Fifo);
    AstralCanvas::applicationInstance.Run(&Update, &FixedUpdate, &Draw, &PostEndDraw, &Init, &Deinit);
}