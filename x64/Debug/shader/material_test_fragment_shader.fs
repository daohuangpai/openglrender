#version 440 core
#include "comment/parallaxmap.glsl"
#include "comment/TBNnormal.glsl"
#include "comment/light.glsl"
#include "comment/normalmap.glsl"
uniform sampler2D diffusetex;
uniform sampler2D normalmaptex;
uniform sampler2D speculartex;
uniform sampler2D parallaxtex;
uniform vec3 eyePos;
uniform float heightscale;
uniform bool useparallaxmap; 
in vec3 pos;
in vec2 tex;
in vec3 nor;
in vec3 btg;
in vec3 tgt;
out vec4 color;
void main()
{
 mat3 TBN = ComputerTBNModel(tgt,btg,nor);
 vec2 texreal;
 if(useparallaxmap)
 {
  texreal= parallaxmap(TBN,parallaxtex,tex,eyePos - pos,heightscale);
 }
 else
 {
  texreal = tex;
 }
 vec4 diffusecolor = texture(diffusetex, texreal.xy);
 vec4 normalmapcolor = texture(normalmaptex, texreal.xy);
 vec3 normal = ComputerNormalMap(normalmapcolor.xyz,TBN,texreal);
 
 vec4 ambient = vec4(diffusecolor.xyz,0.3);
 vec4 diffuse = vec4(diffusecolor.xyz,1.0);
 vec4 specular=vec4(diffusecolor.xyz,texture(speculartex, texreal.xy).x);
 Material material = {ambient,diffuse,specular,4.0};
 vec3 lightresult = computerLight(material,pos,normal,eyePos);
 color = vec4(lightresult,1.0);
}