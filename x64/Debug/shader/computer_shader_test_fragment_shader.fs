#version 420 core
in vec2 tex;
uniform sampler2D skybox;
out vec4 color;
void main()
{
color = texture2D(skybox,tex);
}