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

#include <SDL.h>
#include <cairo.h>

const int height = 640;
const int width  = 640;

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

void l_wall_2(cairo_t *cr)
{
	cairo_move_to(cr, 160.0, 160.0);
	cairo_line_to(cr, 240.0, 240.0);
	cairo_line_to(cr, 240.0, 400.0);
	cairo_line_to(cr, 160.0, 480.0);
}

void r_wall_2(cairo_t *cr)
{
	cairo_move_to(cr, 480.0, 160.0);
	cairo_line_to(cr, 400.0, 240.0);
	cairo_line_to(cr, 400.0, 400.0);
	cairo_line_to(cr, 480.0, 480.0);
}

void wall_2(cairo_t *cr)
{
	cairo_move_to(cr, 240.0, 240.0);
	cairo_line_to(cr, 400.0, 240.0);
	cairo_line_to(cr, 400.0, 400.0);
	cairo_line_to(cr, 240.0, 400.0);
	cairo_line_to(cr, 240.0, 240.0);
}

void dead_end_2(cairo_t *cr)
{
	l_wall_2(cr);
	r_wall_2(cr);
	wall_2(cr);
}

void l_wall_1(cairo_t *cr)
{
	cairo_move_to(cr, 0.0, 0.0);
	cairo_line_to(cr, 160.0, 160.0);
	cairo_line_to(cr, 160.0, 480.0);
	cairo_line_to(cr, 0.0, 640.0);
}

void r_wall_1(cairo_t *cr)
{
	cairo_move_to(cr, 640.0, 0.0);
	cairo_line_to(cr, 480.0, 160.0);
	cairo_line_to(cr, 480.0, 480.0);
	cairo_line_to(cr, 640.0, 640.0);
}

void wall_1(cairo_t *cr)
{
	cairo_move_to(cr, 160.0, 160.0);
	cairo_line_to(cr, 480.0, 160.0);
	cairo_line_to(cr, 480.0, 480.0);
	cairo_line_to(cr, 160.0, 480.0);
	cairo_line_to(cr, 160.0, 160.0);
}

void dead_end_1(cairo_t *cr)
{
	l_wall_1(cr);
	r_wall_1(cr);
	wall_1(cr);
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
		dead_end_2(cr);
		l_wall_1(cr);
		r_wall_1(cr);
		cairo_stroke(cr);
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
