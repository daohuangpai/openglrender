#version 440
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 texture;
uniform mat4 model_matrix;
uniform mat4 projection_matrix;

out vec3 tex;
out vec3 pos;
out vec3 nor;
void main(void)
{
    gl_Position = projection_matrix * (model_matrix * vec4(position,1.0));
	pos = position;
	tex = texture;   
	nor = normal;
}