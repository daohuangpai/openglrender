#version 440 core
layout (location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in float pointsize;

uniform mat4 model_matrix;
uniform mat4 projection_matrix;

out vec3 pointcolor;
void main()
{
gl_Position = projection_matrix * (model_matrix *vec4(position, 1.0));
pointcolor = color;
gl_PointSize = pointsize;
}