#ifndef MAP_H_
#define MAP_H_
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

struct map;

int  map_width(struct map *map);
int  map_height(struct map *map);
char map_tile(struct map *map, int x, int y);
void map_set_tile(struct map *map, int x, int y, char tile);
struct map *map_new(int width, int height);
void map_delete(struct map *map);

#endif
