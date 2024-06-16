#include <raylib.h>
#include <stdlib.h>



//global values

#define grid_w 50
#define grid_h 30
#define bg_color (Color) {66, 66, 70, 255}
#define empty_color (Color) {100, 100, 105, 255}
#define conductor_color (Color) {255, 255, 0, 255}
#define head_color (Color) {0, 0, 255, 255}
#define tail_color (Color) {255, 0, 0, 255}
#define selector_color (Color) {0, 0, 0, 255}



//types

typedef struct {
	int width;
	int height;
	int *tab; //values
} Board;

Board* init_board(int n, int m) {
	Board *b = malloc(sizeof(Board));
	b->width = n;
	b->height = m;
	b->tab = malloc(n*m * (sizeof(int)));
	return b;
}

void free_board(Board *b) {
	free(b->tab);
	free(b);
}



//functions

void window_init(Color bg) {
	InitWindow(0, 0, "Wireworld");
	ClearBackground(bg);
	SetTargetFPS(100);
}

int min(int x, int y) {
	if (x < y) x;
	else y;
}

void draw_color_display(int active_color) {
	int w = GetScreenWidth();
	int h = GetScreenHeight();
	int start_x = h - 100;
	int start_y = w / 2 - 75;
	DrawRectangle(start_y, start_x, 50, 50, conductor_color);
	DrawRectangle(start_y + 50, start_x, 50, 50, head_color);
	DrawRectangle(start_y + 100, start_x, 50, 50, tail_color);
	//draw selected rectangle
	Color active_truecolor;
	switch (active_color) {
	case 2 : active_truecolor = head_color; break;
	case 3 : active_truecolor = tail_color; break;
	default : active_truecolor = conductor_color; break;
	}
	int offset = active_color * 50;
	DrawRectangle(start_y - 55 + offset, start_x - 5, 60, 60, selector_color);
	DrawRectangle(start_y - 50 + offset, start_x, 50, 50, active_truecolor);
	DrawText("W", start_y + 14, start_x + 12, 30, selector_color);
	DrawText("H", start_y + 64, start_x + 12, 30, selector_color);
	DrawText("T", start_y + 114, start_x + 12, 30, selector_color);
}

void draw(Board *b, bool display_grid, int active_color) {
	int w = GetScreenWidth();
	int h = GetScreenHeight();
	int px = min((w-400) / grid_w, (h-400) / grid_h);
	int start_x = h / 2 - px * grid_h / 2;
	int start_y = w / 2 - px * grid_w / 2;

	ClearBackground(bg_color);
	for (int i=0; i < grid_h - 1; i++) {
		for (int j=0; j < grid_w - 1; j++) {
			int cell_size = px;
			if (display_grid) cell_size--;
			if (b->tab[j*grid_h + i] == 1) DrawRectangle(start_y + j * px, start_x + i * px, cell_size, cell_size, conductor_color);
			if (b->tab[j*grid_h + i] == 2) DrawRectangle(start_y + j * px, start_x + i * px, cell_size, cell_size, head_color);
			if (b->tab[j*grid_h + i] == 3) DrawRectangle(start_y + j * px, start_x + i * px, cell_size, cell_size, tail_color);
			if (b->tab[j*grid_h + i] == 0) DrawRectangle(start_y + j * px, start_x + i * px, cell_size, cell_size, empty_color);
		}
	}
	draw_color_display(active_color);
}

int head_neighbors(Board *b, int x, int y) {
	//count head neighbors without index out of bounds
	int count = 0;
	if (x > 0 && b->tab[y * grid_h + (x-1)] == 2) count++;
	if (x < grid_h - 1 && b->tab[y * grid_h + (x+1)] == 2) count++;
	if (y > 0 && b->tab[(y-1) * grid_h + x] == 2) count++;
	if (y < grid_w - 1 && b->tab[(y+1) * grid_h + x] == 2) count++;
	if (x > 0 && y > 0 && b->tab[(y-1) * grid_h + (x-1)] == 2) count++;
	if (x > 0 && y < grid_w - 1 && b->tab[(y+1) * grid_h + (x-1)] == 2) count++;
	if (x < grid_h - 1 && y > 0 && b->tab[(y-1) * grid_h + (x+1)] == 2) count++;
	if (x < grid_h - 1 && y < grid_w - 1 && b->tab[(y+1) * grid_h + (x+1)] == 2) count++;
	return count;
}

