#version 440
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 TextCoord;

uniform mat4 model_matrix;
uniform mat4 projection_matrix;

out vec2 tex;
void main(void)
{
    vec4 pos = projection_matrix * (model_matrix * position);
    gl_Position = pos;
	tex = TextCoord;
}