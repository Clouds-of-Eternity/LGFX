#include "Linxc.h"
#include "lgfx-astral/lgfx-astral-types.hpp"
#include "lgfx-astral/Application.hpp"
#include "lgfx-astral/Shader.hpp"
#include "random.hpp"
#include "lgfx-astral/Input.hpp"
#include "ModelLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define POINT_COUNT 15000

LGFXRenderProgram rp;

LGFXBuffer indexBuffer;
LGFXTexture albedoMap;
LGFXTexture heightMap;
LGFXSamplerState samplerState;
LGFXSamplerState textureSamplerState;
Model teapot;

AstralCanvas::Shader loadShader;
LGFXShaderState loadShaderState;
LGFXBuffer transformedBuffer;
LGFXBuffer indirectBuffer;

AstralCanvas::Shader renderShader;
LGFXShaderState renderShaderState;

AstralCanvas::Shader renderMeshShader;
LGFXShaderState renderMeshShaderState;
LGFXVertexDeclaration vertDecl;

float fpsTimer = 0.0f;

// flycam
Maths::Vec3 cameraPosition;
float yaw;
float pitch;
bool dragStart;
Maths::Vec2 mouseAtDragStart;
Maths::Matrix4x4 rotationMatrix;

struct DrawIndexedIndirectCommand
{
    u32 indexCount;
    u32 instanceCount;
    u32 firstIndex;
    i32 vertexOffset;
    u32 firstInstance;
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
    AstralCanvas::Window *win = AstralCanvas::Input_GetWindow();
    Maths::Vec2 mousePosition = win->windowInputState.mousePosition;
    if (AstralCanvas::Input_IsMousePressed(AstralCanvas::MouseButton_Middle))
    {
        mouseAtDragStart = mousePosition;
        dragStart = true;
        win->SetMouseState(AstralCanvas::WindowMouseState_Hidden);
    }
    if (dragStart)
    {
        Maths::Vec2 diff = mousePosition - mouseAtDragStart;
        mouseAtDragStart = mousePosition;

        yaw += diff.X * Degree2Radian * 0.15f;
        pitch += diff.Y * Degree2Radian * 0.15f;
        if (yaw < 0.0f)
        {
            yaw += 360.0f * Degree2Radian;
        }
        else if (yaw > 360.0f * Degree2Radian)
        {
            yaw -= 360.0f * Degree2Radian;
        }
        if (pitch < -89.0f * Degree2Radian)
        {
            pitch = -89.0f * Degree2Radian;
        }
        if (pitch > 89.0f * Degree2Radian)
        {
            pitch = 89.0f * Degree2Radian;
        }

        if (AstralCanvas::Input_IsMouseReleased(AstralCanvas::MouseButton_Middle))
        {
            win->SetMouseState(AstralCanvas::WindowMouseState_Default);
            dragStart = false;
        }
        Maths::Quaternion quat = Maths::Quaternion::FromYawPitchRoll(yaw, pitch, 0.0f);
        rotationMatrix = Maths::Matrix4x4::CreateFromQuaternion(quat);

        float xAxis = (AstralCanvas::Input_IsKeyDown(AstralCanvas::Keys_A) ? -1.0f : 0.0f) + (AstralCanvas::Input_IsKeyDown(AstralCanvas::Keys_D) ? 1.0f : 0.0f);
        float zAxis = (AstralCanvas::Input_IsKeyDown(AstralCanvas::Keys_S) ? -1.0f : 0.0f) + (AstralCanvas::Input_IsKeyDown(AstralCanvas::Keys_W) ? 1.0f : 0.0f);
        float yAxis = (AstralCanvas::Input_IsKeyDown(AstralCanvas::Keys_Space) ? -1.0f : 0.0f) + (AstralCanvas::Input_IsKeyDown(AstralCanvas::Keys_ShiftLeft) ? 1.0f : 0.0f);

        float speedMult = 1.0f;
        Maths::Vec3 forward = rotationMatrix.Transform(Maths::Vec3(0.0f, 0.0f, 1.0f));
        forward.Y = 0.0f;
        forward = forward.Normalized();

        cameraPosition += forward * zAxis * deltaTime * speedMult;
        forward.Y = 0.0f;
        forward.Normalize();

        Maths::Vec3 right = Maths::Vec3::Cross(forward, Maths::Vec3(0.0f, -1.0f, 0.0f));
        Maths::Vec3 up = -Maths::Vec3::Cross(forward, right);

        cameraPosition += right * xAxis * deltaTime * speedMult;
        cameraPosition += up * yAxis * deltaTime * speedMult;
        // pitch = Clamp(data.pitch, -89f * Mathf.DegreeRadian, 89f * Mathf.DegreeRadian);
    }
}
struct ShaderGlobalData
{
    Maths::Matrix4x4 projection;
    Maths::Matrix4x4 view;
    Maths::Matrix4x4 projectionInverse;
    Maths::Matrix4x4 viewInverse;
    Maths::Vec4 minExtents;
    Maths::Vec4 maxExtents;
};
void Draw(float deltaTime, AstralCanvas::Window *window)
{
    LGFXCommandBuffer mainCmds = window->mainCommandBuffer;

    ShaderGlobalData globalData = {};
    globalData.projection = Maths::Matrix4x4::CreatePerspectiveFOV(70.0f * Degree2Radian, 16.0f / 9.0f, 0.1f, 1000.0f);
    globalData.view = Maths::Matrix4x4::CreateLookAt(cameraPosition, rotationMatrix.Transform(Maths::Vec3(0.0f, 0.0f, 1.0f)), Maths::Vec3(0.0f, -1.0f, 0.0f));
    globalData.projectionInverse = globalData.projection.Invert().value;
    globalData.viewInverse = globalData.view.Invert().value;
    globalData.minExtents = Maths::Vec4(teapot.minBounds, 0.0f);
    globalData.maxExtents = Maths::Vec4(teapot.maxBounds, 0.0f);

    // loadShader.SetShaderVariable("ShaderGlobalData", &globalData, sizeof(ShaderGlobalData));
    // loadShader.SetShaderVariableComputeBuffer("Input", teapot.pcBuffer);
    // loadShader.SetShaderVariableComputeBuffer("Output", transformedBuffer);
    // loadShader.SetShaderVariableComputeBuffer("DrawCommand", indirectBuffer);
    // loadShader.SyncUniformsWithGPU(mainCmds);
    // LGFXUseShaderState(mainCmds, loadShaderState);
    // LGFXDispatchCompute(mainCmds, (u32)ceilf(POINT_COUNT / 256.0f), 1, 1);
    // LGFXAwaitWriteFunction(mainCmds, LGFXFunctionType_Compute, LGFXFunctionOperationType_IndirectBufferRead);

    LGFXBeginRenderProgramSwapchain(rp, mainCmds, window->swapchain, {0, 0, 0, 255}, true);

    LGFXSetViewport(mainCmds, {0, 0, (float)window->resolution.X, (float)window->resolution.Y});
    LGFXSetClipArea(mainCmds, {0, 0, (u32)window->resolution.X, (u32)window->resolution.Y});

    #if !DRAW_SDFs
    renderShader.SetShaderVariable("ShaderGlobalData", &globalData, sizeof(ShaderGlobalData));
    renderShader.SetShaderVariableSampler("samplerState", samplerState);
    renderShader.SetShaderVariableSampler("textureSamplerState", textureSamplerState);
    renderShader.SetShaderVariableTexture("meshDistanceField", teapot.meshSDFTexture);
    renderShader.SetShaderVariableTexture("albedoMap", albedoMap);
    renderShader.SetShaderVariableTexture("heightMap", heightMap);
    renderShader.SyncUniformsWithGPU(mainCmds);

    LGFXUseShaderState(mainCmds, renderShaderState);
    LGFXUseIndexBuffer(mainCmds, indexBuffer, 0);
    LGFXDrawIndexed(mainCmds, 6, 1, 0, 0, 0);
    #else
    renderShader.SetShaderVariable("ShaderGlobalData", &globalData, sizeof(ShaderGlobalData));
    renderShader.SyncUniformsWithGPU(mainCmds);

    LGFXUseShaderState(mainCmds, renderMeshShaderState);
    LGFXUseVertexBuffer(mainCmds, &teapot.vertexBuffer, 1);
    LGFXUseIndexBuffer(mainCmds, teapot.indexBuffer, 0);
    LGFXDrawIndexed(mainCmds, teapot.indices.length, 1, 0, 0, 0);
    #endif

    LGFXEndRenderProgram(rp, mainCmds);
}
void PostEndDraw(float deltaTime)
{
}
LGFXTexture FastLoadTexture(LGFXDevice device, const char *path)
{
    i32 w = 0;
    i32 h = 0;
    i32 comp = 0;
    stbi_uc *bytes = stbi_load(path, &w, &h, &comp, 4);
    LGFXTextureCreateInfo textureCreateInfo = {};
    textureCreateInfo.depth = 1;
    textureCreateInfo.format = LGFXTextureFormat_RGBA8Unorm;
    textureCreateInfo.width = w;
    textureCreateInfo.height = h;
    textureCreateInfo.mipLevels = 1;
    textureCreateInfo.sampleCount = 1;
    textureCreateInfo.usage = (LGFXTextureUsage)(LGFXTextureUsage_Sampled | LGFXTextureUsage_TransferDest);
    LGFXTexture result = LGFXCreateTexture(device, &textureCreateInfo);

    LGFXTextureSetData(device, result, bytes, w * h * 4);

    stbi_image_free(bytes);
    return result;
}
void Init()
{
    rotationMatrix = Maths::Matrix4x4::Identity();
    cameraPosition = Maths::Vec3(0.0f, 0.0f, -5.0f);
    LGFXDevice device = AstralCanvas::applicationInstance.device;

    // render program
    LGFXRenderAttachmentInfo attachments[2];
    attachments[0].clear = true;
    attachments[0].format = LGFXTextureFormat_BGRA8Unorm;
    attachments[0].readByRenderTarget = false;

    attachments[1].clear = true;
    attachments[1].format = LGFXTextureFormat_Depth32Float;
    attachments[1].readByRenderTarget = false;

    i32 firstAttachment = 0;

    LGFXRenderPassInfo passes;
    passes.colorAttachmentIDs = &firstAttachment;
    passes.colorAttachmentsCount = 1;
    passes.depthAttachmentID = 1;
    passes.readAttachmentIDs = NULL;
    passes.readAttachmentsCount = 0;

    LGFXRenderProgramCreateInfo rpCreateInfo;
    rpCreateInfo.attachmentsCount = 2;
    rpCreateInfo.attachments = attachments;
    rpCreateInfo.renderPassCount = 1;
    rpCreateInfo.renderPasses = &passes;
    rpCreateInfo.outputToBackbuffer = true;
    rp = LGFXCreateRenderProgram(device, &rpCreateInfo);

    LGFXVertexElementFormat vertexElems[2] = {
        LGFXVertexElementFormat_Vector4,
        LGFXVertexElementFormat_Vector4
    };
    vertDecl = LGFXCreateVertexDeclaration(vertexElems, 2, true, true);

    // shader
    string fileContents = io::ReadFile(GetCAllocator(), "DrawSDFs.shaderobj", false);
    if (AstralCanvas::CreateShaderFromString(device, GetCAllocator(), fileContents, &renderShader) != 0)
    {
        printf("Error loading SDFs shader json\n");
    }
    fileContents.deinit();

    fileContents = io::ReadFile(GetCAllocator(), "CreateMeshSDF.shaderobj", false);
    if (AstralCanvas::CreateShaderFromString(device, GetCAllocator(), fileContents, &loadShader) != 0)
    {
        printf("Error loading Splat Transformation shader json\n");
    }
    fileContents.deinit();

    // index buffer
    LGFXBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_IndexBuffer | LGFXBufferUsage_TransferDest);
    bufferCreateInfo.memoryUsage = LGFXMemoryUsage_GPU_ONLY;
    bufferCreateInfo.size = sizeof(u32) * 6;
    indexBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);

    u32 indices[] = {0, 1, 2, 3, 0, 2};
    LGFXSetBufferDataOptimizedData(indexBuffer, NULL, (u8 *)indices, 6 * sizeof(u32));

    // shader state
    LGFXShaderStateCreateInfo stateCreateInfo = {};
    stateCreateInfo.function = renderShader.gpuFunction;
    stateCreateInfo.cullMode = LGFXCullMode_None;
    stateCreateInfo.dynamicLineWidth = false;
    stateCreateInfo.dynamicViewportScissor = true;
    stateCreateInfo.primitiveType = LGFXPrimitiveType_TriangleList;
    stateCreateInfo.blendState = DISABLE_BLEND;
    stateCreateInfo.depthTest = false;
    stateCreateInfo.depthWrite = false;
    //stateCreateInfo.vertexDeclarations = &vertDecl;
    stateCreateInfo.vertexDeclarationCount = 0;
    stateCreateInfo.forRenderProgram = rp;
    stateCreateInfo.forRenderPass = 0;
    renderShaderState = LGFXCreateShaderState(device, &stateCreateInfo);

    stateCreateInfo = {};
    stateCreateInfo.function = loadShader.gpuFunction;
    loadShaderState = LGFXCreateShaderState(device, &stateCreateInfo);

    // sampler state
    LGFXSamplerStateCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.repeatModeU = LGFXSamplerRepeatMode_ClampToEdgeColor;
    samplerCreateInfo.repeatModeV = LGFXSamplerRepeatMode_ClampToEdgeColor;
    samplerCreateInfo.repeatModeW = LGFXSamplerRepeatMode_ClampToEdgeColor;
    samplerCreateInfo.minimizationFilter = LGFXFilterType_Linear;
    samplerCreateInfo.magnificationFilter = samplerCreateInfo.minimizationFilter;
    samplerState = LGFXCreateSamplerState(device, &samplerCreateInfo);

    samplerCreateInfo = {};
    samplerCreateInfo.repeatModeU = LGFXSamplerRepeatMode_Repeat;
    samplerCreateInfo.repeatModeV = LGFXSamplerRepeatMode_Repeat;
    samplerCreateInfo.repeatModeW = LGFXSamplerRepeatMode_Repeat;
    samplerCreateInfo.minimizationFilter = LGFXFilterType_Linear;
    samplerCreateInfo.magnificationFilter = samplerCreateInfo.minimizationFilter;
    textureSamplerState = LGFXCreateSamplerState(device, &samplerCreateInfo);

    // texture
    albedoMap = FastLoadTexture(device, "GrassAlbedo.jpg");
    //normalMap = FastLoadTexture(device, "GrassNormal.jpg");
    heightMap = FastLoadTexture(device, "GrassDisplacement.jpg");

    //model
    teapot = LoadModel("bunny.obj", &loadShader, loadShaderState);
}
void Deinit()
{
    // LGFXDestroyShaderState(computeShaderState);
    // computeShader.deinit();
    // LGFXDestroyBuffer(indirectBuffer);
    LGFXDestroyBuffer(indexBuffer);
    LGFXDestroyShaderState(renderShaderState);
    renderShader.deinit();
    loadShader.deinit();
    LGFXDestroyShaderState(loadShaderState);
    LGFXDestroyRenderProgram(rp);
    LGFXDestroySamplerState(samplerState);
    LGFXDestroySamplerState(textureSamplerState);
    LGFXDestroyTexture(heightMap);
    LGFXDestroyTexture(albedoMap);
    // LGFXDestroyBuffer(transformedBuffer);
    teapot.deinit();
}

void FixedUpdate(float deltaTime)
{
}

i32 main()
{
    AstralCanvas::ApplicationInit(
        GetCAllocator(),
        string(GetCAllocator(), "SDFs"),
        string(GetCAllocator(), "Astral.Canvas"),
        0, 0, 165.0f);

    AstralCanvas::applicationInstance.AddWindow("SDFs", 1920, 1080);
    AstralCanvas::applicationInstance.Run(&Update, &FixedUpdate, &Draw, &PostEndDraw, &Init, &Deinit);
}