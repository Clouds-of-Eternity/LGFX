#include "Linxc.h"
#include "lgfx-astral/lgfx-astral-types.hpp"
#include "lgfx-astral/Application.hpp"
#include "lgfx-astral/Shader.hpp"

LGFXRenderProgram rp;

LGFXVertexDeclaration vertexDecl;
LGFXBuffer vertexBuffer;
LGFXBuffer indexBuffer;

AstralCanvas::Shader shader;
LGFXShaderState shaderState;

void Update(float deltaTime)
{

}
void Draw(float deltaTime, AstralCanvas::Window *window)
{
    LGFXSetViewport(window->mainCommandBuffer, {0, 0, (float)window->resolution.X, (float)window->resolution.Y});
    LGFXSetClipArea(window->mainCommandBuffer, {0, 0, (u32)window->resolution.X, (u32)window->resolution.Y});

    LGFXBeginRenderProgramSwapchain(rp, window->mainCommandBuffer, window->swapchain, {128, 128, 128, 255}, true);

    LGFXUseVertexBuffer(window->mainCommandBuffer, &vertexBuffer, 1);
    LGFXUseIndexBuffer(window->mainCommandBuffer, indexBuffer, 0);
    LGFXUseShaderState(window->mainCommandBuffer, shaderState);

    LGFXDrawIndexed(window->mainCommandBuffer, 3, 1, 0, 0, 0);

    LGFXEndRenderProgram(rp, window->mainCommandBuffer);
}
void PostEndDraw(float deltaTime)
{

}
void Init()
{
    LGFXDevice device = AstralCanvas::applicationInstance.device;

    //render program
    LGFXRenderAttachmentInfo attachments;
    attachments.clear = true;
    attachments.format = LGFXTextureFormat_BGRA8Unorm;
    attachments.outputType = LGFXRenderAttachmentOutput_ToScreen;
    attachments.samples = 1;

    i32 firstAttachment = 0;

    LGFXRenderPassInfo passes;
    passes.colorAttachmentIDs = &firstAttachment;
    passes.colorAttachmentsCount = 1;
    passes.depthAttachmentID = -1;
    passes.readAttachmentIDs = NULL;
    passes.readAttachmentsCount = 0;
    passes.resolveAttachmentID = -1;

    LGFXRenderProgramCreateInfo rpCreateInfo;
    rpCreateInfo.attachmentsCount = 1;
    rpCreateInfo.attachments = &attachments;
    rpCreateInfo.renderPassCount = 1;
    rpCreateInfo.renderPasses = &passes;
    rpCreateInfo.outputToBackbuffer = true;
    rp = LGFXCreateRenderProgram(device, &rpCreateInfo);

    //vertex buffer
    LGFXBufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_VertexBuffer | LGFXBufferUsage_TransferDest);
    bufferCreateInfo.memoryUsage = LGFXMemoryUsage_GPU_ONLY;
    bufferCreateInfo.size = sizeof(LGFX::VertexPositionColor) * 3;
    vertexBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);

    //index buffer
    bufferCreateInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_IndexBuffer | LGFXBufferUsage_TransferDest);
    bufferCreateInfo.size = sizeof(u32) * 3;
    indexBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);

    vertexDecl = LGFX::GetVertexPositionColorDecl();

    //vertex data
    LGFX::VertexPositionColor vertices[3] = {
        {
            Maths::Vec3(0.0, -1.0, 0.0),
            Maths::Vec4(1.0, 0.0, 0.0, 1.0)
        },
        {
            Maths::Vec3(-1.0, 1.0, 0.0),
            Maths::Vec4(0.0, 1.0, 0.0, 1.0)
        },
        {
            Maths::Vec3(1.0, 1.0, 0.0),
            Maths::Vec4(0.0, 0.0, 1.0, 1.0)
        }
    };
    LGFXSetBufferDataOptimizedData(vertexBuffer, NULL, (u8*)vertices, sizeof(vertices));

    u32 indices[3] = {
        0, 1, 2
    };
    LGFXSetBufferDataOptimizedData(indexBuffer, NULL, (u8 *)indices, sizeof(indices));

    //shader
    string fileContents = io::ReadFile(GetCAllocator(), "Triangle.shaderobj", false);
    if (AstralCanvas::CreateShaderFromString(device, GetCAllocator(), fileContents, &shader) != 0)
    {
        printf("Error loading shader json\n");
    }
    fileContents.deinit();

    //shader state
    LGFXShaderStateCreateInfo stateCreateInfo = {0};
    stateCreateInfo.blendState = ALPHA_BLEND;
    stateCreateInfo.cullMode = LGFXCullMode_None;
    stateCreateInfo.depthTest = false;
    stateCreateInfo.depthWrite = false;
    stateCreateInfo.primitiveType = LGFXPrimitiveType_TriangleList;
    stateCreateInfo.dynamicViewportScissor = true;
    stateCreateInfo.function = shader.gpuFunction;
    stateCreateInfo.vertexDeclarationCount = 1;
    stateCreateInfo.vertexDeclarations = &vertexDecl;
    stateCreateInfo.forRenderProgram = rp;
    stateCreateInfo.forRenderPass = 0;
    shaderState = LGFXCreateShaderState(device, &stateCreateInfo);
}
void Deinit()
{
    LGFXDestroyShaderState(shaderState);
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
    AstralCanvas::ApplicationInit(
        GetCAllocator(),
        string(GetCAllocator(), "Triangle"), 
        string(GetCAllocator(), "Astral.Canvas"),
        0, 0, 0.0f);

    AstralCanvas::applicationInstance.AddWindow("Triangle", 640, 480);
    AstralCanvas::applicationInstance.Run(&Update, &FixedUpdate, &Draw, &PostEndDraw, &Init, &Deinit);
}