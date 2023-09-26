#pragma once

#include "input.h"
#include "yape_lib.h"
#include "render_interface.h"

struct GameState {
    bool initialized = false;
    IVec2 playerPos;
};

static GameState* gameState;

extern "C" {
    EXPORT_FN void update_game(GameState* gamestateIn, RenderData* renderDataIn, Input* inputIn);
}