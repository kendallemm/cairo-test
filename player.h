#ifndef PLAYER_H
#define PLAYER_H
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

int player_set_gold(int amount);
int player_modify_gold(int amount);
int player_gold();
int player_facing();
int player_y();
int player_x();
void player_set_y(int x);
void player_set_x(int x);
void player_turn_left(void);
void player_turn_right(void);

#endif
