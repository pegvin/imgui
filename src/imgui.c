#include "imgui.h"
#include "os/os.h"
#include "base/math.h"

#include <stdio.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

ImGuiCtx imgui_init(void) {
	Display* dpy = XOpenDisplay(NULL);
	if (Unlikely(!dpy)) {
		os_abort_with_message(1, str8_lit("Failed to open the display"));
	}

	// Load The First Available Font From The Array
	const char* FontNames[] = { "9x15", "8x16", "8x13", "7x14", "7x13", "6x13", "6x12", "6x10", "6x9", "5x8", "5x7", "4x6" };
	XFontStruct *fontInfo = NULL;
	for (unsigned long i = 0; i < ArrayCount(FontNames); i++) {
		fontInfo = XLoadQueryFont(dpy, FontNames[i]);
		if (fontInfo) {
			break;
		}
	}

	if (!fontInfo) {
		os_abort_with_message(1, str8_lit("Failed to load any font"));
	}

	// Create Window
	const int s = XDefaultScreen(dpy);
	Screen* scr = XScreenOfDisplay(dpy, s);
	Rect window_size = { 320, 240 };
	Window w = XCreateSimpleWindow(
		dpy, XRootWindowOfScreen(scr),
		(WidthOfScreen(scr) / 2) - (window_size.w / 2), // Center Window Horizontally
		(HeightOfScreen(scr) / 2) - (window_size.h / 2), // Center Window Vertically
		window_size.w, window_size.h, 0, 0, WhitePixel(dpy, s)
	);

	// Create GC
	XGCValues gcValues = {0};
	gcValues.font = fontInfo->fid;
	gcValues.foreground = BlackPixel(dpy, s);
	GC gc = XCreateGC(dpy, w, GCFont + GCForeground, &gcValues);

	// Select Inputs
	U64 EventMasks = 0;
	EventMasks |= ExposureMask;
	EventMasks |= StructureNotifyMask; // For Handling Changes To Window Size, Etc.
	EventMasks |= KeyPressMask | KeyReleaseMask;
	EventMasks |= ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
	XSelectInput(dpy, w, EventMasks);

	// Window Title
	XStoreName(dpy, w, "Immediate Mode GUI");

	// Disable Resizing & Hint To Center The Window
	// XSizeHints hints = {0};
	// hints.flags       = PSize | PMinSize | PMaxSize | PWinGravity;
	// hints.min_width   = hints.max_width  = hints.base_width  = window_size.w;
	// hints.min_height  = hints.max_height = hints.base_height = window_size.h;
	// hints.win_gravity = CenterGravity;
	// XSetWMNormalHints(dpy, w, &hints);

	// Set Window Type
	Atom type = XInternAtom(dpy,"_NET_WM_WINDOW_TYPE", False);
	Atom value = XInternAtom(dpy,"_NET_WM_WINDOW_TYPE_DIALOG", False);
	XChangeProperty(dpy, w, type, XA_ATOM, 32, PropModeReplace, (unsigned char*)&value, 1);

	// Delete Window Property
	Atom wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, w, &wmDeleteMessage, 1);

	// Show & Raise The Window
	XMapRaised(dpy, w);

	ImGuiCtx ctx = {
		.platform = {
			.display = dpy,
			.screen = scr,
			.font = fontInfo,
			.window = w,
			.gc = gc,
			.wmDeleteMessage = wmDeleteMessage
		},
		.window_size = window_size,
		.should_close = 0,
		.mouse = { 0, 0 },
		.mouse_down = 0,
		.hot_item = 0,
		.active_item = 0
	};

	return ctx;
}

void imgui_release(ImGuiCtx* ctx) {
	ImGuiPlatform platform = ctx->platform;
	XFreeFont(platform.display, platform.font);
	XFreeGC(platform.display, platform.gc);
	XDestroyWindow(platform.display, platform.window);
	XCloseDisplay(platform.display);
}

