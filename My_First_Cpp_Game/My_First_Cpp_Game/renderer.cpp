#include "utils.cpp"

internal void
clear_screen(u32 color) {
	u32* pixel = (u32*)render_state.memory;
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			*pixel++ = color;
		}
	}
}

internal void
draw_rect(int x0, int y0, int x1, int y1, u32 color) {

	/*x0 = clamp(0, x0, 6);
	x1 = clamp(0, x1, render_state.width);
	y0 = clamp(0, y0, render_state.height);
	y1 = clamp(0, y1, render_state.height);*/

	for (int y = y0; y < y1; y++) {
		// This is because the pixel is being drawn from bottom left to top right
		// We offset the starting point of the memory with initial x => for moving the pointer from left to right
		// plus (y time width) for the height => for moving the pointer from bottom to top. y because y will in crease making the pixel line increment as well.
		// Use y0 will result in a 1 line pixel
		u32* pixel = (u32*)render_state.memory + x0 + (y * render_state.width);
		for (int x = x0; x < x1; x++) {
			*pixel++ = color;
		}
	}
}