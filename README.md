# pong
Clon of a classic game using RayLib


### How to compile?

Make sure you have `gnumake` and `raylib` install in your system, if not,
here's a `nix-shell` command for it:

    nix-shell -p gnumake raylib

For other linux installation install `gnumake` and `raylib` in your
system using your prefer package manager, e.g:

    sudo apt install gnumake raylib

Once installed the libs, run this command to compile:

    make game

### How to play it?

Run the game:

    ./src/pong

The keys bindings:

    - Player one: (w and s)

    - Player two: (up and down arrows)

    - Start game: (Enter or m)

    - Pause: (Space Tab)

    - Menu: (when pause press m)

    - Exit: (Esc)
