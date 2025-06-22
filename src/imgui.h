#ifndef SRC_IMGUI_H
#define SRC_IMGUI_H
#pragma once

#include "base/types.h"
#include "base/math.h"
#include "os/os.h"

#include <X11/Xlib.h>

typedef struct {
	Display* display;
	Screen* screen;
	XFontStruct* font;
	Window window;
	GC gc;
	Atom wmDeleteMessage;
} ImGuiPlatform;

typedef struct {
	Point mouse;
	B32 mouse_down;

	S64 hot_item;
	S64 active_item;

	B32 should_close;

	U64 frame_start;
	U64 frame_end;

	Rect window_size;
	ImGuiPlatform platform;
} ImGuiCtx;

// Init
ImGuiCtx imgui_init(void);
void imgui_release(ImGuiCtx* ctx);

void imgui_begin_frame(ImGuiCtx* ctx);
void imgui_end_frame(ImGuiCtx* ctx);

// Widgets
B32 imgui_button(ImGuiCtx* ctx, U64 id, Rng2D rect);

// Drawing
void imgui_draw_rect(ImGuiCtx* ctx, RGBU8 color, Rng2D coords);
void imgui_draw_rect_filled(ImGuiCtx* ctx, RGBU8 color, Rng2D coords);

#endif // SRC_IMGUI_H
