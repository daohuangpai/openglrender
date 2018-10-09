#version 420
layout(location = 0) in vec3 position;
uniform mat4 model_matrix;
uniform mat4 matixview;
uniform mat4 projection_matrix;

void main(void)
{
    gl_Position = projection_matrix * matixview*(model_matrix * vec4(position,1.0));
}