# pong
Clone of the classic game using raylib.


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

The key bindings:

    - Player one: (w and s)

    - Player two: (up and down arrows)

    - Start game: (Enter or m)

    - Pause / continue: (Space)

    - Menu: (when paused or after a win, press m)

    - Rematch: (after a win, press Enter)

    - Exit: (Esc)

### Tests

Run the game logic unit tests (no window needed):

    make test
