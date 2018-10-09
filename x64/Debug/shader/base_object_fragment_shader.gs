#version 420 core
uniform vec4 vertexColor; 
out vec4 color;
uniform sampler2D skybox;
in float num;
in vec2 TextCoord;

void main()
{
//float f = float(num)*0.2;
//color = vec4(f,f,0.0,1.0);
color = texture2D(skybox,TextCoord);
}