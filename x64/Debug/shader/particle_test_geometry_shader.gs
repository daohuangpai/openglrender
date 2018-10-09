#version 440 core
in vec3 pointcolor;
out vec4 color;
void main()
{
color = vec4(pointcolor,1.0);
}