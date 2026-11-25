CFLAGS ?= -O1
CFLAGS += -Wall -Wextra -std=c99 -D_DEFAULT_SOURCE -Iinclude

# raylib flags come from pkg-config; fall back to a typical Linux/X11 link line
RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
RAYLIB_LIBS := $(shell pkg-config --libs raylib 2>/dev/null || echo -lraylib -lGL -lpthread -ldl -lrt -lX11)
LDLIBS += $(RAYLIB_LIBS) -lm

GAME = build/pong
TEST = build/test_game
GAME_SRC = src/main.c src/game.c
HEADERS = include/pong.h

.PHONY: all game test run clean

all: game

game: $(GAME)

test: $(TEST)
	./$(TEST)

run: $(GAME)
	./$(GAME)

$(GAME): $(GAME_SRC) $(HEADERS) | build
	$(CC) $(CFLAGS) $(RAYLIB_CFLAGS) -o $@ $(GAME_SRC) $(LDFLAGS) $(LDLIBS)

$(TEST): tests/test_game.c src/game.c $(HEADERS) | build
	$(CC) $(CFLAGS) $(RAYLIB_CFLAGS) -o $@ tests/test_game.c src/game.c $(LDFLAGS) $(LDLIBS)

build:
	mkdir -p build

clean:
	rm -rf build
