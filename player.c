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

#include "direction.h"
#include "player.h"

static int _player_x = 1, _player_y = 1;
static int _player_facing = DIRECTION_EAST;
static int gold = 0;

void player_set_x(int x)
{
	_player_x = x;
}

void player_set_y(int y)
{
	_player_y = y;
}

int player_x()
{
	return _player_x;
}

int player_y()
{
	return _player_y;
}

int player_facing()
{
	return _player_facing;
}

int player_gold()
{
	return gold;
}

int player_modify_gold(int amount)
{
	gold += amount;
	return gold;
}

int player_set_gold(int amount)
{
	return gold = amount;
}

void player_turn_right(void)
{
	_player_facing += 1;
	if (player_facing() > DIRECTION_WEST)
		_player_facing = DIRECTION_NORTH;
}

void player_turn_left(void)
{
	_player_facing -= 1;
	if (player_facing() < DIRECTION_NORTH)
		_player_facing = DIRECTION_WEST;
}
