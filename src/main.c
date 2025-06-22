#include "imgui.h"

#include <stdio.h>
#include <X11/Xlib.h>

int main(void) {
	ImGuiCtx ctx = imgui_init();

	XEvent e;
	while (1) {
		XNextEvent(ctx.display, &e);
		if (e.type == Expose) {
			imgui_draw_rect(&ctx, rgb_u8(255, 0, 127), (Rng2D){ .min = {2, 2}, .max = {50, 50} });
			imgui_draw_rect_filled(&ctx, rgb_u8(0, 255, 0), (Rng2D){ .min = {60, 60}, .max = {100, 100} });
		} else if (e.type == KeyPress) {
			printf("[LOG] Key Pressed! (%d)\n", e.xkey.keycode);
		} else if (e.type == KeyRelease) {
			printf("[LOG] Key Released! (%d)\n", e.xkey.keycode);
		} else if (e.type == ButtonPress) {
			printf("[LOG] Mouse Button Pressed! (%d)\n", e.xbutton.button);
		} else if (e.type == ButtonRelease) {
			printf("[LOG] Mouse Button Released! (%d)\n", e.xbutton.button);
		} else if (e.type == MotionNotify) {
			printf("[LOG] Mouse Moved! (Relative: %d, %d) (Absolute: %d, %d)\n", e.xmotion.x, e.xmotion.y, e.xmotion.x_root, e.xmotion.y_root);
		} else if (e.type == DestroyNotify || (e.type == ClientMessage && (Atom)e.xclient.data.l[0] == ctx.wmDeleteMessage)) {
			break;
		}
	}

	imgui_release(&ctx);
	return 0;
}
