#pragma once

#include "input.h"
#include "yape_lib.h"
#include "render_interface.h"

constexpr int UPDATES_PER_SECOND = 60;
constexpr double UPDATE_DELAY = 1.0 / UPDATES_PER_SECOND;
constexpr int WORLD_WIDTH = 320;
constexpr int WORLD_HEIGHT = 180;
constexpr int TILESIZE = 8;
constexpr IVec2 WORLD_GRID { WORLD_WIDTH / TILESIZE, WORLD_HEIGHT / TILESIZE };

enum GameInputType {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_JUMP,
    MOVE_DASH,

    MOUSE_PRIMARY,
    MOUSE_SECONDARY,
    MOUSE_MIDDLE,

    GAME_INPUT_COUNT
};

struct KeyMapping {
    Array<GLuint, 3> keys;
};

struct Tile {
    int neighbourMask;
    bool isVisible;
};

struct Player {
    Vec2 position;
    Vec2 prevPosition;
};

struct GameState {
    float updateTimer;
    bool initialized = false;
    Player player;

    Array<IVec2, 21> tileCordinates;
    Tile worldGrid[WORLD_GRID.x][WORLD_GRID.y];
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
    EXPORT_FN void update_game(GameState* gamestateIn, RenderData* renderDataIn, Input* inputIn, float dt);
}