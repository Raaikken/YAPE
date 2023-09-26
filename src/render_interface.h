#pragma once

#include "yape_lib.h"
#include "assets.h"

// Constants
constexpr int MAX_TRANSFORMS = 1000;

// Structs
struct Transform {
	IVec2 atlasOffset;
	IVec2 spriteSize;
	Vec2 position;
	Vec2 size;
};

struct OrthographicCamera2D {
	float zoom = 1.0f;
	Vec2 dimensions;
	Vec2 position;
};

struct RenderData {
	OrthographicCamera2D gameCamera;
	OrthographicCamera2D uiCamera;

	int transformCount;
	Transform transforms[MAX_TRANSFORMS];
};

static RenderData* renderData;

void draw_sprite(SpriteID spriteID, Vec2 position) {
	Sprite sprite = get_sprite(spriteID);

	Transform transform = {};
	transform.position = position - IVec2ToVec2(sprite.spriteSize) / 2.0f;
	transform.size = IVec2ToVec2(sprite.spriteSize);
	transform.atlasOffset = sprite.atlasOffset;
	transform.spriteSize = sprite.spriteSize;

	renderData->transforms[renderData->transformCount++] = transform;
}