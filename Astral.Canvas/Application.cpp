#include "lgfx-astral/Application.hpp"
#include "array.hpp"
#include "string.hpp"
#include "lgfx-astral/Input.hpp"
#include "GLFW/glfw3.h"
#include "lgfx/lgfx.h"
#include "lgfx/lgfx-glfw.h"
#include "lgfx-astral/Shader.hpp"

using namespace collections;

namespace AstralCanvas
{
	Application applicationInstance;
	float refreshTimer = 0.0f;

	double GetElapsedTime()
	{
		return glfwGetTime();
	}
	Application::Application()
	{
		windows = collections::vector<Window *>();
		currentWindow = NULL;
		allocator = IAllocator{};
		windowsArena = ArenaAllocator();

		instance = NULL;
		device = NULL;

		appName = string();
		engineName = string();
		appVersion = 0;
		engineVersion = 0;
		startTime = 0.0f;
		endTime = 0.0f;
		updateTimer = 0.0f;
		fixedUpdateTimer = 0.0f;
		shouldResetDeltaTimer = 0.0f;

		framesPerSecond = 0.0f;
		fixedTimeStep = 0.0f;
		timeScale = 1.0f;
	}
	void ApplicationInit(IAllocator allocator, string appName, string engineName, u32 appVersion, u32 engineVersion, float framesPerSecond, bool noWindow)
	{
		Application result = {};
		result.framesPerSecond = framesPerSecond;
		result.allocator = allocator;
		result.windows = vector<Window *>(allocator);
		result.appName = appName;
		result.engineName = engineName;
		result.appVersion = appVersion;
		result.engineVersion = engineVersion;
		result.timeScale = 1.0f;
		result.fixedTimeStep = 0.02f;
		result.windowsArena = ArenaAllocator(allocator);
		result.shouldShutdown = false;
		applicationInstance = result;

		if (!noWindow)
		{
			#ifdef X11
			glfwInitHint(GLFW_X11_XCB_VULKAN_SURFACE, GLFW_FALSE);
			#endif
			glfwInit();
			u32 extensionsCount;
			const char **extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

			LGFXInstanceCreateInfo instanceCreateInfo = {0};
			instanceCreateInfo.appName = appName.buffer;
			instanceCreateInfo.appVersion = appVersion;
			instanceCreateInfo.engineName = engineName.buffer;
			instanceCreateInfo.engineVersion = engineVersion;
	#if DEBUG
			instanceCreateInfo.runtimeErrorChecking = true;
	#else
			instanceCreateInfo.runtimeErrorChecking = false;
	#endif
			instanceCreateInfo.backend = LGFXBackendType_Vulkan;
			instanceCreateInfo.enabledExtensionsCount = extensionsCount;
			instanceCreateInfo.enabledExtensions = extensions;

			applicationInstance.instance = LGFXCreateInstance(&instanceCreateInfo);

			//create device
			LGFXDeviceCreateInfo deviceCreateInfo = {0};
			deviceCreateInfo.requiredFeatures.fillModeNonSolid = true;
			//deviceCreateInfo.requiredFeatures.wideLines = true;
			applicationInstance.device = LGFXCreateDevice(applicationInstance.instance, &deviceCreateInfo);
		}
	}
	bool Application::AddWindow(const char *name, i32 width, i32 height, bool resizeable, bool fullscreen, bool maximized, void *iconData, u32 iconWidth, u32 iconHeight, LGFXSwapchainPresentationMode presentMode)
	{
		Window *result = (Window *)this->windowsArena.AsAllocator().Allocate(sizeof(Window));
		if (WindowInit(this->windowsArena.AsAllocator(), name, result, width, height, resizeable, maximized, fullscreen, iconData, iconWidth, iconHeight, presentMode))
		{
			if (framesPerSecond <= -1.0f)
			{
				framesPerSecond = (float)result->GetCurrentMonitorFramerate();
				//glfwsetwindow((GLFWwindow *)result.handle, GLFW_REFRESH_RATE, );
			}
			windows.Add(result);
			glfwSetWindowUserPointer((GLFWwindow*)result->handle, windows.ptr[windows.count - 1]);

			return true;
		}
		return false;
	}
	void Application::ResetDeltaTimer()
	{
		glfwSetTime(0.0);
		shouldResetDeltaTimer = true;
	}
	void Application::Run(ApplicationUpdateFunction updateFunc, ApplicationUpdateFunction fixedUpdateFunc, ApplicationDrawFunction drawFunc, ApplicationUpdateFunction postEndDrawFunc, ApplicationInitFunction initFunc, ApplicationDeinitFunction deinitFunc)
	{
		currentWindow = windows.count > 0 ? windows.ptr[0] : NULL;
		if (initFunc != NULL)
		{
			initFunc();
		}
		startTime = (float)glfwGetTime();
		endTime = startTime;
		bool noWindows = windows.count == 0;
		if (noWindows)
		{
			alwaysUpdate = true;
		}

		bool shouldStop = false;
		while (!shouldStop)
		{
			float deltaTime = endTime - startTime;

			glfwPollEvents();
			updateTimer += deltaTime;
			fixedUpdateTimer += deltaTime;

			bool runUpdate = framesPerSecond < 1.0f || updateTimer >= 1.0f / framesPerSecond;
			bool minimized = windows.count > 0;
			for (u32 i = 0; i < windows.count; i++)
			{
				if (windows[i]->resolution.X > 0 && windows[i]->resolution.Y > 0)
				{
					minimized = false;
					break;
				}
			}
			if (minimized)
			{
				glfwWaitEvents();
				endTime = (float)glfwGetTime();
			}

			//fixed update
			if (fixedUpdateTimer > fixedTimeStep * 4.0f) //cap to avoid death spiral
			{
				fixedUpdateTimer = fixedTimeStep * 4.0f;
			}
			while (fixedUpdateTimer > fixedTimeStep)
			{
				if (!minimized || alwaysUpdate)
				{
					fixedUpdateFunc(fixedTimeStep);
				}
				fixedUpdateTimer -= fixedTimeStep;
			}

			for (i32 i = (i32)windows.count - 1; i >= 0; i--)
			{
				Window &window = *windows.ptr[i];
				
				if (window.resolution.X == 0 || window.resolution.Y == 0 || window.frameBufferSize.X == 0 || window.frameBufferSize.Y == 0)
				{
					continue;
				}
				currentWindow = windows.ptr[i];
				if (runUpdate || alwaysUpdate)
				{
					updateFunc(updateTimer * this->timeScale);
				}
				if (window.handle != NULL && !glfwWindowShouldClose((GLFWwindow*)window.handle))
				{
					window.windowInputState.ResetPerFrameInputStates();
				}
				else
				{
					window.deinit();
					windows.RemoveAt_Swap(i);
					continue;
				}

				//begin draw
				if (LGFXNewFrame(device, &window.swapchain, (u32)window.frameBufferSize.X, (u32)window.frameBufferSize.Y))
				{
					LGFXCommandBufferReset(window.mainCommandBuffer);
					LGFXCommandBufferBegin(window.mainCommandBuffer, true);

					drawFunc(deltaTime, &window);

					//end draw
					LGFXCommandBufferEndSwapchain(window.mainCommandBuffer, window.swapchain);
					LGFXSubmitFrame(device, window.swapchain);

					if (postEndDrawFunc != NULL)
					{
						postEndDrawFunc(deltaTime);
					}

					for (u32 i = 0; i < AstralCanvas::allUsedShaders.count; i++)
					{
						AstralCanvas::allUsedShaders.ptr[i]->descriptorForThisDrawCall = 0;
					}
					AstralCanvas::allUsedShaders.Clear();
				}
			}

			if (windows.count == 0 && !noWindows)
			{
				break;
			}
			updateTimer = 0.0f;
			

			startTime = endTime;
			endTime = (float)glfwGetTime();
			if (this->shouldResetDeltaTimer)
			{
				startTime = endTime;
				this->shouldResetDeltaTimer = false;
			}
			if (shouldShutdown)
			{
				shouldStop = true;
				for (usize i = 0; i < windows.count; i++)
				{
					glfwSetWindowShouldClose((GLFWwindow *)windows[i]->handle, 1);
				}
			}
		}

		//await rendering process shutdown

		if (deinitFunc != NULL)
		{
			deinitFunc();
		}
		windowsArena.deinit();

		//deinitialize backend
		if (device != NULL)
		{
			LGFXDestroyDevice(device);
		}
		if (instance != NULL)
		{
			LGFXDestroyInstance(instance);
		}
	}
	const char *GetClipboardText()
	{
		return glfwGetClipboardString(NULL);
	}
	void SetClipboardText(const char *text)
	{
		glfwSetClipboardString(NULL, text);
	}
}
