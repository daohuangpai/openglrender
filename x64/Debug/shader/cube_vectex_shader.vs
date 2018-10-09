#version 420
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 texture;
uniform mat4 model_matrix;
uniform mat4 projection_matrix;

out vec3 tex;
void main(void)
{
    gl_Position = projection_matrix * (model_matrix * vec4(position,1.0));
	tex = texture;   
}