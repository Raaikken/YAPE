#pragma once

#include "yape_lib.h"

// ########## //
// Constansts //
// ########## //

// ####### //
// Structs //
// ####### //
enum SpriteID {
    SPRITE_WHITE,
    SPRITE_ORANGE,

    SPRITE_COUNT
};

struct Sprite {
    IVec2 atlasOffset;
    IVec2 spriteSize;
};

Sprite get_sprite(SpriteID spriteID) {
    Sprite sprite = {};

    switch (spriteID) {
    case SPRITE_WHITE: {
        sprite.atlasOffset = {0, 0};
        sprite.spriteSize = {1, 1};
        break;
    }
    case SPRITE_ORANGE: {
        sprite.atlasOffset = {16, 0};
        sprite.spriteSize = {16, 16};
        break;
    }
    default:
        break;
    }

    return sprite;
}