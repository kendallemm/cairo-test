/*
 *  Copyright 2016 Kendall E. Blake
 *
 *  This file is part of cairo-test.
 *
 *  cairo-test is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  cairo-test is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with cairo-test.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <math.h> // powf

#include <SDL.h>
#include <cairo.h>

#include "drawing.h"

#define INCHES(x) ((x)/12.0)

static const int height = 480;
static const int width  = 480;
static const int door_height = 7.0;
static const int door_width  = 5.0;

static const int chest_height = INCHES(18.0);
static const int chest_width  = INCHES(22.0);
static const int chest_depth  = INCHES(14.0);

const float ladder_height   = 4.2;
const float ladder_width    = 2.0;
const float rung_spacing    = 1.1;
const float ladder_box_side = 4.0;

static float left_bias = 0.0;

void ladder_outline_color(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
}

void chest_outline_color(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 102.0/255.0, 81.0/255.0, 70.0/255.0);
}

void chest_fill_color(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 141.0/255.0, 101.0/255.0, 56.0/255.0);
}

void door_outline_color(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
}

void door_fill_color(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 0.5, 0.5, 0.0);
}

void wall_color_light(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 0.60, 0.450, 0.30);
	
}
void wall_color_dark(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 0.50, 0.350, 0.25);
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
	wall_color_dark(cr);
	move_to_3(cr, left_bias, 0.0, distance);
	line_to_3(cr, left_bias + 10.0, 0.0, distance);
	line_to_3(cr, left_bias + 10.0, 10.0, distance);
	line_to_3(cr, left_bias, 10.0, distance);
	line_to_3(cr, left_bias, 0.0, distance);
	cairo_stroke_preserve(cr);
	wall_color_light(cr);
	cairo_fill(cr);
}

void left_wall(cairo_t *cr, float distance)
{
	wall_color_dark(cr);
	move_to_3(cr, left_bias, 0.0, distance);
	line_to_3(cr, left_bias, 0.0, distance+10.0);
	line_to_3(cr, left_bias, 10.0, distance+10.0);
	line_to_3(cr, left_bias, 10.0, distance);
	line_to_3(cr, left_bias, 0.0, distance);
	cairo_stroke_preserve(cr);
	wall_color_light(cr);
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

	wall_color_light(cr);
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
	wall_color_dark(cr);
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

void chest(cairo_t *cr, float distance)
{
	float
		left_x = (10.0 - chest_width) / 2.0,
		right_x = (10.0 + chest_width) / 2.0,
		top_y  = (chest_height),
		bottom_y = 0.0,
		back_z = distance + (10.0 + chest_depth) / 2.0,
		front_z = distance + (10.0 - chest_depth) / 2.0
	;

	// back
	chest_outline_color(cr);
	move_to_3(cr, left_x, bottom_y, back_z);
	line_to_3(cr, left_x, top_y, back_z);
	line_to_3(cr, right_x, top_y, back_z);
	line_to_3(cr, right_x, bottom_y, back_z);
	line_to_3(cr, left_x, bottom_y, back_z);
	cairo_stroke_preserve(cr);
	chest_fill_color(cr);
	cairo_fill(cr);

	// bottom
	chest_outline_color(cr);
	move_to_3(cr, left_x, bottom_y, back_z);
	line_to_3(cr, left_x, bottom_y, front_z);
	line_to_3(cr, right_x, bottom_y, front_z);
	line_to_3(cr, right_x, bottom_y, back_z);
	line_to_3(cr, left_x, bottom_y, back_z);
	cairo_stroke_preserve(cr);
	chest_fill_color(cr);
	cairo_fill(cr);

	// left
	chest_outline_color(cr);
	move_to_3(cr, left_x, bottom_y, back_z);
	line_to_3(cr, left_x, bottom_y, front_z);
	line_to_3(cr, left_x, top_y, front_z);
	line_to_3(cr, left_x, top_y, back_z);
	line_to_3(cr, left_x, bottom_y, back_z);
	cairo_stroke_preserve(cr);
	chest_fill_color(cr);
	cairo_fill(cr);

	// right
	chest_outline_color(cr);
	move_to_3(cr, right_x, bottom_y, back_z);
	line_to_3(cr, right_x, bottom_y, front_z);
	line_to_3(cr, right_x, top_y, front_z);
	line_to_3(cr, right_x, top_y, back_z);
	line_to_3(cr, right_x, bottom_y, back_z);
	cairo_stroke_preserve(cr);
	chest_fill_color(cr);
	cairo_fill(cr);

	// top
	chest_outline_color(cr);
	move_to_3(cr, left_x, top_y, back_z);
	line_to_3(cr, left_x, top_y, front_z);
	line_to_3(cr, right_x, top_y, front_z);
	line_to_3(cr, right_x, top_y, back_z);
	line_to_3(cr, left_x, top_y, back_z);
	cairo_stroke_preserve(cr);
	chest_fill_color(cr);
	cairo_fill(cr);

	// front
	chest_outline_color(cr);
	move_to_3(cr, left_x, bottom_y, front_z);
	line_to_3(cr, left_x, top_y, front_z);
	line_to_3(cr, right_x, top_y, front_z);
	line_to_3(cr, right_x, bottom_y, front_z);
	line_to_3(cr, left_x, bottom_y, front_z);
	cairo_stroke_preserve(cr);
	chest_fill_color(cr);
	cairo_fill(cr);
}

void draw_ladder(cairo_t *cr, float distance,
	float ladder_left, float ladder_right,
	float ladder_top, float ladder_bottom)
{
	float halfway = distance + 10.0 / 2.0;
	ladder_outline_color(cr);
	/* The ladder */
	move_to_3(cr, left_bias + ladder_left, ladder_bottom, halfway);
	line_to_3(cr, left_bias + ladder_left, ladder_top, halfway);
	cairo_stroke(cr);
	move_to_3(cr, left_bias + ladder_right, ladder_bottom, halfway);
	line_to_3(cr, left_bias + ladder_right, ladder_top, halfway);
	cairo_stroke(cr);
	{ /* rungs */
		float rung;
		if (ladder_bottom > 0) {
			rung = ladder_bottom + rung_spacing;
		} else {
			float nrungs = (ladder_top - ladder_bottom) / rung_spacing - 1.0;
			
			rung = ladder_bottom + rung_spacing * modff(nrungs, &nrungs);
		}
		for (; rung < ladder_top; rung += rung_spacing) {
			move_to_3(cr, left_bias + ladder_left, rung, halfway);
			line_to_3(cr, left_bias + ladder_right, rung, halfway);
			cairo_stroke(cr);
		}
	}
}

