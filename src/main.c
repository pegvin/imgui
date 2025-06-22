#include "imgui.h"
#include "base/math.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
	ImGuiCtx ctx = imgui_init();

	while (!ctx.io.RequestedClose) {
		imgui_begin_frame(&ctx);

		static Rect button = { 40, 30 };
		static Point p1 = { 20, 20 };
		if (imgui_button(&ctx, rng2d_from_xy_wh(p1.x, p1.y, button.w, button.h))) {
			printf("[INFO] Button Pressed!\n");
			p1.x = rand() % (ctx.io.WindowSize.w - button.w);
			p1.y = rand() % (ctx.io.WindowSize.h - button.h);
		}

		static Point p2 = { 70, 20 };
		if (imgui_button(&ctx, rng2d_from_xy_wh(p2.x, p2.y, button.w, button.h))) {
			printf("[INFO] Button Pressed!\n");
			p2.x = rand() % (ctx.io.WindowSize.w - button.w);
			p2.y = rand() % (ctx.io.WindowSize.h - button.h);
		}

		static Point p3 = { 120, 20 };
		if (imgui_button(&ctx, rng2d_from_xy_wh(p3.x, p3.y, button.w, button.h))) {
			printf("[INFO] Button Pressed!\n");
			p3.x = rand() % (ctx.io.WindowSize.w - button.w);
			p3.y = rand() % (ctx.io.WindowSize.h - button.h);
		}

		imgui_end_frame(&ctx);
	}

	imgui_release(&ctx);
	return 0;
}
