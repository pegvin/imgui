#ifndef SRC_IMGUI_H
#define SRC_IMGUI_H
#pragma once

#include "base/types.h"
#include "base/math.h"
#include <X11/Xlib.h>

typedef struct {
	Display* display;
	Screen* screen;
	XFontStruct* font;
	Window window;
	GC gc;
	Rect window_size;

	// Misc.
	Atom wmDeleteMessage;
} ImGuiCtx;

// Init
ImGuiCtx imgui_init(void);
void imgui_release(ImGuiCtx* ctx);

// Drawing
void imgui_draw_rect(ImGuiCtx* ctx, RGBU8 color, Rng2D coords);
void imgui_draw_rect_filled(ImGuiCtx* ctx, RGBU8 color, Rng2D coords);

#endif // SRC_IMGUI_H
