#include "game.h"
#include "assets.h"
#include "yape_lib.h"

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

Tile* get_tile(int x, int y) {
    Tile* tile = nullptr;

    if(x >= 0 && x < WORLD_GRID.x && y >= 0 && y < WORLD_GRID.y) {
        tile = &gameState->worldGrid[x][y];
    }

    return tile;
}

Tile* get_tile(IVec2 worldPos) {
    int x = worldPos.x / TILESIZE;
    int y = worldPos.y / TILESIZE;

    return get_tile(x, y);
}

void initialize_game() {
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

        gameState->keyMappings[MOUSE_PRIMARY].keys.add(GLFW_MOUSE_BUTTON_LEFT);
        gameState->keyMappings[MOUSE_SECONDARY].keys.add(GLFW_MOUSE_BUTTON_RIGHT);
        gameState->keyMappings[MOUSE_MIDDLE].keys.add(GLFW_MOUSE_BUTTON_MIDDLE);
    }

    renderData->gameCamera.position.x = 160;
    renderData->gameCamera.position.y = -90;

    {
        IVec2 tilesPosition = { 48, 0 };
        
        for(int y = 0; y < 5; y++) {
            for(int x = 0; x < 4; x++) {
                gameState->tileCordinates.add({tilesPosition.x + x * 8, tilesPosition.y + y * 8});
            }
        }

        gameState->tileCordinates.add({tilesPosition.x, tilesPosition.y + 5 * 8});
    }
}

void update_tileset() {
    int neighbourOffsets[24] = { 0,-1,   -1, 0,    1, 0,    0, 1,
                                -1,-1,    1,-1,   -1, 1,    1, 1,
                                 0,-2,   -2, 0,    2, 0,    0, 2};

    for(int y = 0; y < WORLD_GRID.y; y++) {
        for(int x = 0; x < WORLD_GRID.x; x++) {
            Tile* tile = get_tile(x, y);

            if(!tile->isVisible) {
                continue;
            }

            tile->neighbourMask = 0;
            int neighbourCount = 0;
            int extendedNeighbourCount = 0;
            int emptyNeighbourSlot = 0;

            for(int n = 0; n < 12; n++) {
                Tile* neighbour = get_tile(x + neighbourOffsets[n * 2], y + neighbourOffsets[n * 2 + 1]);

                if(!neighbour || neighbour->isVisible) {
                    tile->neighbourMask |= BIT(n);
                    if(n < 8) {
                        neighbourCount++;
                    }
                    else {
                        extendedNeighbourCount++;
                    }
                }
                else if(n < 8) {
                    emptyNeighbourSlot = n;
                }
            }

            if(neighbourCount == 7 && emptyNeighbourSlot >= 4) {
                tile->neighbourMask = 16 + (emptyNeighbourSlot - 4);
            }
            else if(neighbourCount == 8 && extendedNeighbourCount == 4) {
                tile->neighbourMask = 20;
            }
            else {
                tile->neighbourMask = tile->neighbourMask & 0b1111;
            }
        }
    }
}

void simulate() {
    int speed = 2;
    Vec2 direction;

    gameState->player.prevPosition = gameState->player.position;

    if(is_down(MOVE_LEFT)) {
        //gameState->playerPos.x -= speed;
        direction.x = -speed;
    }
    if(is_down(MOVE_RIGHT)) {
        //gameState->playerPos.x += speed;
        direction.x = speed;
    }
    if(is_down(MOVE_UP)) {
        //gameState->playerPos.y -= speed;
        direction.y = -speed;
    }
    if(is_down(MOVE_DOWN)) {
        //gameState->playerPos.y += speed;
        direction.y = speed;
    }

    gameState->player.position += direction;

    // float posX = input->mousePosWorld.x;
    // float posY = input->mousePosWorld.y;

    // gameState->player.position = { posX, posY };
}


EXPORT_FN void update_game(GameState* gameStateIn, RenderData* renderDataIn, Input* inputIn, float dt) {
	if(renderData != renderDataIn) {
		gameState = gameStateIn;
		renderData = renderDataIn;
		input = inputIn;
	}

	if(!gameState->initialized) {
		initialize_game();
	}

    bool updateTiles = false;
    if(is_down(MOUSE_PRIMARY)) {
        IVec2 mousePosWorld = DVec2ToIVec2(input->mousePosWorld);
        Tile* tile = get_tile(mousePosWorld);

        if(tile) {
            tile->isVisible = true;
            updateTiles = true;
        }
    }

    if(is_down(MOUSE_SECONDARY)) {
        IVec2 mousePosWorld = DVec2ToIVec2(input->mousePosWorld);
        Tile* tile = get_tile(mousePosWorld);

        if(tile) {
            tile->isVisible = false;
            updateTiles = true;
        }
    }

    if(updateTiles) {
        update_tileset();
    }

    {
        gameState->updateTimer += dt;
        while(gameState->updateTimer >= UPDATE_DELAY) {
            gameState->updateTimer -= UPDATE_DELAY;
            simulate();

            input->relMousePos = input->mousePos - input->prevMousePos;
            input->prevMousePos = input->mousePos;

            clearKeyCodes();
        }
    }

    float interpolatedDT = (float)(gameState->updateTimer / UPDATE_DELAY);

    for(int y = 0; y < WORLD_GRID.y; y++) {
        for(int x = 0; x < WORLD_GRID.x; x++) {
            Tile* tile = get_tile(x, y);

            if(!tile->isVisible) {
                continue;
            }

            Transform transform = {};
            transform.position = { x * (float)TILESIZE, y * (float)TILESIZE };
            transform.size = { 8, 8 };
            transform.spriteSize = { 8, 8 };
            transform.atlasOffset = gameState->tileCordinates[tile->neighbourMask];
            draw_quad(transform);
        }
    }

    Player& player = gameState->player;
    Vec2 playerPos = lerp(player.prevPosition, player.position, interpolatedDT);
	
    draw_sprite(SPRITE_ORANGE, playerPos);
}