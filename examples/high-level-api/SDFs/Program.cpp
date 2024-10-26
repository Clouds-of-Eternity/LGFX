#include "Linxc.h"
#include "lgfx-astral/lgfx-astral-types.hpp"
#include "lgfx-astral/Application.hpp"
#include "lgfx-astral/Shader.hpp"
#include "random.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

LGFXRenderProgram rp;

LGFXBuffer indexBuffer;
LGFXTexture texture;
LGFXSamplerState samplerState;

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
    matrices[0] = Maths::Matrix4x4::CreateOrthographic(40.0f, 22.5f, -1000.0f, 1000.0f);
    //view
    matrices[1] = Maths::Matrix4x4::Identity(); // Maths::Matrix4x4::CreateLookAt(Maths::Vec3(0.0f, 0.0f, -10.0f), Maths::Vec3(0.0f, 0.0f, 1.0f), Maths::Vec3(0.0f, -1.0f, 0.0f));

    renderShader.SetShaderVariable("ShaderGlobalData", matrices, sizeof(matrices));
    renderShader.SetShaderVariableSampler("samplerState", samplerState);
    renderShader.SetShaderVariableTexture("noiseMap", texture);
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

    u32 indices[] = {0, 1, 2, 3, 0, 2};
    LGFXSetBufferDataOptimizedData(indexBuffer, NULL, (u8*)indices, 6 * sizeof(u32));

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

    //sampler state
    LGFXSamplerStateCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.repeatModeU = LGFXSamplerRepeatMode_Repeat;
    samplerCreateInfo.repeatModeV = LGFXSamplerRepeatMode_Repeat;
    samplerCreateInfo.repeatModeW = LGFXSamplerRepeatMode_Repeat;
    samplerCreateInfo.minimizationFilter = LGFXFilterType_Linear;
    samplerCreateInfo.magnificationFilter = LGFXFilterType_Linear;
    samplerState = LGFXCreateSamplerState(device, &samplerCreateInfo);

    //texture
    i32 w = 0;
    i32 h = 0;
    i32 comp = 0;
    stbi_uc *bytes = stbi_load("Noise.png", &w, &h, &comp, 4);
    LGFXTextureCreateInfo textureCreateInfo = {};
    textureCreateInfo.depth = 1;
    textureCreateInfo.format = LGFXTextureFormat_RGBA8UnormSrgb;
    textureCreateInfo.width = w;
    textureCreateInfo.height = h;
    textureCreateInfo.mipLevels = 1;
    textureCreateInfo.sampleCount = 1;
    textureCreateInfo.usage = (LGFXTextureUsage)(LGFXTextureUsage_Sampled | LGFXTextureUsage_TransferDest);
    texture = LGFXCreateTexture(device, &textureCreateInfo);

    LGFXTextureSetData(device, texture, bytes, w * h * comp);

    stbi_image_free(bytes);
}
void Deinit()
{
    //LGFXDestroyShaderState(computeShaderState);
    //computeShader.deinit();
    LGFXDestroyBuffer(indexBuffer);
    LGFXDestroyShaderState(renderShaderState);
    renderShader.deinit();
    LGFXDestroyRenderProgram(rp);
    LGFXDestroySamplerState(samplerState);
    LGFXDestroyTexture(texture);
}

i32 main()
{
    AstralCanvas::ApplicationInit(
        GetCAllocator(),
        string(GetCAllocator(), "SDFs"), 
        string(GetCAllocator(), "Astral.Canvas"),
        0, 0, 165.0f);

    AstralCanvas::applicationInstance.AddWindow("SDFs", 1920, 1080);
    AstralCanvas::applicationInstance.Run(&Update, &Draw, &PostEndDraw, &Init, &Deinit);
}