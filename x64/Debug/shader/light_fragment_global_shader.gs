#version 440 core
#include "comment/light.glsl"
uniform samplerCube skybox;
uniform vec3 eyePos;
in vec3 pos;
in vec3 tex;
in vec3 nor;
out vec4 color;
void main()
{
 vec4 texcolor = texture(skybox, tex);
 vec4 ambient = vec4(texcolor.xyz,0.1);
 vec4 diffuse = vec4(texcolor.xyz,0.2);;
 vec4 specular=vec4(texcolor.xyz,0.3);;
 Material material = {texcolor,diffuse,specular,4.0};
 vec3 lightresult = computerLight(material,pos,nor,eyePos);
 color = vec4(lightresult,1.0);
}