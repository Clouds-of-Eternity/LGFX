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
		collections::vector<Window> windows;
		IAllocator allocator;

		LGFXInstance instance;
		LGFXDevice device;

		string appName;
		string engineName;
		u32 appVersion;
		u32 engineVersion;
		float startTime;
		float endTime;
		bool shouldResetDeltaTimer;

		float framesPerSecond;
		float timeScale;

		Application();
		bool AddWindow(text name, i32 width, i32 height, bool resizeable = true, void *iconData = NULL, u32 iconWidth = 0, u32 iconHeight = 0);
		void Run(ApplicationUpdateFunction updateFunc, ApplicationDrawFunction drawFunc, ApplicationUpdateFunction postEndDrawFunc, ApplicationInitFunction initFunc, ApplicationDeinitFunction deinitFunc);
		void ResetDeltaTimer();
	};

	extern Application applicationInstance;

	void ApplicationInit(IAllocator allocator, string appName, string engineName, u32 appVersion, u32 engineVersion, float framesPerSecond);
	text GetClipboardText();
	void SetClipboardText(text text);
}