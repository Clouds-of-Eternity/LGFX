#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "Linxc.h"
#include "stdio.h"
#include "io.hpp"
#include "lgfx/lgfx.h"
#include "lgfx/lgfx-glfw.h"
#include "lgfx-astral/lgfx-astral-types.hpp"
#include "lgfx-astral/Shader.hpp"

LGFXInstance instance;
LGFXDevice device;
LGFXSwapchain swapchain;
LGFXCommandBuffer mainCommands;
LGFXRenderProgram rp;

LGFXVertexDeclaration vertexDecl;
LGFXBuffer vertexBuffer;
LGFXBuffer indexBuffer;

AstralCanvas::Shader shader;
LGFXShaderState shaderState;

void WindowFramebufferResized(GLFWwindow *window, i32 width, i32 height)
{
    if (swapchain != NULL)
    {
        LGFXSwapchainInvalidate(swapchain);
    }
}

i32 main()
{
    glfwInitHint(GLFW_X11_XCB_VULKAN_SURFACE, GLFW_FALSE);
    glfwInit();

    //create window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(640, 480, "LGFX Triangle!", NULL, NULL);

    //create instance
    u32 extensionsCount;
    const char **extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

    LGFXInstanceCreateInfo instanceCreateInfo = {0};
    instanceCreateInfo.appName = "LGFX Triangle";
    instanceCreateInfo.appVersion = 0;
    instanceCreateInfo.engineName = "None";
    instanceCreateInfo.engineVersion = 0;
    #ifdef DEBUG
    instanceCreateInfo.runtimeErrorChecking = true;
    #endif
    instanceCreateInfo.backend = LGFXBackendType_Vulkan;
    instanceCreateInfo.enabledExtensionsCount = extensionsCount;
    instanceCreateInfo.enabledExtensions = extensions;

    instance = LGFXCreateInstance(&instanceCreateInfo);

    //create device
    LGFXDeviceCreateInfo deviceCreateInfo = {0};
    device = LGFXCreateDevice(instance, &deviceCreateInfo);

    //create swapchain
    LGFXSwapchainCreateInfo swapchainCreateInfo = {0};
    swapchainCreateInfo.oldSwapchain = NULL;
    swapchainCreateInfo.presentationMode = LGFXSwapchainPresentationMode_Mailbox;
    int w;
    int h;
    glfwGetFramebufferSize(window, &w, &h);
    swapchainCreateInfo.width = (u32)w;
    swapchainCreateInfo.height = (u32)h;
    swapchainCreateInfo.nativeWindowHandle = LGFXGetNativeWindowHandle(window);
    swapchainCreateInfo.displayHandle = LGFXGetNativeWindowDisplay();

    swapchain = LGFXCreateSwapchain(device, &swapchainCreateInfo);

    //create main command buffer to reuse
    mainCommands = LGFXCreateCommandBuffer(device, false);

    //create resource buffers

    LGFXBufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_VertexBuffer | LGFXBufferUsage_TransferDest);
    bufferCreateInfo.memoryUsage = LGFXMemoryUsage_GPU_ONLY;
    bufferCreateInfo.size = sizeof(LGFX::VertexPositionColor) * 3;
    vertexBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);

    bufferCreateInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_IndexBuffer | LGFXBufferUsage_TransferDest);
    bufferCreateInfo.size = sizeof(u32) * 3;
    indexBuffer = LGFXCreateBuffer(device, &bufferCreateInfo);

    vertexDecl = LGFX::GetVertexPositionColorDecl();

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
    rpCreateInfo.maxBackbufferTexturesCount = LGFXSwapchainGetBackbufferTexturesCount(swapchain);
    rp = LGFXCreateRenderProgram(device, &rpCreateInfo);

    //shadah
    string fileContents = io::ReadFile(GetCAllocator(), "Triangle.shaderobj", false);
    if (AstralCanvas::CreateShaderFromString(device, GetCAllocator(), fileContents, &shader) != 0)
    {
        printf("Error loading shader json\n");
    }
    fileContents.deinit();

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

    glfwSetFramebufferSizeCallback(window, &WindowFramebufferResized);

    //main loop
    while (!glfwWindowShouldClose(window)) 
    {
        i32 framebufferWidth;
        i32 framebufferHeight;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        while (framebufferWidth == 0 || framebufferHeight == 0)
        {
            glfwPollEvents();
            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        }
        if (LGFXNewFrame(device, &swapchain, (u32)framebufferWidth, (u32)framebufferHeight))
        {
            LGFXCommandBufferReset(mainCommands);
            LGFXCommandBufferBegin(mainCommands, true);

            LGFXSetViewport(mainCommands, {0, 0, (float)framebufferWidth, (float)framebufferHeight});
            LGFXSetClipArea(mainCommands, {0, 0, (u32)framebufferWidth, (u32)framebufferHeight});

            LGFXBeginRenderProgramSwapchain(rp, mainCommands, swapchain, {128, 128, 128, 255}, true);

            LGFXUseVertexBuffer(mainCommands, &vertexBuffer, 1);
            LGFXUseIndexBuffer(mainCommands, indexBuffer, 0);
            LGFXUseShaderState(mainCommands, shaderState);

            LGFXDrawIndexed(mainCommands, 3, 1, 0, 0, 0);

            LGFXEndRenderProgram(rp, mainCommands);

            LGFXCommandBufferEndSwapchain(mainCommands, swapchain);
            LGFXSubmitFrame(device, swapchain);
        }

        //LGFXSubmitFrame(device, &swapchain, (u32)framebufferWidth, (u32)framebufferHeight);
        glfwPollEvents();
    }
    LGFXAwaitSwapchainIdle(swapchain);

    glfwDestroyWindow(window);

    //shutdown
    LGFXDestroyShaderState(shaderState);

    shader.deinit();

    LGFXDestroyCommandBuffer(mainCommands);
    LGFXDestroyBuffer(vertexBuffer);
    LGFXDestroyBuffer(indexBuffer);

    LGFXDestroyRenderProgram(rp);

    LGFXDestroySwapchain(swapchain, true);

    LGFXDestroyDevice(device);

    LGFXDestroyInstance(instance);
}