#ifndef SRC_IMGUI_H
#define SRC_IMGUI_H
#pragma once

#include "base/types.h"
#include "base/math.h"
#include "os/os.h"

#include <X11/Xlib.h>

typedef enum {
	ImGuiMouse_Left = 0,
	ImGuiMouse_Right,
	ImGuiMouse_Middle,
	ImGuiMouse_COUNT
} ImGuiMouse;

typedef struct {
	// Mouse
	Point MousePos;
	Point MousePosPrev;
	B32   MouseDown[ImGuiMouse_COUNT];

	// Window
	Rect  WindowSize;
	B32   RequestedClose;
} ImGuiIO;

typedef struct {
	Display* display;
	Screen* screen;
	XFontStruct* font;
	Window window;
	GC gc;
	Atom wmDeleteMessage;
} ImGuiPlatform;

typedef struct {
	ImGuiIO io;

	S64 hot_item;
	S64 active_item;

	U64 frame_start;
	U64 frame_end;

	ImGuiPlatform platform;
} ImGuiCtx;

// Init
ImGuiCtx imgui_init(void);
void imgui_release(ImGuiCtx* ctx);

void imgui_begin_frame(ImGuiCtx* ctx);
void imgui_end_frame(ImGuiCtx* ctx);

// Widgets
#define IMID (__LINE__)

#define imgui_button(ctx, rect) imgui_button_(ctx, IMID, rect)
B32 imgui_button_(ImGuiCtx* ctx, U64 id, Rng2D rect);

// Drawing
void imgui_draw_rect(ImGuiCtx* ctx, RGBU8 color, Rng2D coords);
void imgui_draw_rect_filled(ImGuiCtx* ctx, RGBU8 color, Rng2D coords);

#endif // SRC_IMGUI_H
