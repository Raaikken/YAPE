#!usr/bin/env bash

timestamp=$(date +%s)

libs="-ldl -lstdc++ -lm -lglfw3 -lGL"

gcc -Wall -g -oYape src/main.cpp src/glad.c $libs

rm -f game_*
gcc -Wall -g "src/game.cpp" -shared -o game_$timestamp
mv game_$timestamp game