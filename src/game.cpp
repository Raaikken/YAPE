#include "game.h"
#include "assets.h"
#include "yape_lib.h"

constexpr int WORLD_WIDTH = 320;
constexpr int WORLD_HEIGHT = 180;
constexpr int TILESIZE = 8;

EXPORT_FN void update_game(GameState* gamestateIn, RenderData* renderDataIn, Input* inputIn) {
	if(renderData != renderDataIn) {
		gameState = gamestateIn;
		renderData = renderDataIn;
		input = inputIn;
	}

	if(!gameState->initialized) {
		renderData->gameCamera.dimensions = { WORLD_WIDTH, WORLD_HEIGHT };
		gameState->initialized = true;
	}

	draw_sprite(SPRITE_DICE, {0.0f, 0.0f});
} 