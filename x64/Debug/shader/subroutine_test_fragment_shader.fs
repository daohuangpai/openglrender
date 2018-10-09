#version 420 core
out vec4 color;
#include "comment/normalmap.glsl"

subroutine vec4 veccolor();
subroutine uniform veccolor veccolorModel;
uniform float ppp;
subroutine float veccolorscale();
subroutine uniform veccolorscale veccolorscaleModel;

subroutine(veccolor)
vec4 vecRcolor()
{
  return vec4(1.0,0.0f,0.0f,1.0f);
}

subroutine(veccolor)
vec4 vecGcolor()
{
  return vec4(0.0,1.0f,0.0f,1.0f);
}

subroutine(veccolor)
vec4 vecBcolor()
{
  return vec4(0.0,0.0f,1.0f,1.0f);
}


subroutine(veccolorscale)
float veccolorRscale()
{
  return 0.9f;
}

subroutine(veccolorscale)
float veccolorGscale()
{
  return 0.7f;
}

subroutine(veccolorscale)
float veccolorBcale()
{
  return 0.5f;
}


void main()
{
  float a = veccolorscaleModel();
  color = a*veccolorModel()*vec4(ComputerNormalMapModel(vec3(1.0,1.0,1.0),vec3(1.0,1.0,1.0),vec3(1.0,1.0,1.0),vec3(1.0,1.0,1.0),vec2(1.0,1.0)),1.0);
}