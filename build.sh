#!usr/bin/env bash

timestamp=$(date +%s)

libs="-ldl -lstdc++ -lm -lglfw3 -lGL"

gcc -g -oYape src/main.cpp src/glad.c $libs

rm -f game_*
gcc -g "src/game.cpp" -shared -o game_$timestamp.so
mv game_$timestamp.so game.so