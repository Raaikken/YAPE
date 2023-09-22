#version 460 core

// Output
layout (location = 0) out vec2 textureCordinatesOut;

void main() {
    vec2 vertices[6] = {
        vec2(-0.5,  0.5),
        vec2(-0.5, -0.5),
        vec2( 0.5,  0.5),
        vec2( 0.5,  0.5),
        vec2(-0.5, -0.5),
        vec2( 0.5, -0.5)
    };

    float left = 0.0;
    float top = 0.0;
    float right = 16.0;
    float bottom = 16.0;

    vec2 textureCordinates[6] = {
        vec2(left,  top),
        vec2(left,  bottom),
        vec2(right, top),
        vec2(right, top),
        vec2(left,  bottom),
        vec2(right, bottom),
    };

    gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0);
    textureCordinatesOut = textureCordinates[gl_VertexID];
}