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

#include "drawing.h"

static const int height = 640;
static const int width  = 640;
static const int door_height = 7.0;
static const int door_width  = 5.0;

static float left_bias = 0.0;

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
	distance += (10.0 - door_width)/2.0;
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
	distance += (10.0 - door_width)/2.0;
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

void do_door(cairo_t *cr, float dist)
{
	if (dist == 0.0) {
		open_door(cr, dist);
	} else {
		wall(cr, dist);
		door(cr, dist);
	}
}

void both_walls(cairo_t *cr, float dist)
{
	left_wall(cr, dist);
	right_wall(cr, dist);
}

void both_doors(cairo_t *cr, float dist)
{
	left_door(cr, dist);
	right_door(cr, dist);
}

float modify_left_bias (float amount)
{
	left_bias += amount;
	return left_bias;
}

void set_left_bias(float amount)
{
	left_bias = amount;
}

float display_height ()
{
	return height;
}

float display_width ()
{
	return width;
}
