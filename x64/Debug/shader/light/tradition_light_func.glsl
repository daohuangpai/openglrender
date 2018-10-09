
struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

vec4 tradition_light_color(vec3 pointpostion,in vec3 normal,vec3 eyePos,vec4 pointcolor,Material material)
{
   for(uint i = 0u; i < 8u; ++i)
   {
     vec3 lightdir = normalize(Global.lights[i].m_position - pointpostion);
     float diffuseStrength = max(dot(lightdir,normal),0);
     vec3 reflectv = reflect(-lightdir,normal);
     vec3 eyedir = normalize(eyePos-pointpostion);
       float reflectStrength = pow(max(dot(eyedir,reflectv),0),4);
       vec3 lightresult = (ambientStrength+diffuseStrength+reflectStrength)*lightColor;
       color = vec4(lightresult,1.0)*texture(skybox, tex);
   }
}