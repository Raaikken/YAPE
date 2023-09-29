#pragma once

#include "yape_lib.h"
#include "assets.h"
#include "input.h"

// Constants

struct Transform {
	IVec2 atlasOffset;      // top left corner of the sprite in the sprite atlas
	IVec2 spriteSize;       // the size of the sprite in the sprite atlas
	Vec2 position;          // where the sprite is drawn in relation to the centre of the camera
	Vec2 size;              // size of the sprite
};

struct OrthographicCamera2D {
	float zoom = 1.0f;
	Vec2 dimensions;
	Vec2 position;
};

struct RenderData {
	OrthographicCamera2D gameCamera;
	OrthographicCamera2D uiCamera;

    Array<Transform, 1000> transforms;
};

static RenderData* renderData;

/* converts a position on screen to a game world position
 * 
 * function: screen_to_world
 * param: screePosition, position on the screen
 * returns: Vec2
*/
DVec2 screen_to_world(DVec2 screePosition) {
    OrthographicCamera2D camera = renderData->gameCamera;

    float xPos = (float)screePosition.x / (float)input->screenSize.x * camera.dimensions.x;
    xPos += -camera.dimensions.x / 2.0f + camera.position.x;

    float yPos = (float)screePosition.y / (float)input->screenSize.y * camera.dimensions.y;
    yPos += camera.dimensions.y / 2.0f + camera.position.y;

    return { xPos, yPos };
}


/* draws a plain white quad
 * 
 * function: draw_quad
 * param: position, position of the sprite
 *        size, size of the sprite
 * returns: void
*/
void draw_quad(Vec2 position, Vec2 size) {
    Transform transform = {};
    transform.position = position - size / 2.0f;
    transform.size = size;
    transform.atlasOffset = { 0, 0 };
    transform.spriteSize = { 1, 1 };

    renderData->transforms.add(transform);
}

/* draws a quad part of tile set
 * 
 * function: draw_quad
 * param: transform, transform of the tile drawn
 * returns: void
*/
void draw_quad(Transform transform) {
    renderData->transforms.add(transform);
}

/* draws a sprite
 * 
 * function: draw_sprite
 * param: spriteID, The sprite to be drawn
 *        position, position to which the sprite is drawn (centralized)
 * returns: void
*/
void draw_sprite(SpriteID spriteID, Vec2 position) {
	Sprite sprite = get_sprite(spriteID);

	Transform transform = {};
	transform.position = position - IVec2ToVec2(sprite.spriteSize) / 2.0f;
	transform.size = IVec2ToVec2(sprite.spriteSize);
	transform.atlasOffset = sprite.atlasOffset;
	transform.spriteSize = sprite.spriteSize;

    renderData->transforms.add(transform);
}

/* draws a sprite
 * 
 * function: draw_sprite
 * param: spriteID, The sprite to be drawn
 *        position, position to which the sprite is drawn (centralized)
 * returns: void
*/
void draw_sprite(SpriteID spriteID, IVec2 position) {
    draw_sprite(spriteID, IVec2ToVec2(position));
}