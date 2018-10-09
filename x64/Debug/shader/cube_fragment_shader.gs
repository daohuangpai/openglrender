#version 420 core
in vec3 tex;
uniform samplerCube skybox;
out vec4 color;
void main()
{
color = texture(skybox, tex);
}