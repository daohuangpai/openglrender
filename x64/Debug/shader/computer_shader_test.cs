#version 440 core
layout (local_size_x = 1024,local_size_y = 1024) in;

layout(binding = 0) uniform readonly image2D image;
layout(binding = 1) uniform writeonly image2D outimage;

void main(void)
{
	//ivec2 vecSize = textureSize(tex);
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	
	if(pos.x != 0){
	  ivec2 backpos = ivec2(pos.x-1,pos.y);
      vec3 result = imageLoad(input_image, pos)-imageLoad(input_image, backpos);
      imageStore(output_image, pos.xy, result);
	  }
}
