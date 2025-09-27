#pragma once
#include "lgfx-astral/Window.hpp"
#include "lgfx/lgfx.h"
#include "vector.hpp"
#include "allocators.hpp"
#include "Maths/All.h"
#include "string.hpp"

namespace AstralCanvas
{
	def_delegate(ApplicationUpdateFunction, void, float);
	def_delegate(ApplicationDrawFunction, void, float, Window *);
	def_delegate(ApplicationInitFunction, void);
	def_delegate(ApplicationDeinitFunction, void);

	struct Application
	{
		collections::vector<Window *> windows;
		Window *currentWindow;
		IAllocator allocator;

		LGFXInstance instance;
		LGFXDevice device;

		bool alwaysUpdate;
		bool shouldShutdown;

		string appName;
		string engineName;
		u32 appVersion;
		u32 engineVersion;
		float startTime;
		float endTime;
		float updateTimer;
		float fixedUpdateTimer;
		bool shouldResetDeltaTimer;

		float framesPerSecond;
		float fixedTimeStep;
		float timeScale;

		Application();
		bool AddWindow(text name, i32 width, i32 height, bool resizeable, bool fullscree, bool maximized, void *iconData, u32 iconWidth, u32 iconHeight, LGFXSwapchainPresentationMode presentMode);
		void Run(ApplicationUpdateFunction updateFunc, ApplicationUpdateFunction fixedUpdateFunc, ApplicationDrawFunction drawFunc, ApplicationUpdateFunction postEndDrawFunc, ApplicationInitFunction initFunc, ApplicationDeinitFunction deinitFunc);
		void ResetDeltaTimer();
	};

	extern Application applicationInstance;

	void ApplicationInit(IAllocator allocator, string appName, string engineName, u32 appVersion, u32 engineVersion, float framesPerSecond, bool noWindow);
	text GetClipboardText();
	void SetClipboardText(text text);
	double GetElapsedTime();
}