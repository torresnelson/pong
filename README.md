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

In the menu, press 1 to play alone against the computer or 2 (also Enter
or m) to play two players. The computer starts easy and levels up a little
every few points and after every match, up to level 10. Its level is shown
under its score.

The key bindings:

    - Player one: (w and s, or the arrows when playing alone)

    - Player two: (up and down arrows)

    - One player game: (1)

    - Two players game: (2, Enter or m)

    - Pause / continue: (Space)

    - Menu: (when paused or after a win, press m)

    - Rematch: (after a win, press Enter)

    - Exit: (Esc)

### Tests

Run the game logic unit tests (no window needed):

    make test
