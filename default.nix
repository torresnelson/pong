{ pkgs ? import <nixpkgs> { } }:

pkgs.mkShell {
  packages = with pkgs; [
    gnumake
    pkg-config
    raylib
  ];
}
