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

const int height = 640;
const int width  = 640;
const int door_height = 7.0;
const int door_width  = 5.0;

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
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
}

void wall_outline_color(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
}

void wall_fill_color(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
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
	wall_outline_color(cr);
	move_to_3(cr, 0.0, 0.0, distance);
	line_to_3(cr, 10.0, 0.0, distance);
	line_to_3(cr, 10.0, 10.0, distance);
	line_to_3(cr, 0.0, 10.0, distance);
	line_to_3(cr, 0.0, 0.0, distance);
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
	wall_outline_color(cr);
	move_to_3(cr, 0.0, 0.0, distance);
	line_to_3(cr, 0.0, 0.0, distance+10.0);
	line_to_3(cr, 0.0, 10.0, distance+10.0);
	line_to_3(cr, 0.0, 10.0, distance);
	line_to_3(cr, 0.0, 0.0, distance);
	cairo_stroke_preserve(cr);
	wall_fill_color(cr);
	cairo_fill(cr);
}

void left_door(cairo_t *cr, float distance)
{
	door_outline_color(cr);
	move_to_3(cr, 0.0, 0.0, distance);
	line_to_3(cr, 0.0, door_height, distance);
	line_to_3(cr, 0.0, door_height, distance+door_width);
	line_to_3(cr, 0.0, 0.0, distance+door_width);
	line_to_3(cr, 0.0, 0.0, distance);
	cairo_stroke_preserve(cr);
	door_fill_color(cr);
	cairo_fill(cr);
}

void right_door(cairo_t *cr, float distance)
{
	door_outline_color(cr);
	move_to_3(cr, 10.0, 0.0, distance);
	line_to_3(cr, 10.0, door_height, distance);
	line_to_3(cr, 10.0, door_height, distance+door_width);
	line_to_3(cr, 10.0, 0.0, distance+door_width);
	line_to_3(cr, 10.0, 0.0, distance);
	cairo_stroke_preserve(cr);
	door_fill_color(cr);
	cairo_fill(cr);
}

void right_wall(cairo_t *cr, float distance)
{
	wall_outline_color(cr);
	move_to_3(cr, 10.0, 0.0, distance);
	line_to_3(cr, 10.0, 0.0, distance+10.0);
	line_to_3(cr, 10.0, 10.0, distance+10.0);
	line_to_3(cr, 10.0, 10.0, distance);
	line_to_3(cr, 10.0, 0.0, distance);
	cairo_stroke_preserve(cr);
	wall_fill_color(cr);
	cairo_fill(cr);
}

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

		cairo_set_source_rgb(cr, 255, 0, 0);
		cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(cr, 40.0);
		cairo_move_to(cr, 10.0, 50.0);
		cairo_show_text(cr, "Hello, world!");

		cairo_set_source_rgb(cr, 255, 255, 255);
/*
		cairo_move_to(cr, 50.0, 50.0);
		cairo_line_to(cr, 100.0, 100.0);
*/
		wall(cr, 20.0);
		left_wall(cr, 10.0);
		right_wall(cr, 10.0);
		left_wall(cr, 0.0);
		right_wall(cr, 0.0);
		left_door(cr, 2.5);
		right_door(cr, 12.5);

		cairo_destroy(cr);
		// should I do this?
		cairo_surface_destroy(cairo_surface);
	}
	SDL_UnlockTexture(texture);
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

int main (int argc, char *argv[])
{
	window_setup();
	paint();
	nap();
	window_teardown();
	return 0;
}
