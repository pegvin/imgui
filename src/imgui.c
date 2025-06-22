#include "imgui.h"
#include "os/os.h"

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
	EventMasks |= KeyPressMask | KeyReleaseMask;
	EventMasks |= ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
	XSelectInput(dpy, w, EventMasks);

	// Window Title
	XStoreName(dpy, w, "Immediate Mode GUI");

	// Disable Resizing & Hint To Center The Window
	XSizeHints hints = {0};
	hints.flags       = PSize | PMinSize | PMaxSize | PWinGravity;
	hints.min_width   = hints.max_width  = hints.base_width  = window_size.w;
	hints.min_height  = hints.max_height = hints.base_height = window_size.h;
	hints.win_gravity = CenterGravity;
	XSetWMNormalHints(dpy, w, &hints);

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
		.display = dpy,
		.screen = scr,
		.font = fontInfo,
		.window = w,
		.gc = gc,
		.window_size = window_size,

		// Misc.
		.wmDeleteMessage = wmDeleteMessage
	};

	return ctx;
}

void imgui_release(ImGuiCtx* ctx) {
	XFreeFont(ctx->display, ctx->font);
	XFreeGC(ctx->display, ctx->gc);
	XDestroyWindow(ctx->display, ctx->window);
	XCloseDisplay(ctx->display);
}

#define _RGBA(r, g, b, a) (((U32)(a) << 24) + ((U32)(r) << 16) + ((U32)(g) << 8) + ((U32)(b)))
#define _RGB(r, g, b) _RGBA(r, g, b, 255)

void imgui_draw_rect(ImGuiCtx* ctx, RGBU8 color, Rng2D coords) {
	XSetForeground(ctx->display, ctx->gc, _RGB(color.r, color.g, color.b));
	XDrawRectangle(
		ctx->display, ctx->window, ctx->gc,
		coords.min.x, coords.min.y,
		coords.max.x - coords.min.x, coords.max.y - coords.min.y
	);
}

void imgui_draw_rect_filled(ImGuiCtx* ctx, RGBU8 color, Rng2D coords) {
	XSetForeground(ctx->display, ctx->gc, _RGB(color.r, color.g, color.b));
	XFillRectangle(
		ctx->display, ctx->window, ctx->gc,
		coords.min.x, coords.min.y,
		coords.max.x - coords.min.x, coords.max.y - coords.min.y
	);
}
