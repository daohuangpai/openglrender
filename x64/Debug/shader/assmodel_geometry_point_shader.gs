#version 440 core
uniform sampler2D texturepic;
in vec2 TextCoord;
out vec4 color;
void main()
{
color = texture(texturepic,TextCoord.xy);//vec4(1.0,0.0f,0.0f,1.0f);
}