#version 420 core
in vec3 tex;
uniform samplerCube skybox;

uniform vec3 lightColor;
uniform float ambientStrength;
uniform vec3 lightPos;
uniform vec3 eyePos;

out vec4 color;
in vec3 pos;
in vec3 nor;
void main()
{
vec3 ambient = ambientStrength * lightColor;
vec3 dir = normalize(lightPos - pos);
float diffuseStrength = max(dot(dir,nor),0);
vec3 reflectv = reflect(-dir,nor);
vec3 eyedir = normalize(eyePos-pos);
float reflectStrength = pow(max(dot(eyedir,reflectv),0),4);
vec3 lightresult = (ambientStrength+diffuseStrength+reflectStrength)*lightColor;
color = vec4(lightresult,1.0)*texture(skybox, tex);
}