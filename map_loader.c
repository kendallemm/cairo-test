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

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "map_loader.h"

static char * read_line_from(char *buffer, int buflen, int *line_width)
{
	*line_width = 0;
	while ((buflen != 0) && (*buffer != '\r') && (*buffer != '\n')) {
		buflen--;
		(*line_width)++;
		buffer++;
	}
	if (buflen > 0) {
		int return_first = (*buffer == '\r');
		buflen--;
		buffer++;
		if (!return_first && buflen != 0) {
			if (*buffer == '\n') {
				buflen--;
				buffer++;
			}
		}
	}
	return buffer;
}

/* Should I put in a max map file size here? 
	Load a map file one line at a time.  For now the contents of a map file
   look like this (this is subject to change):

	XXXXX
   X...X
   X.X.X
   X.|.X
   XXXXX

	Each line must be of the same length, only 1 newline must end each line.
   Newlines must be of the form:  [ \n, \r, \r\n ].
	
	Any number of newlines may end the file.
*/
struct map * load_map_from_path (const char *path)
{
	struct stat stat_buf;
	struct map *map  = NULL;
	char       *buffer = NULL;
	int         width = 0, height = 0;
	int         fd;
	char       *original = NULL;

	
	if ((fd = open(path, O_RDONLY)) >= 0 && fstat(fd, &stat_buf) == 0) {
		int linewidth = 0;
		char *current = 0;
		int buflen    = stat_buf.st_size;

		buffer = (char *)malloc((size_t)buflen + 1);
		if (!buffer) {
			goto cleanup;
		}
		read(fd, buffer, (size_t)buflen);
		original = buffer;

		while (original + stat_buf.st_size > buffer) {
			current = read_line_from(buffer, buflen, &linewidth);
			/* all widths must match */
			if (original != buffer) {
				if (linewidth == 0) {
					goto end_of_line;
				}
				if (linewidth != width) {
					goto cleanup;
				}
				if (current+width >= original + stat_buf.st_size) {
					goto end_of_line;
				}
			} else {
				width = linewidth;
			}
			memmove(original+((height+1)*width), current, width);
			end_of_line:
			height++;
			buffer = current;
		}

		map = map_new(width, height);
		{
			for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++)
			{
				map_set_tile(map, x, y, original[x + y * width]);
			}
		}
	}

	cleanup:
	if (original) free(original);

	return map;
}
