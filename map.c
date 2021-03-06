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

#include <stdlib.h>
#include "map.h"

struct map
{
	size_t width;
	size_t height;
	char *data;
};

/*@null@*/
struct map *map_new(size_t width, size_t height)
{
	struct map *map = (struct map *)malloc(sizeof(struct map));
	if (map)
	{
		map->width = width;
		map->height = height;
		map->data = (char *)malloc(width * height * sizeof(char));
	}

	return map;
}

void map_delete (struct map *map)
{
	if (map) {
		if (map->data) free(map->data);
		free (map);
	}
}

int map_width (struct map *map)
{
	return (int)map->width;
}

int map_height (struct map *map)
{
	return (int)map->height;
}

char map_tile (struct map *map, int x, int y)
{
	if (x < 0 || y < 0 || x >= map_width(map) || y >= map_height(map))
		return 'X';
	return map->data[x + y * map_width(map)];
}

void map_set_tile(struct map *map, int x, int y, char tile)
{
	if (x < 0 || y < 0 || x >= map_width(map) || y >= map_height(map))
		return;
	map->data[x + y * map_width(map)] = tile;
}
