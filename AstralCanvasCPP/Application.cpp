#include "AstralCanvasHPP/Application.hpp"
#include "Array.hpp"
#include "string.hpp"
#include "AstralCanvasHPP/Input.hpp"
#include "GLFW/glfw3.h"
#include "lgfx/lgfx.h"
#include "lgfx/lgfx-glfw.h"
#include "AstralCanvasHPP/ShaderFunction.hpp"

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
		windows = collections::List<Window *>();
		currentWindow = NULL;
		allocator = IAllocator{};

		instance = NULL;
		device = NULL;

		appName = string();
		engineName = string();
		appVersion = 0;
		engineVersion = 0;
		startTime = 0.0f;
		endTime = 0.0f;
		fixedUpdateTimer = 0.0f;
		shouldResetDeltaTimer = 0.0f;

		framesPerSecond = 0.0f;
		fixedTimeStep = 0.0f;
		timeScale = 1.0f;
	}
	Application::Application(IAllocator allocator, text appName, text engineName, u32 appVersion, u32 engineVersion, float framesPerSecond, bool noWindow)
	{
		this->framesPerSecond = framesPerSecond;
		this->allocator = allocator;
		this->windows = collections::List<Window *>(allocator);
		this->appName = string(allocator, appName);
		this->engineName = string(allocator, engineName);
		this->appVersion = appVersion;
		this->engineVersion = engineVersion;
		this->timeScale = 1.0f;
		this->fixedTimeStep = 0.02f;
		this->shouldShutdown = false;

		if (!noWindow)
		{
			#ifdef X11
			glfwInitHint(GLFW_X11_XCB_VULKAN_SURFACE, GLFW_FALSE);
			#endif
			glfwInit();
			u32 extensionsCount;
			const char **extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

			LGFXInstanceCreateInfo instanceCreateInfo = {0};
			instanceCreateInfo.appName = appName;
			instanceCreateInfo.appVersion = appVersion;
			instanceCreateInfo.engineName = engineName;
			instanceCreateInfo.engineVersion = engineVersion;
	#if DEBUG
			instanceCreateInfo.runtimeErrorChecking = true;
	#else
			instanceCreateInfo.runtimeErrorChecking = false;
	#endif
			instanceCreateInfo.backend = LGFXBackendType_Vulkan;
			instanceCreateInfo.enabledExtensionsCount = extensionsCount;
			instanceCreateInfo.enabledExtensions = extensions;

			this->instance = LGFXCreateInstance(&instanceCreateInfo);

			//create device
			LGFXDeviceCreateInfo deviceCreateInfo = {0};
			deviceCreateInfo.requiredFeatures.fillModeNonSolid = true;
			//deviceCreateInfo.requiredFeatures.wideLines = true;
			this->device = LGFXCreateDevice(this->instance, &deviceCreateInfo);
		}
	}
	bool Application::AddWindow(const char *name, i32 width, i32 height, bool resizeable, bool fullscreen, bool maximized, void *iconData, u32 iconWidth, u32 iconHeight, LGFXSwapchainPresentationMode presentMode)
	{
		Window *result = (Window *)allocator.Allocate(sizeof(Window));

		const GLFWvidmode *vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		if (width == 0)
		{
			width = vidMode->width;
		}
		if (height == 0)
		{
			height = vidMode->height;
		}
		*result = Window(allocator, name, width, height, resizeable, maximized, fullscreen, iconData, iconWidth, iconHeight, presentMode);
		
		if (framesPerSecond <= -1.0f)
		{
			framesPerSecond = (float)result->GetCurrentMonitorFramerate();
			//glfwsetwindow((GLFWwindow *)result.handle, GLFW_REFRESH_RATE, );
		}

		windows.Add(result);
		glfwSetWindowUserPointer((GLFWwindow*)result->handle, result);

		return true;
	}
	void Application::ResetDeltaTimer()
	{
		glfwSetTime(0.0);
		shouldResetDeltaTimer = true;
	}
	void Application::Run(ApplicationUpdateFunction updateFunc, ApplicationUpdateFunction fixedUpdateFunc, ApplicationDrawFunction drawFunc, ApplicationUpdateFunction postEndDrawFunc, ApplicationVoidFunction initFunc, ApplicationVoidFunction deinitFunc)
	{
		AstralCanvas::globalTemplateStore = AstralCanvas::BatchTemplateStore(GetCAllocator(), AstralCanvas::applicationInstance.device);
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
			fixedUpdateTimer += deltaTime;

			bool runUpdate = true;
			bool minimized = windows.count > 0;
			for (i32 i = (i32)windows.count - 1; i >= 0; i--)
			{
				Window *window = windows[i];
				if (window->isDisposed)
				{
					allocator.Free(window);
					windows.RemoveAt_Pullback((usize)i);
					continue;
				}

				if (window->resolution.X > 0 && window->resolution.Y > 0)
				{
					minimized = false;
					break;
				}
			}
			if (windows.count == 0 && !noWindows)
			{
				break;
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
				Window *window = windows[i];
				Maths::Point2 resolution = window->resolution;
				Maths::Point2 frameSize = window->frameBufferSize;
				
				if (resolution.X == 0 || resolution.Y == 0 || frameSize.X == 0 || frameSize.Y == 0)
				{
					continue;
				}
				currentWindow = windows.ptr[i];
				if (runUpdate || alwaysUpdate)
				{
					updateFunc(deltaTime);
				}
				
            	window->windowInputState.ResetPerFrameInputStates();

				//begin draw
				if (LGFXNewFrame(device, &window->swapchain, (u32)window->frameBufferSize.X, (u32)window->frameBufferSize.Y))
				{
					LGFXCommandBufferReset(window->mainCommandBuffer);
					LGFXCommandBufferBegin(window->mainCommandBuffer, true);

					drawFunc(deltaTime, window);

					//end draw
					LGFXCommandBufferEndSwapchain(window->mainCommandBuffer, window->swapchain);
					LGFXSubmitFrame(device, window->swapchain);

					if (postEndDrawFunc != NULL)
					{
						postEndDrawFunc(deltaTime);
					}
				}
			}

			if (windows.count == 0 && !noWindows)
			{
				break;
			}

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
		AstralCanvas::globalTemplateStore.deinit();

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
