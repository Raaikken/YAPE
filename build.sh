#!/bin/bash

clang++ -g -oYape src/main.cpp src/glad.c -lstdc++ -lm -lglfw3 -lGL
