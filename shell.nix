{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
    buildInputs = with pkgs; [
        gdb
        gf
        bear
        clang-tools
        clang
    ];
}
