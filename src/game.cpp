#include "game.h"
#include "assets.h"
#include "yape_lib.h"

constexpr int WORLD_WIDTH = 320;
constexpr int WORLD_HEIGHT = 180;
constexpr int TILESIZE = 8;

bool just_pressed(GameInputType type) {
    KeyMapping mapping = gameState->keyMappings[type];
    for(int i = 0; i < mapping.keys.count; i++) {
        if(input->keys[mapping.keys[i]].justPressed) {
            return true;
        }
    }

    return false;
}

bool is_down(GameInputType type) {
    KeyMapping mapping = gameState->keyMappings[type];
    for(int i = 0; i < mapping.keys.count; i++) {
        if(input->keys[mapping.keys[i]].isDown) {
            return true;
        }
    }

    return false;
}

void initialize_game() {

}


EXPORT_FN void update_game(GameState* gameStateIn, RenderData* renderDataIn, Input* inputIn) {
	if(renderData != renderDataIn) {
		gameState = gameStateIn;
		renderData = renderDataIn;
		input = inputIn;
	}

	if(!gameState->initialized) {
		renderData->gameCamera.dimensions = { WORLD_WIDTH, WORLD_HEIGHT };
		gameState->initialized = true;

        {
            gameState->keyMappings[MOVE_UP].keys.add(GLFW_KEY_W);
            gameState->keyMappings[MOVE_UP].keys.add(GLFW_KEY_UP);
            gameState->keyMappings[MOVE_LEFT].keys.add(GLFW_KEY_A);
            gameState->keyMappings[MOVE_LEFT].keys.add(GLFW_KEY_LEFT);
            gameState->keyMappings[MOVE_DOWN].keys.add(GLFW_KEY_S);
            gameState->keyMappings[MOVE_DOWN].keys.add(GLFW_KEY_DOWN);
            gameState->keyMappings[MOVE_RIGHT].keys.add(GLFW_KEY_D);
            gameState->keyMappings[MOVE_RIGHT].keys.add(GLFW_KEY_RIGHT);
        }
	}

    int speed = 1;

    if(is_down(MOVE_LEFT)) {
        gameState->playerPos.x -= speed;
    }
    if(is_down(MOVE_RIGHT)) {
        gameState->playerPos.x += speed;
    }
    if(is_down(MOVE_UP)) {
        gameState->playerPos.y -= speed;
    }
    if(is_down(MOVE_DOWN)) {
        gameState->playerPos.y += speed;
    }

    // float posX = input->mousePosWorld.x;
    // float posY = input->mousePosWorld.y;

    // gameState->playerPos = { posX, posY };
	
    draw_sprite(SPRITE_DICE, gameState->playerPos);
}