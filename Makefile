# gcc hello.c `pkg-config sdl2 --cflags --libs` `pkg-config cairo --cflags --libs`
CFLAGS=`pkg-config sdl2 --cflags` `pkg-config cairo --cflags` -Wall -Werror
LDFLAGS=`pkg-config sdl2 --libs` `pkg-config cairo --libs` -lm
MAP_TEST_OBJECTS=map_test.o map.o
DUNGEON_OBJECTS=dungeon.o map.o
HELLO_OBJECTS=hello.o
BINARIES=hello dungeon map_test
OBJECTS=$(MAP_TEST_OBJECTS) $(DUNGEON_OBJECTS) $(HELLO_OBJECTS)

all: hello dungeon map_test

hello: hello.o

dungeon: dungeon.o map.o map_loader.o

map_test: map_test.o map.o map_loader.o

clean:
	rm -f $(OBJECTS) $(BINARIES)

.PHONY: all clean
