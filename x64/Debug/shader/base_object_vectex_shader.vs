#version 420 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex;
layout (binding = 0, offset = 0) uniform atomic_uint atRed;
out vec2 TextCoord;
out mat4 modelview;
out float num;
void main()
{
gl_Position = vec4(position, 1.0);
TextCoord = tex;
uint n  = atomicCounterDecrement(atRed);
num = float(n);
}