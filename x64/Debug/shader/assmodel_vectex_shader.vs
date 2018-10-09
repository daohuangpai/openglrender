#version 440
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texturecoord;
layout (location = 6) in int BoneID;
uniform mat4 model_matrix;
uniform mat4 matixview;
uniform mat4 projection_matrix;

const int MAX_BONE_SIZE = 63;
struct bonetable
{
     int size;
	 int matixid[MAX_BONE_SIZE];
	 float matixwieght[MAX_BONE_SIZE];
};

layout(std140,binding = 0) buffer matixbuffer
{
	mat4 matixbuffers[];
};

layout(std140,binding = 1) buffer bonetablebuffer
{
	bonetable bonetablebuffers[];
};

out vec2 TextCoord;
void main(void)
{
    mat4 tranmat = projection_matrix * matixview* model_matrix;

    gl_Position =tranmat*vec4(position,1.0);
	TextCoord = texturecoord;
}