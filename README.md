# pong
Clone of the classic game using raylib.

Play it in the browser: https://torresnelson.github.io/pong/


### How to compile?

You need a C compiler, `make`, `pkg-config` and `raylib`. With Nix, enter
a shell that provides them:

    nix-shell

On Debian/Ubuntu, install them with:

    sudo apt install build-essential pkg-config libraylib-dev

Then compile:

    make

### How to play it?

Run the game:

    make run

or run the binary directly:

    ./build/pong

In the menu, choose ONE PLAYER (against the computer) or TWO PLAYERS with
the up/down arrows or w/s and press Enter, or click an option. 1 and 2
start that option directly. The computer starts easy and levels up a little
every few points and after every match, up to level 10. Its level is shown
under its score.

The key bindings:

    - Player one: (w and s, or the arrows when playing alone)

    - Player two: (up and down arrows)

    - Menu: choose with (up/down or w/s), start with (Enter or m), or click

    - Menu shortcuts: (1) one player, (2) two players

    - Pause / continue: (Space)

    - Menu: (when paused or after a win, press m)

    - Rematch: (after a win, press Enter)

    - Exit: (Esc)

### Tests

Run the game logic unit tests (no window needed):

    make test

### Web version

The same C code also builds for the browser with Emscripten. `make web`
downloads raylib (version `RAYLIB_VERSION`, 6.2 by default), compiles it for
the web and writes the page to `build/web/` (`index.html`, `index.js`,
`index.wasm`). With Nix:

    nix-shell -p emscripten gnumake git --run 'make web'
    make serve-web    # then open http://localhost:8000

Every push to `master` runs the tests, builds the web version and publishes
it to GitHub Pages (`.github/workflows/pages.yml`). In the browser Esc does
not close the page.
