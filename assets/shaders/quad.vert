#version 430 core

struct Transform {
    ivec2 atlasOffset;
    ivec2 spriteSize;
    vec2 position;
    vec2 size;
};

// Input
layout (std430, binding = 0) buffer TransformSBO {
    Transform transforms[];
};

uniform vec2 screenSize;

// Output
layout (location = 0) out vec2 textureCordinatesOut;

void main() {
    Transform transform = transforms[gl_InstanceID];

    vec2 vertices[6] = {
        transform.position,
        vec2(transform.position + vec2(0.0, transform.size.y)),
        vec2(transform.position + vec2(transform.size.x, 0.0)),
        vec2(transform.position + vec2(transform.size.x, 0.0)),
        vec2(transform.position + vec2(0.0, transform.size.y)),
        transform.position + transform.size
    };

    float left = transform.atlasOffset.x;
    float top = transform.atlasOffset.y;
    float right = transform.atlasOffset.x + transform.spriteSize.x;
    float bottom = transform.atlasOffset.y + transform.spriteSize.y;

    vec2 textureCordinates[6] = {
        vec2(left,  top),
        vec2(left,  bottom),
        vec2(right, top),
        vec2(right, top),
        vec2(left,  bottom),
        vec2(right, bottom),
    };

    {
        vec2 vertexPosition = vertices[gl_VertexID];
        vertexPosition.y = -vertexPosition.y + screenSize.y;
        vertexPosition = 2.0 * (vertexPosition / screenSize) - 1.0;
        gl_Position = vec4(vertexPosition, 1.0, 1.0);
    }

    textureCordinatesOut = textureCordinates[gl_VertexID];
}