void right_wall(cairo_t *cr, float distance)
{
	wall_color_light(cr);
	move_to_3(cr, left_bias + 10.0, 0.0, distance);
	line_to_3(cr, left_bias + 10.0, 0.0, distance+10.0);
	line_to_3(cr, left_bias + 10.0, 10.0, distance+10.0);
	line_to_3(cr, left_bias + 10.0, 10.0, distance);
	line_to_3(cr, left_bias + 10.0, 0.0, distance);
	cairo_stroke_preserve(cr);
	wall_color_dark(cr);
	cairo_fill(cr);
}

void ladder_down(cairo_t *cr, float distance)
{
	/*
	 * down ladders have boxes on the floor for the hole
	 * and then a ladder poking up.
	 * Everything is just lines, so no need to get cranky about drawing order
	 * ladders extend up 4 feet and are 2 feet wide.
	 * Rungs are 1.5 feet apart.
    * The hole boxes are 4x4
	 */

	float ladder_left  = (10.0 - ladder_width) / 2.0;
	float ladder_right = (10.0 + ladder_width) / 2.0;
	float ladder_top   = ladder_height;
	float ladder_bottom = 0;
	draw_ladder(cr, distance, ladder_left, ladder_right, ladder_top, ladder_bottom);
}

void ladder_up(cairo_t *cr, float distance)
{
	/*
	 * down ladders have boxes on the floor for the hole
	 * and then a ladder poking up.
	 * Everything is just lines, so no need to get cranky about drawing order
	 * ladders extend up 4 feet and are 2 feet wide.
	 * Rungs are 1.5 feet apart.
    * The hole boxes are 4x4
	 */

	float ladder_left  = (10.0 - ladder_width) / 2.0;
	float ladder_right = (10.0 + ladder_width) / 2.0;
	float ladder_top   = 10.0;
	float ladder_bottom = 10.0 - ladder_height;
	draw_ladder(cr, distance, ladder_left, ladder_right, ladder_top, ladder_bottom);
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
