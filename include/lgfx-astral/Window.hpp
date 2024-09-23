#pragma once
#include "lgfx-astral/InputState.hpp"
#include "lgfx/lgfx.h"
#include "Maths/All.h"
#include "Linxc.h"
#include "option.hpp"
#include "string.hpp"

namespace AstralCanvas
{
	enum WindowMouseState
	{
		WindowMouseState_Default,
		WindowMouseState_Hidden,
		WindowMouseState_Disabled
	};

	def_delegate(WindowOnTextInputFunction, void, void *window, u32 characterUnicode);
	def_delegate(WindowOnKeyInteractedFunction, void, void *window, AstralCanvas::Keys key, i32 action);
	struct Window
	{
		void *customCursorHandle;
		void *handle;
		LGFXSwapchain swapchain;
		LGFXCommandBuffer mainCommandBuffer;

		Maths::Point2 resolution;
		Maths::Point2 position;
		InputState windowInputState;
		string windowTitle;
		bool isFullscreen;
		bool *justResized;

		WindowOnTextInputFunction onTextInputFunc;
		WindowOnKeyInteractedFunction onKeyInteractFunc;

		bool isDisposed;

		Window();
		void deinit();
		inline Maths::Rectangle AsRectangle()
		{
			return Maths::Rectangle(0, 0, resolution.X, resolution.Y);
		}
		void SetWindowTitle(string title);
		void SetFullscreen(bool value);
		void SetMouseState(WindowMouseState state);
		WindowMouseState GetMouseState();
		void SetMouseIcon(void *iconData, u32 iconWidth, u32 iconHeight, i32 originX, i32 originY);
		void CloseWindow();
		i32 GetCurrentMonitorFramerate();
	};

	bool WindowInit(IAllocator allocator, const char *name, Window * result, i32 width, i32 height, bool resizeable, void *iconData, u32 iconWidth, u32 iconHeight);
}