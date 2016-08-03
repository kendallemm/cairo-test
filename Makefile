# gcc hello.c `pkg-config sdl2 --cflags --libs` `pkg-config cairo --cflags --libs`
CFLAGS=`pkg-config sdl2 --cflags` `pkg-config cairo --cflags` -Wall -Werror -Wextra -pedantic -g
LDFLAGS=`pkg-config sdl2 --libs` `pkg-config cairo --libs` -lm
MAP_TEST_OBJECTS=map_test.o map.o map_loader.o
DUNGEON_OBJECTS=dungeon.o map.o drawing.o map_loader.o player.o
HELLO_OBJECTS=hello.o
BINARIES=hello dungeon map_test
OBJECTS=$(MAP_TEST_OBJECTS) $(DUNGEON_OBJECTS) $(HELLO_OBJECTS)

all: hello dungeon map_test

hello: hello.o

dungeon: $(DUNGEON_OBJECTS)

map_test: $(MAP_TEST_OBJECTS)

clean:
	rm -f $(OBJECTS) $(BINARIES)

.PHONY: all clean
