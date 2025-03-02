#include "Linxc.h"
#include "lgfx-astral/lgfx-astral-types.hpp"
#include "lgfx-astral/Application.hpp"
#include "lgfx-astral/Shader.hpp"
#include "random.hpp"

#define PARTICLES_COUNT 1048576

LGFXRenderProgram rp;

LGFXBuffer inputBuffer;
LGFXBuffer outputBuffer;
LGFXBuffer indexBuffer;
LGFXVertexDeclaration particleAsVertexDecl;

AstralCanvas::Shader computeShader;
LGFXShaderState computeShaderState;

AstralCanvas::Shader renderShader;
LGFXShaderState renderShaderState;

struct Particle
{
    Maths::Vec4 position;
    Maths::Vec4 velocity;
};

float fpsTimer = 0.0f;

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

    computeShader.SetShaderVariableComputeBuffer("ParticlesIn", inputBuffer);
    computeShader.SetShaderVariableComputeBuffer("ParticlesOut", outputBuffer);
    computeShader.SetShaderVariable("TimeData", &deltaTime, sizeof(float));
    computeShader.SyncUniformsWithGPU(mainCmds);

    //first run compute :3
    LGFXUseShaderState(mainCmds, computeShaderState);
    LGFXDispatchCompute(mainCmds, PARTICLES_COUNT / 256, 1, 1);

    //then run draw :>
    LGFXAwaitWriteFunction(mainCmds, LGFXFunctionType_Compute, LGFXFunctionOperationType_VertexBufferRead);

    LGFXBeginRenderProgramSwapchain(rp, mainCmds, window->swapchain, {0, 0, 0, 255}, true);

    Maths::Matrix4x4 matrices[2];
    //projection
    matrices[0] = Maths::Matrix4x4::CreateOrthographic(80.0f, 45.0f, -1000.0f, 1000.0f);
    //view
    matrices[1] = Maths::Matrix4x4::Identity();

    renderShader.SetShaderVariable("Matrices", matrices, sizeof(matrices));
    renderShader.SyncUniformsWithGPU(mainCmds);

    LGFXSetViewport(mainCmds, {0, 0, (float)window->resolution.X, (float)window->resolution.Y});
    LGFXSetClipArea(mainCmds, {0, 0, (u32)window->resolution.X, (u32)window->resolution.Y});

    LGFXUseShaderState(mainCmds, renderShaderState);
    LGFXUseVertexBuffer(mainCmds, &outputBuffer, 1);
    LGFXUseIndexBuffer(mainCmds, indexBuffer, 0);

    LGFXDrawIndexed(mainCmds, 6, PARTICLES_COUNT, 0, 0,0 );

    LGFXEndRenderProgram(rp, mainCmds);

    computeShader.descriptorForThisDrawCall = 0;
    renderShader.descriptorForThisDrawCall = 0;

    //swap buffers
    LGFXBuffer temp = outputBuffer;
    outputBuffer = inputBuffer;
    inputBuffer = temp;
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
    rpCreateInfo.maxBackbufferTexturesCount = LGFXSwapchainGetBackbufferTexturesCount(AstralCanvas::applicationInstance.windows.ptr[0].swapchain);
    rp = LGFXCreateRenderProgram(device, &rpCreateInfo);

    //compute buffer
    LGFXBufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_StorageBuffer | LGFXBufferUsage_VertexBuffer | LGFXBufferUsage_TransferDest);
    bufferCreateInfo.memoryUsage = LGFXMemoryUsage_GPU_ONLY;
    bufferCreateInfo.size = sizeof(Particle) * PARTICLES_COUNT;
    inputBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);
    outputBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);
    //particles
    Random random = Random::FromTime();

    Particle *particles = (Particle *)malloc(sizeof(Particle) * PARTICLES_COUNT);
    for (u32 i = 0; i < PARTICLES_COUNT; i++)
    {
        particles[i] = {0};
        particles[i].position = Maths::Vec4(random.NextFloatRange(-40.0f, 40.0f), random.NextFloatRange(-22.5f, 22.5f), random.NextFloatRange(0.0f, 5.0f), 0.0f);
        particles[i].velocity = Maths::Vec4(random.NextFloatRange(-2.0f, 2.0f), random.NextFloatRange(-2.0f, 2.0f), 0.0f, 0.0f);
    }
    LGFXSetBufferDataOptimizedData(inputBuffer, NULL, (u8 *)particles, sizeof(Particle) * PARTICLES_COUNT);

    LGFXVertexElementFormat formats[2];
    formats[0] = LGFXVertexElementFormat_Vector4;
    formats[1] = LGFXVertexElementFormat_Vector4;
    particleAsVertexDecl = LGFXCreateVertexDeclaration(formats, 2, true, true);

    //index buffer
    bufferCreateInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_IndexBuffer | LGFXBufferUsage_TransferDest);
    bufferCreateInfo.memoryUsage = LGFXMemoryUsage_GPU_ONLY;
    bufferCreateInfo.size = sizeof(u32) * 6;
    indexBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);

    u32 indices[] = {0, 1, 2, 3, 0, 2};
    LGFXSetBufferDataOptimizedData(indexBuffer, NULL, (u8*)indices, 6 * sizeof(u32));

    //shader
    string fileContents = io::ReadFile(GetCAllocator(), "UpdateParticles.shaderobj", false);
    if (AstralCanvas::CreateShaderFromString(device, GetCAllocator(), fileContents, &computeShader) != 0)
    {
        printf("Error loading compute shader json\n");
    }
    fileContents.deinit();

    fileContents = io::ReadFile(GetCAllocator(), "DrawParticles.shaderobj", false);
    if (AstralCanvas::CreateShaderFromString(device, GetCAllocator(), fileContents, &renderShader) != 0)
    {
        printf("Error loading render shader json\n");
    }
    fileContents.deinit();

    //shader state
    LGFXShaderStateCreateInfo stateCreateInfo = {0};
    stateCreateInfo.function = computeShader.gpuFunction;
    computeShaderState = LGFXCreateShaderState(device, &stateCreateInfo);

    stateCreateInfo.function = renderShader.gpuFunction;
    stateCreateInfo.cullMode = LGFXCullMode_None;
    stateCreateInfo.dynamicLineWidth = false;
    stateCreateInfo.dynamicViewportScissor = true;
    stateCreateInfo.primitiveType = LGFXPrimitiveType_TriangleList;
    stateCreateInfo.blendState = ALPHA_BLEND;
    stateCreateInfo.depthTest = false;
    stateCreateInfo.depthWrite = false;
    stateCreateInfo.vertexDeclarations = &particleAsVertexDecl;
    stateCreateInfo.vertexDeclarationCount = 1;
    stateCreateInfo.forRenderProgram = rp;
    stateCreateInfo.forRenderPass = 0;
    renderShaderState = LGFXCreateShaderState(device, &stateCreateInfo);
}
void Deinit()
{
    LGFXDestroyShaderState(computeShaderState);
    LGFXDestroyShaderState(renderShaderState);
    computeShader.deinit();
    renderShader.deinit();
    LGFXDestroyBuffer(indexBuffer);
    LGFXDestroyBuffer(inputBuffer);
    LGFXDestroyBuffer(outputBuffer);
    LGFXDestroyRenderProgram(rp);
}

void FixedUpdate(float deltaTime)
{

}

i32 main()
{
    AstralCanvas::ApplicationInit(
        GetCAllocator(),
        string(GetCAllocator(), "Compute"), 
        string(GetCAllocator(), "Astral.Canvas"),
        0, 0, 0.0f);

    AstralCanvas::applicationInstance.AddWindow("Compute", 1920, 1080);
    AstralCanvas::applicationInstance.Run(&Update, &FixedUpdate, &Draw, &PostEndDraw, &Init, &Deinit);
}