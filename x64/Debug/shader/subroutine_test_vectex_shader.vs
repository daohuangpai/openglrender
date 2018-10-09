#version 420 core
layout (location = 0) in vec3 position;
layout(location = 1) in vec2 color;
uniform mat4 model_matrix;
uniform mat4 projection_matrix;

out vec4 vertexColor;
void main()
{
gl_Position = vec4(position, 1.0);
//gl_PointSize = (1.0 - position.x / position.y) * 64.0;
vertexColor = vec4(color,0.0,1.0);
}