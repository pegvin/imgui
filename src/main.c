#include "imgui.h"
#include "base/math.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
	ImGuiCtx ctx = imgui_init();

	while (!ctx.should_close) {
		imgui_begin_frame(&ctx);

		static Point p = { 20, 20 };
		static Rect button = { 40, 30 };
		if (imgui_button(&ctx, 10, rng2d_from_xy_wh(p.x, p.y, button.w, button.h))) {
			printf("[INFO] Button Pressed!\n");
			p.x = rand() % (ctx.window_size.w - button.w);
			p.y = rand() % (ctx.window_size.h - button.h);
		}

		imgui_end_frame(&ctx);
	}

	imgui_release(&ctx);
	return 0;
}
