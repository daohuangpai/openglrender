struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};
#define DIFFUSE_INDEX 0
#define AMBIENT_INDEX 1
#define SPECULAR_INDEX 2
#define SHININESS_INDEX 3
uniform sampler2DArray Materialrray;