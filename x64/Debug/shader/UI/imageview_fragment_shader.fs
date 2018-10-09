#version 440 core
in vec2 tex;
uniform sampler2D textureview;
out vec4 color;
void main()
{
color = texture2D(textureview,tex);
}