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

	Application::Application()
	{
	}
	void ApplicationInit(IAllocator allocator, string appName, string engineName, u32 appVersion, u32 engineVersion, float framesPerSecond)
	{
		glfwInit();
		Application result;
		result.framesPerSecond = framesPerSecond;
		result.allocator = allocator;
		result.windows = vector<Window>(allocator);
		result.appName = appName;
		result.engineName = engineName;
		result.appVersion = appVersion;
		result.engineVersion = engineVersion;
		applicationInstance = result;

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
		applicationInstance.device = LGFXCreateDevice(applicationInstance.instance, &deviceCreateInfo);
	}
	bool Application::AddWindow(const char *name, i32 width, i32 height, bool resizeable, void *iconData, u32 iconWidth, u32 iconHeight)
	{
		Window result;
		glfwWindowHint(GLFW_REFRESH_RATE, (i32)this->framesPerSecond);
		if (WindowInit(this->allocator, name, &result, width, height, resizeable, iconData, iconWidth, iconHeight))
		{
			windows.Add(result);
			glfwSetWindowUserPointer((GLFWwindow*)result.handle, &windows.ptr[windows.count - 1]);

			return true;
		}
		return false;
	}
	void Application::ResetDeltaTimer()
	{
		glfwSetTime(0.0);
		shouldResetDeltaTimer = true;
	}
	void Application::Run(ApplicationUpdateFunction updateFunc, ApplicationDrawFunction drawFunc, ApplicationUpdateFunction postEndDrawFunc, ApplicationInitFunction initFunc, ApplicationDeinitFunction deinitFunc)
	{
		if (initFunc != NULL)
		{
			initFunc();
		}
		startTime = (float)glfwGetTime();
		endTime = startTime;

		bool shouldStop = false;
		while (!shouldStop)
		{
			float deltaTime = endTime - startTime;

			bool slowdownPollEvents = true;

			for (usize i = 0; i < windows.count; i++)
			{
				if (windows.ptr[i].resolution.X != 0 && windows.ptr[0].resolution.Y != 0)
				{
					slowdownPollEvents = false;
				}
			}
			if (slowdownPollEvents)
			{
				while (windows.ptr[0].resolution.X == 0 || windows.ptr[0].resolution.Y == 0)
				{
					deltaTime = endTime - startTime;
					refreshTimer += deltaTime;
					if (refreshTimer >= 0.03f)
					{
						refreshTimer = 0.0f;
						glfwPollEvents();
					}
					startTime = endTime;
					endTime = (float)glfwGetTime();
				}
			}
			else
			{
				refreshTimer = 0.0f;
				glfwPollEvents();
			}

			if (framesPerSecond < 1.0f || deltaTime >= 1.0f / framesPerSecond)
			{
				updateFunc(deltaTime);

				for (usize i = 0; i < windows.count; i++)
				{
					if (windows.ptr[i].resolution.X == 0 || windows.ptr[i].resolution.Y == 0)
					{
						continue;
					}
					if (windows.ptr[i].handle != NULL && !glfwWindowShouldClose((GLFWwindow*)windows.ptr[i].handle))
					{
						windows.ptr[i].windowInputState.ResetPerFrameInputStates();
					}
					else
					{
						windows.ptr[i].deinit();
						windows.RemoveAt_Swap(i);
						continue;
					}

					//begin draw
					if (LGFXNewFrame(device, &windows.ptr[i].swapchain, (u32)windows.ptr[i].resolution.X, (u32)windows.ptr[i].resolution.Y))
					{
						LGFXCommandBufferReset(windows.ptr[i].mainCommandBuffer);
						LGFXCommandBufferBegin(windows.ptr[i].mainCommandBuffer, true);

						drawFunc(deltaTime, &windows.ptr[i]);

                        //end draw
            			LGFXCommandBufferEndSwapchain(windows.ptr[i].mainCommandBuffer, windows.ptr[i].swapchain);
						LGFXSubmitFrame(device, windows.ptr[i].swapchain);

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
				if (windows.count == 0)
				{
					break;
				}
				startTime = endTime;
			}
			endTime = (float)glfwGetTime();
			if (this->shouldResetDeltaTimer)
			{
				startTime = endTime;
				this->shouldResetDeltaTimer = false;
			}
		}

		//await rendering process shutdown

		if (deinitFunc != NULL)
		{
			deinitFunc();
		}

        //deinitialize backend
		LGFXDestroyDevice(device);

		LGFXDestroyInstance(instance);
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
