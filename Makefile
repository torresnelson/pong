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

# Web build: Emscripten (emcc) plus raylib compiled for the web.
# The page and its files end up in build/web, ready for any static host.
EMCC ?= emcc
RAYLIB_VERSION ?= 6.2
RAYLIB_SRC ?= build/raylib-$(RAYLIB_VERSION)
RAYLIB_WEB_DIR = build/raylib-web
RAYLIB_WEB = $(RAYLIB_WEB_DIR)/libraylib.web.a
WEB_DIR = build/web
WEB_CFLAGS = -Os -Wall -std=gnu99 -D_DEFAULT_SOURCE -DPLATFORM_WEB -Iinclude -I$(RAYLIB_SRC)/src
WEB_LDFLAGS = -sUSE_GLFW=3 -sGL_ENABLE_GET_PROC_ADDRESS --shell-file web/shell.html
# A writable Emscripten cache (the Nix-provided one is read-only)
export EM_CACHE ?= $(abspath build/emcache)

.PHONY: all game test run clean web serve-web

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

web: $(WEB_DIR)/index.html

serve-web: web
	python3 -m http.server --directory $(WEB_DIR) 8000

$(RAYLIB_SRC): | build
	git clone --depth 1 --branch $(RAYLIB_VERSION) https://github.com/raysan5/raylib.git $@

$(RAYLIB_WEB): | $(RAYLIB_SRC)
	mkdir -p $(RAYLIB_WEB_DIR)
	$(MAKE) -C $(RAYLIB_SRC)/src PLATFORM=PLATFORM_WEB RAYLIB_RELEASE_PATH=$(abspath $(RAYLIB_WEB_DIR))

$(WEB_DIR)/index.html: $(GAME_SRC) $(HEADERS) web/shell.html $(RAYLIB_WEB)
	mkdir -p $(WEB_DIR)
	$(EMCC) $(WEB_CFLAGS) -o $@ $(GAME_SRC) $(RAYLIB_WEB) $(WEB_LDFLAGS)

clean:
	rm -rf build
