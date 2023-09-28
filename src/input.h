#pragma once

#include <GLFW/glfw3.h>

#include "yape_lib.h"

DVec2 screen_to_world(DVec2);

const int KEY_COUNT = 512;

struct Key {
    bool isDown;
    bool justPressed;
    bool justReleased;

    unsigned char halfTansitionCount;
};

struct Input{
    IVec2 screenSize;
    
    DVec2 prevMousePos;
    DVec2 mousePos;
    DVec2 relMousePos;
    
    DVec2 prevMousePosWorld;
    DVec2 mousePosWorld;
    DVec2 relMousePosWorld;

    Key keys[KEY_COUNT];
};

static Input* input;

void key_callback(GLFWwindow* window, int keyCode, int scancode, int action, int mods) {
    Key* key = &input->keys[keyCode];
    bool isDown = (action == GLFW_REPEAT || action == GLFW_PRESS);
    key->justPressed = !key->justPressed && !key->isDown && isDown;
    key->justReleased = !key->justReleased && key->isDown && isDown;
    key->isDown = isDown;
    key->halfTansitionCount++;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    input->prevMousePos = input->mousePos;

    input->mousePos.x = xpos;
    input->mousePos.y = ypos;
    input->relMousePos = input->mousePos - input->prevMousePos;

    input->mousePosWorld = screen_to_world(input->mousePos);
}

bool key_just_pressed(int keyCode) {
    Key key = input->keys[keyCode];
    bool result = key.isDown && (key.halfTansitionCount == 1 || key.halfTansitionCount > 1);
    return result;
}

bool key_just_released(int keyCode) {
    Key key = input->keys[keyCode];
    bool result = !key.isDown && (key.halfTansitionCount == 1 || key.halfTansitionCount > 1);
    return result;
}

bool key_pressed(int keyCode) {
    return input->keys[keyCode].isDown;
}

void clearKeyCodes() {
    for(int keyCode = 0; keyCode < KEY_COUNT; keyCode++) {
        input->keys[keyCode].justReleased = false;
        input->keys[keyCode].justPressed = false;
        input->keys[keyCode].halfTansitionCount = 0;
    }
}