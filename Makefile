# gcc hello.c `pkg-config sdl2 --cflags --libs` `pkg-config cairo --cflags --libs`
CFLAGS=`pkg-config sdl2 --cflags` `pkg-config cairo --cflags`
LDFLAGS=`pkg-config sdl2 --libs` `pkg-config cairo --libs` -lm

all: hello dungeon map_test

hello: hello.o

dungeon: dungeon.o

map_test: map_test.o map.o

.PHONY: all clean
