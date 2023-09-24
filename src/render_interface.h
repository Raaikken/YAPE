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

struct RenderData {
    int transformCount;
    Transform transforms[MAX_TRANSFORMS];
};

static RenderData renderData;

void draw_sprite(SpriteID spriteID, Vec2 position, Vec2 size) {
    Sprite sprite = get_sprite(spriteID);

    Transform transform = {};
    transform.position = position;
    transform.size = size;
    transform.atlasOffset = sprite.atlasOffset;
    transform.spriteSize = sprite.spriteSize;

    renderData.transforms[renderData.transformCount++] = transform;
}