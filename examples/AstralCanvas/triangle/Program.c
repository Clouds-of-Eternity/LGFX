#include "AstralCanvas/Application.h"
#include "AstralCanvas/ShaderFunction.h"

bool initializedSuccessfully;

typedef struct VertexPositionColor
{
    Vec4 position;
    Vec4 color;
} VertexPositionColor;

LGFXRenderProgram rp;
LGFXBuffer vertexBuffer;
LGFXBuffer indexBuffer;
LGFXVertexDeclaration vertexDecl;
ShaderFunction shader;
LGFXShaderState shaderState;

inline static LGFXVertexDeclaration GetVertexPositionColorDecl()
{
    LGFXVertexElementFormat formats[2] = {
        LGFXVertexElementFormat_Vector4,
        LGFXVertexElementFormat_Vector4
    };
    return LGFXCreateVertexDeclaration(formats, 2, false, true);
}

void Init()
{
    LGFXDevice device = AstralCanvas_GetGraphicsDevice();

    //render program
    LGFXRenderAttachmentInfo attachments = {};
    attachments.clear = true;
    attachments.format = LGFXTextureFormat_BGRA8Unorm;
    attachments.outputType = LGFXRenderAttachmentOutput_ToScreen;
    attachments.samples = 1;

    int32_t firstAttachment = 0;

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
    rpCreateInfo.maxBackbufferTexturesCount = LGFXSwapchainGetBackbufferTexturesCount(AstralCanvas_GetWindowSwapchain(0));
    rp = LGFXCreateRenderProgram(device, &rpCreateInfo);

    //vertex buffer
    LGFXBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_VertexBuffer | LGFXBufferUsage_TransferDest);
    bufferCreateInfo.memoryUsage = LGFXMemoryUsage_GPU_ONLY;
    bufferCreateInfo.size = sizeof(VertexPositionColor) * 3;
    vertexBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);

    //index buffer
    bufferCreateInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_IndexBuffer | LGFXBufferUsage_TransferDest);
    bufferCreateInfo.size = sizeof(uint32_t) * 3;
    indexBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);

    vertexDecl = GetVertexPositionColorDecl();

    //vertex data
    VertexPositionColor vertices[3] = {
        {CreateVec4(0.0, -1.0, 0.0, 1.0), CreateVec4(1.0, 0.0, 0.0, 1.0)},
        {CreateVec4(-1.0, 1.0, 0.0, 1.0), CreateVec4(0.0, 1.0, 0.0, 1.0)},
        {CreateVec4(1.0, 1.0, 0.0, 1.0), CreateVec4(0.0, 0.0, 1.0, 1.0)}
    };
    LGFXSetBufferDataOptimizedData(vertexBuffer, NULL, (uint8_t *)vertices, sizeof(vertices));

    uint32_t indices[3] = {
        0, 1, 2
    };
    LGFXSetBufferDataOptimizedData(indexBuffer, NULL, (uint8_t *)indices, sizeof(indices));

    //shader
    size_t errorCode = ShaderFunction_FromFile(device, "Triangle.sfn", &shader);
    if (errorCode != 0)
    {
        printf("Error loading shader binary\n");
        initializedSuccessfully = false;
    }

    //shader state
    LGFXShaderStateCreateInfo stateCreateInfo = {0};
    stateCreateInfo.blendState = LGFXBlendStateNonPremultiplied();
    stateCreateInfo.cullMode = LGFXCullMode_None;
    stateCreateInfo.depthTest = false;
    stateCreateInfo.depthWrite = false;
    stateCreateInfo.primitiveType = LGFXPrimitiveType_TriangleList;
    stateCreateInfo.dynamicViewportScissor = true;
    stateCreateInfo.function = ShaderFunction_GetFunc(shader);
    stateCreateInfo.vertexDeclarationCount = 1;
    stateCreateInfo.vertexDeclarations = &vertexDecl;
    stateCreateInfo.forRenderProgram = rp;
    stateCreateInfo.forRenderPass = 0;
    stateCreateInfo.entryPoint1Name = "VertexFunction";
    stateCreateInfo.entryPoint2Name = "FragmentFunction";
    shaderState = LGFXCreateShaderState(device, &stateCreateInfo);
}
void Deinit()
{
    LGFXDestroyShaderState(shaderState);
    ShaderFunction_Deinit(shader);
    LGFXDestroyBuffer(vertexBuffer);
    LGFXDestroyBuffer(indexBuffer);
    LGFXDestroyRenderProgram(rp);
}
void Update(float deltaTime)
{

}
void FixedUpdate(float deltaTime)
{

}
void Draw(float deltaTime, Window *currWindow)
{
    if (initializedSuccessfully)
    {
        LGFXCommandBuffer commandBuffer = Window_GetMainCommandBuffer(currWindow);
        LGFXSwapchain swapchain = Window_GetSwapchain(currWindow);

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
        LGFXSetViewport(commandBuffer, viewportArea);
        LGFXSetClipArea(commandBuffer, clipArea);

        LGFXColor clearColor = {128, 128, 128, 255};
        LGFXBeginRenderProgramSwapchain(rp, commandBuffer, swapchain, clearColor, true);

        LGFXUseVertexBuffer(commandBuffer, &vertexBuffer, 1);
        LGFXUseIndexBuffer(commandBuffer, indexBuffer, 0);
        LGFXUseShaderState(commandBuffer, shaderState);

        LGFXDrawIndexed(commandBuffer, 3, 1, 0, 0, 0);

        LGFXEndRenderProgram(rp, commandBuffer);
    }
}
void PostDraw(float deltaTime)
{

}
int32_t main()
{
    initializedSuccessfully = true;
    Application *application = AstralCanvas_Init("Triangle", "Astral.Canvas", 0, 0, false);

    WindowCreationInfo windowInfo = {};
    windowInfo.width = 640;
    windowInfo.height = 480;
    windowInfo.name = "Triangle";
    windowInfo.presentMode = LGFXSwapchainPresentationMode_Fifo;
    windowInfo.resizeable = true;
    Window_Create(application, &windowInfo);

    AstralCanvas_Run(&Update, &FixedUpdate, &Draw, &PostDraw, &Init, &Deinit);
}