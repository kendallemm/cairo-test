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

#define _GNU_SOURCE
#include <math.h> // powf
#include <stdio.h>

#include <SDL.h>
#include <cairo.h>

#include "direction.h"
#include "drawing.h"
#include "map.h"
#include "map_loader.h"
#include "player.h"

#define message printf
struct map * current_map;

SDL_Window   *window;
SDL_Renderer *renderer;
SDL_Texture  *texture, *stats_texture;

int stats_height()
{
	return 240;
}

int stats_width()
{
	return display_width()+240;
}

int window_height()
{
	return display_height() + stats_height() + 2;
}

int window_width()
{
	return stats_width() + 2;
}

void window_setup (void)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window   = SDL_CreateWindow("Cairo!", 20, 20,
		window_width(), window_height(), 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture  = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
		display_width(), display_height()
	);
	stats_texture = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
		stats_width(), stats_height()
	);
}

typedef void (*drawing_fn_t)(cairo_t *, int, int, int, float);

void draw_square (cairo_t *cr, int approach, int x, int y, float dist, drawing_fn_t drawfn)
{
	if (y >= 0 && y < map_height(current_map))
	if (x >= 0 && x < map_width(current_map))
	{
		drawfn(cr, approach, x, y, dist);
	}
	if (approach < 0) modify_left_bias(10.0);
	if (approach > 0) modify_left_bias(-10.0);
}

void iterate_east (cairo_t *cr, int steps, drawing_fn_t drawfn)
{
	float dist = steps * 10.0;
	int x = player_x() + steps;

	if (x >= map_width(current_map))
		return;

	modify_left_bias(-10.0);
	for (int y = player_y() - steps-1; y < player_y(); y++)
	{
		draw_square (cr, y-player_y(), x, y, dist, drawfn);
	}
	set_left_bias(steps * 10.0);
	modify_left_bias(10.0);
	for (int y = player_y() + steps+1; y >= player_y();  y--)
	{
		draw_square (cr, y-player_y(), x, y, dist, drawfn);
	}
}

void iterate_north (cairo_t *cr, int steps, drawing_fn_t drawfn)
{
	int y = player_y() - steps;
	float dist = steps * 10.0;

	if (y < 0)
		return;

	modify_left_bias(-10.0);
	for (int x = player_x() - steps -1; x < player_x(); x++)
	{
		draw_square (cr, x-player_x(), x, y, dist, drawfn);
	}
	set_left_bias(steps * 10.0);
	modify_left_bias(10.0);
	for (int x = player_x() + steps +1; x >= player_x(); x--)
	{
		draw_square (cr, x-player_x(), x, y, dist, drawfn);
	}
}

void iterate_west (cairo_t *cr, int steps, drawing_fn_t drawfn)
{
	int x = player_x() - steps;
	float dist = steps * 10.0;

	if (x < 0) return;

	modify_left_bias(-10);
	for (int y = player_y() + steps+ 1; y > player_y(); y--)
	{
		draw_square (cr, player_y()-y, x, y, dist, drawfn);
	}
	set_left_bias(steps * 10.0);
	modify_left_bias(10);
	for (int y = player_y() - steps -1; y <= player_y(); y++)
	{
		draw_square (cr, player_y()-y, x, y, dist, drawfn);
	}
}

void iterate_south (cairo_t *cr, int steps, drawing_fn_t drawfn)
{
	int y = player_y() + steps;
	float dist = steps * 10.0;
	if (y > map_height(current_map)) return;

	modify_left_bias(-10.0);
	for (int x = player_x() + steps + 1; x > player_x(); x--)
	{
		draw_square (cr, player_x()-x, x, y, dist, drawfn);
	}
	set_left_bias(10.0*steps);
	modify_left_bias(10.0);
	for (int x = player_x() - steps - 1; x <= player_x(); x++)
	{
		draw_square (cr, player_x()-x, x, y, dist, drawfn);
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
	return player_facing() == DIRECTION_EAST || player_facing() == DIRECTION_WEST;
}

int vertical()
{
	return player_facing() == DIRECTION_NORTH || player_facing() == DIRECTION_SOUTH;
}

void draw_flat_back (cairo_t *cr, int hand, int x, int y, float dist)
{
	hand = hand;
	dist += 10.0;
	switch (map_tile(current_map, x, y)) {
		case 'X': wall(cr, dist); break;
		case '|': if (horizontal()) { do_door(cr, dist); } else { wall(cr,dist); }; break;
		case '-': if (vertical()) { do_door(cr, dist); } else { wall(cr,dist); };  break;
		case '.': break;
	}
}

void draw_flat_front (cairo_t *cr, int hand, int x, int y, float dist)
{
	hand = hand;
	if (dist < 0.0) return;
	switch (map_tile(current_map, x, y)) {
		case 'X': break;
		case '|': if (horizontal()) { do_door(cr, dist); return; } break;
		case '-': if (vertical()) { do_door(cr, dist); return; }; break;
		case '.': return;
		default: return;
	}
	wall(cr, dist); 
}

void draw_core (cairo_t *cr, int hand, int x, int y, float dist)
{
	void (*wallfn)(cairo_t*,float) = right_wall;
	void (*doorfn)(cairo_t*,float) = right_door;
	if (hand > 0) wallfn = left_wall;
	if (!hand) wallfn = both_walls;
	if (!hand) doorfn = both_doors;
	if (hand > 0) doorfn = left_door;

	switch (map_tile(current_map, x, y)) {
		case 'T': chest(cr, dist); break;
		case 'X': wallfn(cr, dist); break;
		case '|': wallfn(cr, dist); if (vertical ()) { doorfn(cr, dist); }; break;
		case '-': wallfn(cr, dist); if (horizontal ()) { doorfn(cr, dist); }; break;
		case 'D': ladder_down(cr, dist); break;
		case 'U': ladder_up(cr, dist); break;
		case '.': break;
	}
}

void cairoize(SDL_Texture *t, int w, int h, cairo_surface_t **psurface, cairo_t **pcr)
{
	void *pixels;
	int   pitch;

	SDL_LockTexture(t, NULL, &pixels, &pitch);
	{
		*psurface =
			cairo_image_surface_create_for_data(
				pixels, CAIRO_FORMAT_ARGB32, w, h, pitch);
		*pcr = cairo_create(*psurface);
	}
}

void decairoize(SDL_Texture *t, cairo_surface_t *cairo_surface, cairo_t *cr)
{
	cairo_destroy(cr);
	// should I do this?
	cairo_surface_destroy(cairo_surface);
	SDL_UnlockTexture(t);
}

void paint_stats(void)
{
	cairo_surface_t *cairo_surface;
	cairo_t         *cr;

	cairoize(stats_texture, stats_width(), stats_height(), &cairo_surface, &cr);
	// clear to black
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_paint(cr);

	cairo_set_source_rgb(cr, 255, 255, 255);
	cairo_select_font_face(cr, "Mono", CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 18.0);
	cairo_move_to(cr, 10.0, 20.0);
	{
		char *buffer = NULL;
		asprintf(&buffer, "Gold: %i", player_gold());
		cairo_show_text(cr, buffer);
		free(buffer);
	}
	cairo_set_source_rgb(cr, 255, 255, 255);
	decairoize(stats_texture, cairo_surface, cr);
}

void paint_view(void)
{
	cairo_surface_t *cairo_surface;
	cairo_t         *cr;

	cairoize(texture, display_width(), display_height(), &cairo_surface, &cr);
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
		set_left_bias(steps * -10.0);
		iterator[player_facing()] (cr, steps, draw_core);
		set_left_bias(steps * -10.0);
		iterator[player_facing()] (cr, steps, draw_flat_front);
	}
	decairoize(texture, cairo_surface, cr);
}