void update(Board *b) {
	//new updated board
	Board *b2 = init_board(grid_w, grid_h);
	for (int i=0; i < grid_h - 1; i++) {
		for (int j=0; j < grid_w - 1; j++) {
			//apply Wireworld rules
			if (b->tab[j * grid_h + i] == 0) b2->tab[j * grid_h + i] = 0;
			else if (b->tab[j * grid_h + i] == 2) b2->tab[j * grid_h + i] = 3;
			else if (b->tab[j * grid_h + i] == 3) b2->tab[j * grid_h + i] = 1;
			else if (b->tab[j * grid_h + i] == 1) {
				int neighbors = head_neighbors(b, i, j);
				if (neighbors == 1 || neighbors == 2) b2->tab[j * grid_h + i] = 2;
				else b2->tab[j * grid_h + i] = 1;
			}
		}
	}
	//paste values in main board and free the temp board
	for (int i=0; i < grid_h - 1; i++) {
		for (int j=0; j < grid_w - 1; j++) {
			b->tab[j * grid_h + i] = b2->tab[j * grid_h + i];
		}
	}
	free_board(b2);
}

void main() {
	//initialization
	window_init(bg_color);
	bool display_grid = true;
	bool running = false;
	int active_color = 1;

	//create blank board
	Board *b = init_board(grid_w, grid_h);
	for (int i=0; i < grid_h - 1; i++) {
		for (int j=0; j < grid_w - 1; j++) {
			b->tab[j*grid_h + i] = 0;
		}
	}
	draw(b, display_grid, active_color);

	//loop
	while (!WindowShouldClose()) {
		//controls : change_color
		if (IsKeyPressed(KEY_W)) {active_color = 1; draw(b, display_grid, active_color);}
		if (IsKeyPressed(KEY_H)) {active_color = 2; draw(b, display_grid, active_color);}
		if (IsKeyPressed(KEY_T)) {active_color = 3; draw(b, display_grid, active_color);}

		//controls : add cells
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			int w = GetScreenWidth();
			int h = GetScreenHeight();
			int px = min((w-200) / grid_w, (h-200) / grid_h);
			int start_x = h / 2 - px * grid_h / 2;
			int start_y = w / 2 - px * grid_w / 2;
			int mx = GetMouseX();
			int my = GetMouseY();
			if ((mx > start_y) && (my > start_x) && (mx < w - start_y) && (my < h - start_x)) {
				b->tab[grid_h * ((mx - start_y) / px) + ((my - start_x) / px)] = active_color;
				draw(b, display_grid, active_color);
			}
		}

		//controls : delete cells
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			int w = GetScreenWidth();
			int h = GetScreenHeight();
			int px = min((w-200) / grid_w, (h-200) / grid_h);
			int start_x = h / 2 - px * grid_h / 2;
			int start_y = w / 2 - px * grid_w / 2;
			int mx = GetMouseX();
			int my = GetMouseY();
			if ((mx > start_y) && (my > start_x) && (mx < w - start_y) && (my < h - start_x)) {
				b->tab[grid_h * ((mx - start_y) / px) + ((my - start_x) / px)] = 0;
				draw(b, display_grid, active_color);
			}
		}

		//controls : pause - unpause
		if (IsKeyPressed(KEY_SPACE)) running = !running;

		//controls : switch grid
		if (IsKeyPressed(KEY_G)) {display_grid = !display_grid; draw(b, display_grid, active_color);}

		//controls : clear grid
		if (IsKeyPressed(KEY_C)) {
			for (int i=0; i < grid_h - 1; i++) {
				for (int j=0; j < grid_w - 1; j++) {
					b->tab[j*grid_h + i] = 0;
				}
			}
			draw(b, display_grid, active_color);
		}

		//controls : step one frame
		if (IsKeyPressed(KEY_S)) {
			update(b);
			draw(b, display_grid, active_color);
		}

		//update board
		if (running) {
			update(b);
			draw(b, display_grid, active_color);
		}

		BeginDrawing();
		EndDrawing();
	}

	//de-initialization
	free_board(b);
}
