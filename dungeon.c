/*
 *  Copyright 2016 Kendall E. Blake
 *
 *  This file is part of This program.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with This program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <math.h> // powf

#include <SDL.h>
#include <cairo.h>

enum {
	DIRECTION_NORTH = 0,
	DIRECTION_EAST,
	DIRECTION_SOUTH,
	DIRECTION_WEST
};

const int height = 640;
const int width  = 640;
const int door_height = 7.0;
const int door_width  = 5.0;
#define MAP_H ((10))
#define MAP_W ((10))

float left_bias = 0.0;

int player_x = 1, player_y = 1;
int player_facing = DIRECTION_EAST;

static char dungeon_map [MAP_H*MAP_W+1] = 
	"XXXXXXXXXX"
   "X....XXXXX"
   "XXXX.XXXXX"
   "XXXX.XXXXX"
   "XXXX.|...X"
   "X....XXXXX"
   "X.XX.XXXXX"
   "X.XX-XXXXX"
   "X........X"
   "XXXXXXXXXX"
;

SDL_Window   *window;
SDL_Renderer *renderer;
SDL_Texture  *texture;
void         *pixels;

void window_setup (void)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window   = SDL_CreateWindow("Cairo!", 20, 20, width, height, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture  = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
		width, height
	);
}

void door_outline_color(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
}

void door_fill_color(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 0.5, 0.5, 0.0);
}

void wall_outline_color(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
}

void wall_fill_color(cairo_t *cr)
{
	static float r = 0.0, g = 0.0, b = 0.0;
	
	cairo_set_source_rgb(cr, r, g, b);
	r = (1.0+r/2.0);
	b += 1.0/20.0;
}

void eye_3_to_2(float x, float y, float z, float *out_x, float *out_y)
{
	float z_coeff  = 0.0;
	
	z_coeff = pow(2, 0-(z / 10.0));

	*out_x = (x-5) * z_coeff + 5.0;
	*out_y = (y-5) * z_coeff + 5.0;
}

void convert_cairo3(cairo_t *cr, float x, float y, float z, void (*fn)(cairo_t *, double, double))
{
	float x2, y2;
	eye_3_to_2(x, y, z, &x2, &y2);
	fn(cr, (x2/10.0 * width), (10.0 - y2)/10.0 * height);
}

void move_to_3(cairo_t *cr, float x, float y, float z)
{
	convert_cairo3(cr, x, y, z, cairo_move_to);
}

void line_to_3(cairo_t *cr, float x, float y, float z)
{
	convert_cairo3(cr, x, y, z, cairo_line_to);
}

void wall(cairo_t *cr, float distance)
{
	printf ("Drawing wall with left bias %f at distance %f\n", left_bias, distance);
	wall_outline_color(cr);
	move_to_3(cr, left_bias, 0.0, distance);
	line_to_3(cr, left_bias + 10.0, 0.0, distance);
	line_to_3(cr, left_bias + 10.0, 10.0, distance);
	line_to_3(cr, left_bias, 10.0, distance);
	line_to_3(cr, left_bias, 0.0, distance);
	cairo_stroke_preserve(cr);
	wall_fill_color(cr);
	cairo_fill(cr);
}

void draw_shape(cairo_t *cr, float(*points)[3], int npoints)
{
	float (*point)[3] = points;
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	move_to_3(cr, *points[0], *points[1], *points[2]);
	while (npoints--) {
		line_to_3(cr, *point[0], *point[1], *point[3]);
	}
	line_to_3(cr, *points[0], *points[1], *points[2]);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_fill(cr);
}

void wall_2(cairo_t *cr, float distance)
{
	float shape[4][3] = {
		{0.0, 0.0, 0.0},
		{10.0, 0.0, 0.0},
		{10.0, 10.0, 0.0},
		{0.0, 10.0, 0.0},
	};
	shape[0][2] = shape[1][2] = shape[2][2] = shape[3][2] = distance;
	draw_shape(cr, shape, 4);
}

void left_wall(cairo_t *cr, float distance)
{
	printf ("Drawing left wall with left bias %f at distance %f\n", left_bias, distance);
	wall_outline_color(cr);
	move_to_3(cr, left_bias, 0.0, distance);
	line_to_3(cr, left_bias, 0.0, distance+10.0);
	line_to_3(cr, left_bias, 10.0, distance+10.0);
	line_to_3(cr, left_bias, 10.0, distance);
	line_to_3(cr, left_bias, 0.0, distance);
	cairo_stroke_preserve(cr);
	wall_fill_color(cr);
	cairo_fill(cr);
}

void left_door(cairo_t *cr, float distance)
{
	door_outline_color(cr);
	move_to_3(cr, left_bias, 0.0, distance);
	line_to_3(cr, left_bias, door_height, distance);
	line_to_3(cr, left_bias, door_height, distance+door_width);
	line_to_3(cr, left_bias, 0.0, distance+door_width);
	line_to_3(cr, left_bias, 0.0, distance);
	cairo_stroke_preserve(cr);
	door_fill_color(cr);
	cairo_fill(cr);
}

void right_door(cairo_t *cr, float distance)
{
	door_outline_color(cr);
	move_to_3(cr, left_bias + 10.0, 0.0, distance);
	line_to_3(cr, left_bias + 10.0, door_height, distance);
	line_to_3(cr, left_bias + 10.0, door_height, distance+door_width);
	line_to_3(cr, left_bias + 10.0, 0.0, distance+door_width);
	line_to_3(cr, left_bias + 10.0, 0.0, distance);
	cairo_stroke_preserve(cr);
	door_fill_color(cr);
	cairo_fill(cr);
}

void door(cairo_t *cr, float distance)
{
	float door_start = (10.0 - door_width) / 2;
	
	door_outline_color(cr);
	move_to_3(cr, left_bias + door_start, 0.0, distance);
	line_to_3(cr, left_bias + door_start, door_height, distance);
	line_to_3(cr, left_bias + door_start+door_width, door_height, distance);
	line_to_3(cr, left_bias + door_start+door_width, 0.0, distance);
	line_to_3(cr, left_bias + door_start, 0.0, distance);
	cairo_stroke_preserve(cr);
	door_fill_color(cr);
	cairo_fill(cr);
}

void open_door(cairo_t *cr, float distance)
{
	float door_start = (10.0 - door_width) / 2;
	float door_end   = door_width + door_start;

	printf("Drawing open door with left bias %f at distance %f\n", left_bias, distance);
	wall_outline_color(cr);
	move_to_3(cr, left_bias, 0.0, distance);
	line_to_3(cr, left_bias + door_start, 0.0, distance);
	line_to_3(cr, left_bias + door_start, door_height, distance);
	line_to_3(cr, left_bias + door_end, door_height, distance);
	line_to_3(cr, left_bias + door_end, 0.0, distance);
	line_to_3(cr, left_bias + 10.0, 0.0, distance);
	line_to_3(cr, left_bias + 10.0, 10.0, distance);
	line_to_3(cr, left_bias, 10.0, distance);
	line_to_3(cr, left_bias, 0.0, distance);
	cairo_stroke_preserve(cr);
	wall_fill_color(cr);
	cairo_fill(cr);
}

void open_door_side(cairo_t *cr)
{
	float wall_depth = 2.0;
	float wall_start = (10.0-wall_depth)/2.0 + left_bias;
	float jamb_dist = door_width / 2.0;

	door_outline_color(cr);
	move_to_3(cr, wall_start, 0.0, jamb_dist);
	line_to_3(cr, wall_start, door_height, jamb_dist);
	line_to_3(cr, wall_start + wall_depth, door_height, jamb_dist);
	line_to_3(cr, wall_start + wall_depth, 0.0, jamb_dist);
	line_to_3(cr, wall_start, 0.0, jamb_dist);
	cairo_stroke_preserve(cr);
	door_fill_color(cr);
	cairo_fill(cr);
	door_outline_color(cr);
	move_to_3(cr, wall_start, door_height, jamb_dist);
	line_to_3(cr, wall_start, 10.0, 0.0);
	line_to_3(cr, wall_start + wall_depth, 10.0, 0.0);
	line_to_3(cr, wall_start + wall_depth, door_height, jamb_dist);
	line_to_3(cr, wall_start, door_height, jamb_dist);
	cairo_stroke_preserve(cr);
	door_fill_color(cr);
	cairo_fill(cr);
}

void right_wall(cairo_t *cr, float distance)
{
	printf ("Drawing right wall with left bias %f at distance %f\n", left_bias, distance);
	wall_outline_color(cr);
	move_to_3(cr, left_bias + 10.0, 0.0, distance);
	line_to_3(cr, left_bias + 10.0, 0.0, distance+10.0);
	line_to_3(cr, left_bias + 10.0, 10.0, distance+10.0);
	line_to_3(cr, left_bias + 10.0, 10.0, distance);
	line_to_3(cr, left_bias + 10.0, 0.0, distance);
	cairo_stroke_preserve(cr);
	wall_fill_color(cr);
	cairo_fill(cr);
}

void iterate_east (cairo_t *cr, int steps, void (*drawfn)(cairo_t *, int, int, int, float))
{
	float dist = steps * 10.0;
	int x = player_x + steps;

	if (x >= MAP_W)
		return;

	left_bias -=10.0;
	for (int y = player_y - steps-1; y < player_y; y++)
	{
		if (y >= 0 && y < MAP_H) {
			printf ("L-R Drawing (%i, %i) @ %f\n", x, y, dist);
			drawfn(cr, y-player_y, x, y, dist);
		}
		left_bias += 10.0;
	}
	left_bias = steps * 10.0;
	left_bias +=10.0;
	for (int y = player_y + steps+1; y >= player_y;  y--)
	{
		if (y >= 0 && y < MAP_H) {
			printf ("R-L Drawing (%i, %i) @ %f\n", x, y, dist);
			drawfn(cr, y-player_y, x, y, dist);
		}
		left_bias -= 10.0;
	}
}

void iterate_north (cairo_t *cr, int steps, void (*drawfn)(cairo_t *, int, int, int, float))
{
	int y = player_y - steps;

	if (y < 0)
		return;

	for (int x = player_x - steps; x < player_x + steps; x++)
	{
		if (x >= 0 && x < MAP_W) {
			drawfn(cr, x-player_x,x, y, steps * 10.0);
		}
		left_bias += 10.0;
	}
}

void iterate_west (cairo_t *cr, int steps, void (*drawfn)(cairo_t *, int, int, int, float))
{
	int x = player_x - steps;

	if (x < 0) return;

	for (int y = player_y + steps - 1; y > player_y; y--)
	{
		if (y >= 0 && y < MAP_H) {
			drawfn(cr, -(y - player_y), x, y, steps * 10.0);
		}
		left_bias += 10.0;
	}
	left_bias = steps * 10.0;
	for (int y = player_y - steps; y <= player_y; y++)
	{
		if (y >= 0 && y < MAP_H) {
			drawfn(cr, -(y - player_y), x, y, steps * 10.0);
		}
		left_bias -= 10.0;
	}
}

void iterate_south (cairo_t *cr, int steps, void (*drawfn)(cairo_t *, int, int, int, float))
{
	int y = player_y + steps;
	if (y >- MAP_H) return;
	for (int x = player_x + steps - 1; x > player_x - steps; x--)
	{
		if (x >= 0 && x < MAP_W) {
			drawfn(cr, -(x - player_x), x, y, steps * 10.0);
		}
		left_bias += 10.0;
	}
}

void (*iterator[])(cairo_t *, int, void (*fn)(cairo_t *, int, int, int, float))= {
	iterate_north,
	iterate_east,
	iterate_south,
	iterate_west
};

int horizontal()
{
	return player_facing == DIRECTION_EAST || player_facing == DIRECTION_WEST;
}

int vertical()
{
	return player_facing == DIRECTION_NORTH || player_facing == DIRECTION_SOUTH;
}

void do_door(cairo_t *cr, float dist)
{
	if (dist == 0.0) {
		open_door(cr, dist);
	} else {
		wall(cr, dist);
		door(cr, dist);
	}
}

void draw_flat_back (cairo_t *cr, int hand, int x, int y, float dist)
{
	dist += 10.0;
	switch (dungeon_map[x+y*MAP_W]) {
		case 'X': wall(cr, dist); break;
		case '|': if (horizontal()) { do_door(cr, dist); } else { wall(cr,dist); }; break;
		case '-': if (vertical()) { do_door(cr, dist); } else { wall(cr,dist); };  break;
		case '.': break;
	}
}

void draw_flat_front (cairo_t *cr, int hand, int x, int y, float dist)
{
	if (dist < 0.0) return;
	switch (dungeon_map[y*MAP_W+x]) {
		case 'X': break;
		case '|': if (horizontal()) { do_door(cr, dist); return; } break;
		case '-': if (vertical()) { do_door(cr, dist); return; }; break;
		case '.': return;
	}
	wall(cr, dist); 
}

void both_walls(cairo_t *cr, float dist)
{
	left_wall(cr, dist);
	right_wall(cr, dist);
}

void draw_core (cairo_t *cr, int hand, int x, int y, float dist)
{
	void (*wallfn)(cairo_t*,float) = right_wall;
	void (*doorfn)(cairo_t*,float) = left_door;
	if (hand > 0) wallfn = left_wall;
	if (!hand) wallfn = both_walls;
	if (hand > 0) doorfn = right_door;

	switch (dungeon_map[y*MAP_W+x]) {
		case 'X': wallfn(cr, dist); break;
		case '|': wallfn(cr, dist); if (vertical ()) { doorfn(cr, dist); }; break;
		case '-': wallfn(cr, dist); if (horizontal ()) { doorfn(cr, dist); }; break;
		case '.': break;
	}
}

/*
void a ()
{
}
*/