void imgui_begin_frame(ImGuiCtx* ctx) {
	ctx->frame_start = os_now_milliseconds();

	XEvent e;
	while (XPending(ctx->platform.display)) {
		XNextEvent(ctx->platform.display, &e);
		if (e.type == Expose) {
		} else if (e.type == KeyPress) {
			// printf("[LOG] Key Pressed! (%d)\n", e.xkey.keycode);
		} else if (e.type == KeyRelease) {
			// printf("[LOG] Key Released! (%d)\n", e.xkey.keycode);
		} else if (e.type == ButtonPress) {
			if (e.xbutton.button == 1) {
				ctx->mouse_down = 1;
			}
			// printf("[LOG] Mouse Button Pressed! (%d)\n", e.xbutton.button);
		} else if (e.type == ButtonRelease) {
			if (e.xbutton.button == 1) {
				ctx->mouse_down = 0;
			}
			// printf("[LOG] Mouse Button Released! (%d)\n", e.xbutton.button);
		} else if (e.type == MotionNotify) {
			ctx->mouse = point_from(e.xmotion.x, e.xmotion.y);
			// printf("[LOG] Mouse Moved! (Relative: %d, %d) (Absolute: %d, %d)\n", e.xmotion.x, e.xmotion.y, e.xmotion.x_root, e.xmotion.y_root);
		} else if (e.type == ConfigureNotify) {
			ctx->window_size.w = e.xconfigure.width;
			ctx->window_size.h = e.xconfigure.height;
			// printf("[LOG] Window Resized (%dx%d)\n", e.xconfigure.width, e.xconfigure.height);
		} else if (e.type == DestroyNotify || (e.type == ClientMessage && (Atom)e.xclient.data.l[0] == ctx->platform.wmDeleteMessage)) {
			ctx->should_close = 1;
		}
	}

	ctx->hot_item = 0;
	XClearWindow(ctx->platform.display, ctx->platform.window);
}

void imgui_end_frame(ImGuiCtx* ctx) {
	ctx->frame_end = os_now_milliseconds();

	if (!ctx->mouse_down) { // Reset Active Item If Mouse Was Released
		ctx->active_item = 0;
	}

	// Shitty Frame Limiter At 15 FPS
	os_sleep_milliseconds(1000 / 15);
}

B32 imgui_button(ImGuiCtx* ctx, U64 id, Rng2D rect) {
	if (point_intersects_rng2d(ctx->mouse, rect)) {
		ctx->hot_item = id;
		if (ctx->active_item < 1 && ctx->mouse_down) {
			ctx->active_item = id;
		}
	}

	B32 is_hot = ctx->hot_item == (S64)id;
	B32 is_active = ctx->active_item == (S64)id;

	RGBU8 btn_color;
	if (is_hot) {
		if (is_active) {
			btn_color = rgb_u8(0, 255, 0);
		} else {
			btn_color = rgb_u8(255, 0, 0);
		}
	} else {
		btn_color = rgb_u8(0, 0, 0);
	}

	imgui_draw_rect_filled(ctx, btn_color, rect);

	return ctx->mouse_down == 0 && is_hot && is_active;
}

#define _RGBA(r, g, b, a) (((U32)(a) << 24) + ((U32)(r) << 16) + ((U32)(g) << 8) + ((U32)(b)))
#define _RGB(r, g, b) _RGBA(r, g, b, 255)

void imgui_draw_rect(ImGuiCtx* ctx, RGBU8 color, Rng2D coords) {
	ImGuiPlatform platform = ctx->platform;
	XSetForeground(platform.display, platform.gc, _RGB(color.r, color.g, color.b));
	XDrawRectangle(
		platform.display, platform.window, platform.gc,
		coords.min.x, coords.min.y,
		coords.max.x - coords.min.x, coords.max.y - coords.min.y
	);
}

void imgui_draw_rect_filled(ImGuiCtx* ctx, RGBU8 color, Rng2D coords) {
	ImGuiPlatform platform = ctx->platform;
	XSetForeground(platform.display, platform.gc, _RGB(color.r, color.g, color.b));
	XFillRectangle(
		platform.display, platform.window, platform.gc,
		coords.min.x, coords.min.y,
		coords.max.x - coords.min.x, coords.max.y - coords.min.y
	);
}
