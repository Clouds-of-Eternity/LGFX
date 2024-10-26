#include "Linxc.h"
#include "lgfx-astral/lgfx-astral-types.hpp"
#include "lgfx-astral/Application.hpp"
#include "lgfx-astral/Shader.hpp"
#include "random.hpp"

LGFXRenderProgram rp;

LGFXBuffer indexBuffer;

AstralCanvas::Shader renderShader;
LGFXShaderState renderShaderState;

float fpsTimer = 0.0f;

struct ShaderGlobalData
{
    Maths::Matrix4x4 view;
    Maths::Matrix4x4 projection;
    float elapsedTime;
};

void Update(float deltaTime)
{
    fpsTimer += deltaTime;
    if (fpsTimer >= 0.1f)
    {
        string title = string(GetCAllocator(), "Compute, Frames: ");
        title.Append(1.0f / deltaTime);
        AstralCanvas::applicationInstance.windows.ptr[0].SetWindowTitle(title);
        title.deinit();
        fpsTimer = 0.0f;
    }
}
void Draw(float deltaTime, AstralCanvas::Window *window)
{
    LGFXCommandBuffer mainCmds = window->mainCommandBuffer;
    LGFXBeginRenderProgramSwapchain(rp, mainCmds, window->swapchain, {0, 0, 0, 255}, true);
    
    Maths::Matrix4x4 matrices[2];
    //projection
    matrices[0] = Maths::Matrix4x4::CreateOrthographic(80.0f, 45.0f, -1000.0f, 1000.0f);
    //view
    matrices[1] = Maths::Matrix4x4::Identity();

    renderShader.SetShaderVariable("ShaderGlobalData", matrices, sizeof(matrices));
    renderShader.SyncUniformsWithGPU(mainCmds);

    LGFXSetViewport(mainCmds, {0, 0, (float)window->resolution.X, (float)window->resolution.Y});
    LGFXSetClipArea(mainCmds, {0, 0, (u32)window->resolution.X, (u32)window->resolution.Y});

    LGFXUseShaderState(mainCmds, renderShaderState);
    LGFXUseIndexBuffer(mainCmds, indexBuffer, 0);
    LGFXDrawIndexed(mainCmds, 6, 1, 0, 0, 0);
    
    LGFXEndRenderProgram(mainCmds);
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
    attachments.readByRenderTarget = false;

    i32 firstAttachment = 0;

    LGFXRenderPassInfo passes;
    passes.colorAttachmentIDs = &firstAttachment;
    passes.colorAttachmentsCount = 1;
    passes.depthAttachmentID = -1;
    passes.readAttachmentIDs = NULL;
    passes.readAttachmentsCount = 0;

    LGFXRenderProgramCreateInfo rpCreateInfo;
    rpCreateInfo.attachmentsCount = 1;
    rpCreateInfo.attachments = &attachments;
    rpCreateInfo.renderPassCount = 1;
    rpCreateInfo.renderPasses = &passes;
    rpCreateInfo.outputToBackbuffer = true;
    rp = LGFXCreateRenderProgram(device, &rpCreateInfo);

    //shader
    string fileContents = io::ReadFile(GetCAllocator(), "DrawSDFs.shaderobj", false);
    if (AstralCanvas::CreateShaderFromString(device, GetCAllocator(), fileContents, &renderShader) != 0)
    {
        printf("Error loading SDFs shader json\n");
    }
    fileContents.deinit();

    //index buffer
    LGFXBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_IndexBuffer | LGFXBufferUsage_TransferDest);
    bufferCreateInfo.memoryUsage = LGFXMemoryUsage_GPU_ONLY;
    bufferCreateInfo.size = sizeof(u32) * 6;
    indexBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);

    //shader state
    LGFXShaderStateCreateInfo stateCreateInfo = {0};
    stateCreateInfo.function = renderShader.gpuFunction;
    stateCreateInfo.cullMode = LGFXCullMode_None;
    stateCreateInfo.dynamicLineWidth = false;
    stateCreateInfo.dynamicViewportScissor = true;
    stateCreateInfo.primitiveType = LGFXPrimitiveType_TriangleList;
    stateCreateInfo.blendState = ALPHA_BLEND;
    stateCreateInfo.depthTest = false;
    stateCreateInfo.depthWrite = false;
    stateCreateInfo.vertexDeclarations = NULL;
    stateCreateInfo.vertexDeclarationCount = 0;
    stateCreateInfo.forRenderProgram = rp;
    stateCreateInfo.forRenderPass = 0;
    renderShaderState = LGFXCreateShaderState(device, &stateCreateInfo);
}
void Deinit()
{
    //LGFXDestroyShaderState(computeShaderState);
    //computeShader.deinit();
    LGFXDestroyBuffer(indexBuffer);
    LGFXDestroyShaderState(renderShaderState);
    renderShader.deinit();
    LGFXDestroyRenderProgram(rp);
}

i32 main()
{
    AstralCanvas::ApplicationInit(
        GetCAllocator(),
        string(GetCAllocator(), "SDFs"), 
        string(GetCAllocator(), "Astral.Canvas"),
        0, 0, 0.0f);

    AstralCanvas::applicationInstance.AddWindow("SDFs", 1920, 1080);
    AstralCanvas::applicationInstance.Run(&Update, &Draw, &PostEndDraw, &Init, &Deinit);
}