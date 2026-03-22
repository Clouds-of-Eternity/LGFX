#include "AstralCanvas/Application.h"
#include "AstralCanvas/ShaderFunction.h"
#include "Random.h"
#include "Maths/Matrix4x4.h"

#define PARTICLES_COUNT 1048576
bool initializedSuccessfully;

LGFXRenderProgram rp;

LGFXBuffer inputBuffer;
LGFXBuffer outputBuffer;
LGFXBuffer indexBuffer;
LGFXVertexDeclaration particleAsVertexDecl;

ShaderFunction computeShader;
ShaderFunctionState computeShaderState;
LGFXShaderState computeShaderPipeline;

ShaderFunction renderShader;
ShaderFunctionState renderShaderState;
LGFXShaderState renderShaderPipeline;

typedef struct Particle
{
    Vec4 position;
    Vec4 velocity;
} Particle;
typedef struct ContextData
{
    uint32_t particlesCount;
    float deltaTime;
} ContextData;

float fpsTimer = 0.0f;

void Init()
{
    LGFXDevice device = AstralCanvas_GetGraphicsDevice();

    //render program
    LGFXRenderAttachmentInfo attachments;
    attachments.clear = true;
    attachments.format = LGFXTextureFormat_BGRA8Unorm;
    attachments.outputType = LGFXRenderAttachmentOutput_ToScreen;
    attachments.samples = 1;

    int32_t firstAttachment = 0;

    LGFXRenderPassInfo passes;
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
    rpCreateInfo.maxBackbufferTexturesCount = LGFXSwapchainGetBackbufferTexturesCount(AstralCanvas_GetWindowSwapchain(0));
    rp = LGFXCreateRenderProgram(device, &rpCreateInfo);

    //compute buffer
    LGFXBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_StorageBuffer | LGFXBufferUsage_VertexBuffer | LGFXBufferUsage_TransferDest);
    bufferCreateInfo.memoryUsage = LGFXMemoryUsage_GPU_ONLY;
    bufferCreateInfo.size = sizeof(Particle) * PARTICLES_COUNT;
    inputBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);
    outputBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);
    //particles
    Random random = Random_FromTime(0);

    Particle *particles = (Particle *)malloc(sizeof(Particle) * PARTICLES_COUNT);
    for (uint32_t i = 0; i < PARTICLES_COUNT; i++)
    {
        particles[i].position = CreateVec4(Random_NextFloatRange(&random, -40.0f, 40.0f), Random_NextFloatRange(&random, -22.5f, 22.5f), Random_NextFloatRange(&random, 0.0f, 5.0f), 0.0f);
        particles[i].velocity = CreateVec4(Random_NextFloatRange(&random, -2.0f, 2.0f), Random_NextFloatRange(&random, -2.0f, 2.0f), 0.0f, Random_NextFloat(&random, 1.0f));
    }
    LGFXSetBufferDataOptimizedData(inputBuffer, NULL, (uint8_t *)particles, sizeof(Particle) * PARTICLES_COUNT);

    LGFXVertexElementFormat formats[2];
    formats[0] = LGFXVertexElementFormat_Vector4;
    formats[1] = LGFXVertexElementFormat_Vector4;
    particleAsVertexDecl = LGFXCreateVertexDeclaration(formats, 2, true, true);

    //index buffer
    bufferCreateInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_IndexBuffer | LGFXBufferUsage_TransferDest);
    bufferCreateInfo.memoryUsage = LGFXMemoryUsage_GPU_ONLY;
    bufferCreateInfo.size = sizeof(uint32_t) * 6;
    indexBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);

    uint32_t indices[] = {0, 1, 2, 3, 0, 2};
    LGFXSetBufferDataOptimizedData(indexBuffer, NULL, (uint8_t*)indices, 6 * sizeof(uint32_t));

    //shader
    if (ShaderFunction_FromFile(device, "UpdateParticles.sfn", &computeShader) != 0)
    {
        printf("Error loading compute shader binary\n");
        assert(false);
    }
    if (ShaderFunction_FromFile(device, "DrawParticles.sfn", &renderShader) != 0)
    {
        printf("Error loading rendering shader binary\n");
        assert(false);
    }

    //shader state
    LGFXShaderStateCreateInfo stateCreateInfo = {0};
    stateCreateInfo.function = ShaderFunction_GetFunc(computeShader);
    computeShaderPipeline = LGFXCreateShaderState(device, &stateCreateInfo);

    stateCreateInfo.function = ShaderFunction_GetFunc(renderShader);
    stateCreateInfo.cullMode = LGFXCullMode_None;
    stateCreateInfo.dynamicLineWidth = false;
    stateCreateInfo.dynamicViewportScissor = true;
    stateCreateInfo.primitiveType = LGFXPrimitiveType_TriangleList;
    stateCreateInfo.blendState = LGFXBlendStateOpaque();
    stateCreateInfo.depthTest = false;
    stateCreateInfo.depthWrite = false;
    stateCreateInfo.vertexDeclarations = &particleAsVertexDecl;
    stateCreateInfo.vertexDeclarationCount = 1;
    stateCreateInfo.forRenderProgram = rp;
    stateCreateInfo.forRenderPass = 0;
    stateCreateInfo.entryPoint1Name = "VertexFunction";
    stateCreateInfo.entryPoint2Name = "FragmentFunction";
    renderShaderPipeline = LGFXCreateShaderState(device, &stateCreateInfo);

    computeShaderState = ShaderFunction_CreateUseState(computeShader);
    renderShaderState = ShaderFunction_CreateUseState(renderShader);
}
void Deinit()
{
    if (computeShaderState != NULL)
    {
        ShaderFunctionState_Deinit(computeShaderState);
        ShaderFunctionState_Deinit(renderShaderState);
    }

    if (computeShaderPipeline != NULL)
    {
        LGFXDestroyShaderState(computeShaderPipeline);
        LGFXDestroyShaderState(renderShaderPipeline);
    }

    if (computeShader != NULL)
    {
        ShaderFunction_Deinit(computeShader);
        ShaderFunction_Deinit(renderShader);
    }
    LGFXDestroyBuffer(indexBuffer);
    LGFXDestroyBuffer(inputBuffer);
    LGFXDestroyBuffer(outputBuffer);
    LGFXDestroyRenderProgram(rp);
}
void Update(float deltaTime)
{
    fpsTimer += deltaTime;
    if (fpsTimer >= 0.1f)
    {
        string title = StringFormat(GetCAllocator(), "Compute, Frames: %f\n", 1.0f / deltaTime);
        //string title = string(GetCAllocator(), "Compute, Frames: ");
        // title.Append(1.0f / deltaTime);
        // AstralCanvas::applicationInstance.windows.ptr[0]->SetWindowTitle(title);
        // title.deinit();
        Window_SetTitle(AstralCanvas_GetWindow(0), title.buffer);

        String_Deinit(&title);
        fpsTimer = 0.0f;
    }
}
void FixedUpdate(float deltaTime)
{

}
void Draw(float deltaTime, Window *currWindow)
{
    if (initializedSuccessfully)
    {
        LGFXCommandBuffer mainCmds = Window_GetMainCommandBuffer(currWindow);
        ContextData contextData = { PARTICLES_COUNT, deltaTime };

        ShaderFunctionState_SetComputeBuffer(computeShaderState, "ParticlesIn", inputBuffer);
        ShaderFunctionState_SetComputeBuffer(computeShaderState, "ParticlesOut", outputBuffer);
        ShaderFunctionState_SetUniform(computeShaderState, "Context", &contextData, sizeof(ContextData));
        ShaderFunctionState_SyncWithGPU(computeShaderState, mainCmds);

        //first run compute
        LGFXUseShaderState(mainCmds, computeShaderPipeline);
        LGFXDispatchCompute(mainCmds, PARTICLES_COUNT / 256, 1, 1);

        //then run draw
        LGFXAwaitWriteFunction(mainCmds, LGFXFunctionType_Compute, LGFXFunctionOperationType_VertexBufferRead);

        LGFXColor clearColor = {0, 0, 0, 255};
        LGFXBeginRenderProgramSwapchain(rp, mainCmds, Window_GetSwapchain(currWindow), clearColor, true);

        //view matrix (aka inverse of camera world position) is identity
        Matrix4x4 viewProjection = Matrix4x4_CreateOrthographic(80.0f, 45.0f, -1000.0f, 1000.0f);

        ShaderFunctionState_SetUniform(renderShaderState, "ViewProjection", &viewProjection, sizeof(Matrix4x4));
        ShaderFunctionState_SyncWithGPU(renderShaderState, mainCmds);

        Vec2 framebufferSize = Window_GetFramebufferSize(currWindow);

        LGFXBox viewportArea;
        viewportArea.X = 0.0f;
        viewportArea.Y = 0.0f;
        viewportArea.width = framebufferSize.X;
        viewportArea.height = framebufferSize.Y;

        LGFXRectangle clipArea;
        clipArea.X = 0;
        clipArea.Y = 0;
        clipArea.width = (uint32_t)framebufferSize.X;
        clipArea.height = (uint32_t)framebufferSize.Y;
        LGFXSetViewport(mainCmds, viewportArea);
        LGFXSetClipArea(mainCmds, clipArea);

        LGFXUseShaderState(mainCmds, renderShaderPipeline);
        LGFXUseVertexBuffer(mainCmds, &outputBuffer, 1);
        LGFXUseIndexBuffer(mainCmds, indexBuffer, 0);

        LGFXDrawIndexed(mainCmds, 6, PARTICLES_COUNT, 0, 0,0 );

        LGFXEndRenderProgram(rp, mainCmds);

        //swap buffers
        LGFXBuffer temp = outputBuffer;
        outputBuffer = inputBuffer;
        inputBuffer = temp;
    }
}
void PostDraw(float deltaTime)
{
    if (computeShaderState != NULL)
    {
        ShaderFunctionState_Clear(computeShaderState);
        ShaderFunctionState_Clear(renderShaderState);
    }
}

int32_t main()
{
    initializedSuccessfully = true;
    Application *application = AstralCanvas_Init("Compute", "Astral.Canvas", 0, 0, false);

    WindowCreationInfo windowInfo = {};
    windowInfo.width = 1280;
    windowInfo.height = 720;
    windowInfo.name = "Compute";
    windowInfo.presentMode = LGFXSwapchainPresentationMode_Fifo;
    windowInfo.resizeable = true;
    Window_Create(application, &windowInfo);

    AstralCanvas_Run(&Update, &FixedUpdate, &Draw, &PostDraw, &Init, &Deinit);
}