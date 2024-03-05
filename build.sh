#!/bin/sh

mkdir -p ./build

clang -E main.c > ./build/macro_expansion.c
clang -ggdb3 -Wextra -Wall -o ./build/main main.c
