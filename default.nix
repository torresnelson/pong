with (import <nixpkgs> {});
let
  raylibStorePath = lib.getLib raylib;
  gnumakeStorePath = lib.getLib gnumake;
in
stdenv.mkDerivation {
  name = "pong-env";
  buildInputs = [
    # System requirements.
    raylib
    gnumake
  ];
  src = null;
  shellHook = ''

    RAYLIB_PATH="${raylibStorePath}/lib"
    GNUMAKE_PATH="${gnumakeStorePath}/lib"

    PATH="$PATH:$RAYLIB_PATH"
    PATH="$PATH:$GNUMAKE_PATH"

    export PATH

    # Compile game
    # make game

  '';
}
