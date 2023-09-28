#pragma once

#include "input.h"
#include "yape_lib.h"
#include "render_interface.h"

enum GameInputType {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_JUMP,
    MOVE_DASH,

    GAME_INPUT_COUNT
};

struct KeyMapping {
    Array<GLuint, 3> keys;
};

struct GameState {
    bool initialized = false;
    Vec2 playerPos;

    KeyMapping keyMappings[GAME_INPUT_COUNT];
};

static GameState* gameState;

extern "C" {
    /* update the game logic
    * 
    * function: update_game
    * param: gameStateIn, ptr to the current game state
    *        renderDataIn, ptr to struct which holds rendering information
    *        inputIn, ptr to the input system
    * returns: void
    */
    EXPORT_FN void update_game(GameState* gamestateIn, RenderData* renderDataIn, Input* inputIn);
}