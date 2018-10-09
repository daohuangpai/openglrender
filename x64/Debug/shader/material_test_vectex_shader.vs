#version 440
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture;
layout(location = 3) in vec3 bitangents;
layout(location = 4) in vec3 tangents;
uniform mat4 model_matrix;
uniform mat4 matixview;
uniform mat4 projection_matrix;

out vec2 tex;
out vec3 pos;
out vec3 nor;
out vec3 btg;
out vec3 tgt;
void main(void)
{
    gl_Position = projection_matrix * matixview*(model_matrix * vec4(position,1.0));
	pos = (model_matrix*vec4(position,1.0)).xyz;
	tex = texture;   
	nor = (model_matrix*vec4(normal,1.0)).xyz;
	btg = (model_matrix*vec4(bitangents,1.0)).xyz;
	tgt = (model_matrix*vec4(tangents,1.0)).xyz;
}