void paint(void)
{
	void *pixels;
	int   pitch;

	SDL_LockTexture(texture, NULL, &pixels, &pitch);
	{
		cairo_surface_t *cairo_surface =
			cairo_image_surface_create_for_data(
				pixels, CAIRO_FORMAT_ARGB32, width, height, pitch);
		cairo_t *cr = cairo_create(cairo_surface);

		// clear to black
		cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
		cairo_paint(cr);

		cairo_set_source_rgb(cr, 255, 0, 0);
		cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(cr, 40.0);
		cairo_move_to(cr, 10.0, 50.0);
		cairo_show_text(cr, "Hello, world!");
		cairo_set_source_rgb(cr, 255, 255, 255);
		for (int steps = 5; steps >= -2; steps--) {
			printf ("Step: %i\n", steps);
//			left_bias = steps * -10.0;
//			iterator[player_facing] (cr, steps, draw_flat_back);
			left_bias = steps * -10.0;
			iterator[player_facing] (cr, steps, draw_core);
			left_bias = steps * -10.0;
			iterator[player_facing] (cr, steps, draw_flat_front);
		}
	
/*
		cairo_move_to(cr, 50.0, 50.0);
		cairo_line_to(cr, 100.0, 100.0);
*/
/*
// flat 3
		left_bias = -30.0;
		wall(cr, 30.0);
		left_bias = -20.0;
		wall(cr, 30.0);
		left_bias = 10.0;
		wall(cr, 30.0);

// line 2
		left_bias = -10.0;
		left_wall(cr, 20.0);
		left_bias = 0.0;
		left_wall(cr, 20.0);
		left_bias = 10.0;
		right_wall(cr, 20.0);

// flat 2
		left_bias = -10.0;
		wall(cr, 20.0);
		door(cr, 20.0);
		left_bias = 10.0;
		wall(cr, 20.0);

// line 1
		left_bias = 0.0;
		right_wall(cr, 10.0);

// flat 1
		left_bias = 10.0;
		wall(cr, 10.0);
*/
/*
		door(cr, 20.0);
		left_wall(cr, 10.0);
		right_wall(cr, 10.0);
		left_wall(cr, 0.0);
		right_wall(cr, 0.0);
		left_door(cr, 2.5);
		right_door(cr, 12.5);
	//	open_door(cr, 0.0);
*/
		cairo_destroy(cr);
		// should I do this?
		cairo_surface_destroy(cairo_surface);
	}
	SDL_UnlockTexture(texture);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void window_teardown (void)
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void nap (void)
{
	SDL_Delay(5000);
}

int dirty_flag = 0;

void mark_dirty()
{
	dirty_flag = 1;
}

void mark_clean()
{
	dirty_flag = 0;
}

int is_dirty()
{
	return dirty_flag;
}

void move_forward(void)
{
	int newx = player_x, newy = player_y;
	switch (player_facing) {
		case DIRECTION_EAST: newx += 1; break;
		case DIRECTION_WEST: newx -= 1; break;
		case DIRECTION_NORTH: newy -= 1; break;
		case DIRECTION_SOUTH: newy += 1; break;
	}
	if (dungeon_map[newx+newy*MAP_W] != 'X') {
		player_x = newx;
		player_y = newy;
		mark_dirty();
	}
}

void move_backward(void)
{
	int newx = player_x, newy = player_y;
	switch (player_facing) {
		case DIRECTION_EAST: newx -= 1; break;
		case DIRECTION_WEST: newx += 1; break;
		case DIRECTION_NORTH: newy += 1; break;
		case DIRECTION_SOUTH: newy -= 1; break;
	}
	if (dungeon_map[newx+newy*MAP_W] != 'X') {
		player_x = newx;
		player_y = newy;
		mark_dirty();
	}
}

void turn_right(void)
{
	player_facing += 1;
	if (player_facing > DIRECTION_WEST)
		player_facing = DIRECTION_NORTH;
	mark_dirty();
}

void turn_left(void)
{
	player_facing -= 1;
	if (player_facing < DIRECTION_NORTH)
		player_facing = DIRECTION_WEST;
	mark_dirty();
}

int quitflag = 0;

void handle_input (void)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
			case SDL_KEYDOWN: {
				switch (ev.key.keysym.sym) {
					case SDLK_UP: move_forward(); break;
					case SDLK_DOWN: move_backward(); break;
					case SDLK_LEFT: turn_left(); break;
					case SDLK_RIGHT: turn_right(); break;
					case SDLK_q: quitflag = 1;
				}
			}
		}
	}
}

int main (int argc, char *argv[])
{
	window_setup();
	mark_dirty();
	while (!quitflag) {
		if (is_dirty()) {
			paint();
			mark_clean();
		}
		handle_input();
		SDL_Delay(1);
	}
	window_teardown();
	return 0;
}
