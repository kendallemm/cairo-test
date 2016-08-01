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
#include <stdio.h>

#include "map.h"

struct map *demo_map_setup ()
{
	#define DEMO_MAP_W 6
	#define DEMO_MAP_H 6
	char raw[] = 
		"XXXXXX"
   	"X....X"
   	"X.XX.X"
   	"X-XX.X"
		"X..|.X"
		"XXXXXX"
	;

	struct map *map = map_new(DEMO_MAP_W, DEMO_MAP_H);
	int x, y;

	for (x = 0; x < DEMO_MAP_W; x++)
	for (y = 0; y < DEMO_MAP_H; y++)
	{
		map_set_tile(map, x, y, raw[x + y * DEMO_MAP_W]);
	}
	return map;
}

typedef int (*test_fn)(void);

#define TEST(name) \
int inner_##name(void); \
int name (void) { \
	int res; \
	printf ("In %s . . . ", #name); \
	res = inner_##name(); \
	printf("%s\n", (res)?"Passed":"Failed"); \
	return res; \
} \
int inner_##name(void)

TEST(test_coordinates)
{
	struct map *map = demo_map_setup();
	int    res = map_tile(map, 3, 4) == '|';
	map_delete(map);
	return res;
}

TEST(test_set_get_cycle)
{
	struct map *map = map_new(1, 1);
	int res;
	map_set_tile(map, 0, 0, 'Z');
	res = map_tile(map, 0, 0) == 'Z';
	map_delete(map);
	return res;
}

int main (int argc, char *argv[])
{
	int passes = 0;
	int fails  = 0;

	test_fn functions [] = {
		test_coordinates,
		test_set_get_cycle,
	};

	if (argc > 1) {
		printf ("Ignoring arguments to %s\n", argv[0]);
	}

//	printf ("Woah: %i -- %i\n", sizeof(functions), sizeof(test_fn));
	for (size_t i = 0; i < sizeof(functions)/sizeof(test_fn); i++)
	{
		(functions[i]())?passes++:fails++;
	}
	printf ("Passed: %i\tFailed: %i\n", passes, fails);
	return fails != 0;
}
