#version 420
#include "../comment/comment.glsl"
#include "../comment/Material.glsl"
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
void main(void)
{
    gl_Position = projection* prevViewProjection *(viewProjection * vec4(position,1.0));
}

