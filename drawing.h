#ifndef DRAWING_H
#define DRAWING_H
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

void wall(cairo_t *cr, float distance);
void left_wall(cairo_t *cr, float distance);
void left_door(cairo_t *cr, float distance);
void right_door(cairo_t *cr, float distance);
void door(cairo_t *cr, float distance);
void open_door(cairo_t *cr, float distance);
void right_wall(cairo_t *cr, float distance);
void do_door(cairo_t *cr, float dist);
void both_walls(cairo_t *cr, float dist);
void both_doors(cairo_t *cr, float dist);
void chest(cairo_t *cr, float dist);
void ladder_down(cairo_t *cr, float dist);
void ladder_up(cairo_t *cr, float dist);

float modify_left_bias(float amount);
void set_left_bias(float amount);
float display_height();
float display_width();

#endif
