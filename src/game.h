#pragma once

#include "input.h"
#include "yape_lib.h"
#include "render_interface.h"

extern "C" {
    EXPORT_FN void update_game(RenderData* renderData, Input* input);
}