void draw_frame (SDL_Rect r)
{
	r.x -=1;
	r.y -=1;
	r.w +=1;
	r.w +=1;
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &r);
}

void paint(void)
{
	SDL_RenderClear(renderer);
	{
		SDL_Rect r = {1, 1, display_width(), display_height()};
		SDL_RenderCopy(renderer, texture, NULL, &r);
		draw_frame(r);

		r.x = 1; r.y = display_height() + 2;
		r.w = stats_width(); r.h = stats_height();
		SDL_RenderCopy(renderer, stats_texture, NULL, &r);
		draw_frame(r);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	}
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

void on_moved(int oldx, int oldy, int newx, int newy)
{
	if (map_tile(current_map, newx, newy) == 'T') {
		printf ("Arr, there be treasure here!\n");
	}
	if (strchr("|-", map_tile(current_map, oldx, oldy))) {
		printf("A door creaks closed behind you\n");
	}
	if (strchr("|-", map_tile(current_map, newx, newy))) {
		printf("The door opens\n");
	}
}

void move_forward(void)
{
	int newx = player_x(), newy = player_y();
	switch (player_facing()) {
		case DIRECTION_EAST: newx += 1; break;
		case DIRECTION_WEST: newx -= 1; break;
		case DIRECTION_NORTH: newy -= 1; break;
		case DIRECTION_SOUTH: newy += 1; break;
	}
	if (map_tile(current_map, newx, newy) != 'X') {
		on_moved(player_x(), player_y(), newx, newy);
		player_set_x(newx);
		player_set_y(newy);
		mark_dirty();
	}
}

void move_backward(void)
{
	int newx = player_x(), newy = player_y();
	switch (player_facing()) {
		case DIRECTION_EAST: newx -= 1; break;
		case DIRECTION_WEST: newx += 1; break;
		case DIRECTION_NORTH: newy += 1; break;
		case DIRECTION_SOUTH: newy -= 1; break;
	}
	if (map_tile(current_map, newx, newy) != 'X') {
		on_moved(player_x(), player_y(), newx, newy);
		player_set_x(newx);
		player_set_y(newy);
		mark_dirty();
	}
}

void turn_right(void)
{
	player_turn_right();
	mark_dirty();
}

void turn_left(void)
{
	player_turn_left();
	mark_dirty();
}

int treasure_max ()
{
	return 10;
}

void do_get(void)
{
	if (map_tile(current_map, player_x(), player_y()) == 'T') {
		int gold = rand() % treasure_max() + 1;
		player_modify_gold(gold);
		message("You found %i gold!\n", gold);
		map_set_tile(current_map, player_x(), player_y(), '.');
		mark_dirty();
	}
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
					case SDLK_g: do_get(); break;
				}
			}
		}
	}
}

void load_map ()
{
	current_map = load_map_from_path("map");
	if (!current_map) {
		fprintf(stderr, "Can't open map file: %s\n", "map");
		exit(1);
	}
}

void release_map()
{
	map_delete(current_map);
	current_map = NULL;
}

int main (int argc, char *argv[])
{
	if (argc > 1) {
		printf ("All arguments to %s ignored.\n", argv[0]);
	}
	window_setup();
	load_map();
	mark_dirty();
	while (!quitflag) {
		if (is_dirty()) {
			paint_view();
			paint_stats();
			paint();
			mark_clean();
		}
		handle_input();
		SDL_Delay(1);
	}
	release_map();
	window_teardown();
	return 0;
}
