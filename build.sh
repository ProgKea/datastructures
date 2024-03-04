#!/bin/sh

mkdir -p ./build

gcc -E main.c > ./build/macro_expansion.c
gcc -ggdb3 -Wextra -Wall -o ./build/main main.c
