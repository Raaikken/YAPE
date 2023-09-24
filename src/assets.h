#pragma once

#include "yape_lib.h"

// ########## //
// Constansts //
// ########## //

// ####### //
// Structs //
// ####### //
enum SpriteID {
    SPRITE_DICE,

    SPRITE_COUNT
};

struct Sprite {
    IVec2 atlasOffset;
    IVec2 spriteSize;
};

Sprite get_sprite(SpriteID spriteID) {
    Sprite sprite = {};

    switch (spriteID) {
    case SPRITE_DICE: {
        sprite.atlasOffset = {0, 0};
        sprite.spriteSize = {16, 16};
    }
    
    default:
        break;
    }

    return sprite;
}