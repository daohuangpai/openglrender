#include "Material.glsl"
struct light
{
    int enable;//关闭该光源	
	int directionlight;//表示是否使用spot光源
	int spotlight;//表示是否使用spot光源	
	float m_spot_exponent;//表示聚光的程度，为零时表示光照范围内向各方向发射的光线强度相同，为正数时表示光照向中央集中，正对发射方向的位置受到更多光照，其它位置受到较少光照。数值越大，聚光效果就越明显。
    float m_spot_cutoff;//表示一个角度，它是光源发射光线所覆盖角度的一半，其取值范围在0到90之间，也可以取180这个特殊值。取值为180时表示光源发射光线覆盖360度，即不使用聚光灯，向全周围发射。即一个点光源。
	
	vec3 m_spot_direction;  //方向光源的方向，光源的默认方向是(0.0,0.0,0.0),即不使用
	vec3 m_position;//光线的位置
	vec3 attenuation;//衰减因子 = 1 // (attenuation[0] + attenuation[1] * d + attenuation[2]*attenuation[2] * d)
	vec3 m_ambientlightcolor;//环境光，前三个颜色
	vec3 m_diffuselightcolor;//漫反射，前三个颜色
	vec3 m_specularlightcolor;//反射光，前三个颜色
};

layout(std140,binding = 1) buffer lightbuffer
{
	light lightmodelbuffer[];
};

//subroutine vec4 lightmodel(light lightsource,Material material,vec4 vectexcolor,vec3 vectexposition,vec3 vectexnormal,vec3 eyeposition);
//subroutine uniform lightmodel lightmodelfun;


//subroutine(lightmodel)
vec3 GouraudLightModel(light lightsource,Material material,vec3 vectexposition,vec3 vectexnormal,vec3 eyeposition)
{  
     	
   if(lightsource.enable==0){
      return vec3(0.0,0.0,0.0);
   }
   
   float lightd = sqrt(dot((lightsource.m_position - vectexposition),(lightsource.m_position - vectexposition)));
   float Attenuationfactor = 1.0 / ((lightsource.attenuation[0] + lightsource.attenuation[1] * lightd + lightsource.attenuation[2]*lightsource.attenuation[2] * lightd));
   //衰减太多则丢弃节省计算
   if(Attenuationfactor<0.001){
       return vec3(0.0,0.0,0.0);
    }
   
   //计算光线方向向量
   vec3 lightdir;
   if(lightsource.directionlight>0){
      lightdir = normalize(lightsource.m_spot_direction);
   }
   else{
	 lightdir = normalize(vectexposition - lightsource.m_position);
   }
	
	//漫反射光强度
   float diffuseStrength = max(dot(-lightdir,vectexnormal),0.0)*material.diffuse.w;
    
   //spotlight 抛弃检测	
   float spotstrength = 1.0;
   float minspotstrength = 0.0;
   if(lightsource.spotlight>0)
   {
     //如果在光线背面则不被照到直接返回
      if(diffuseStrength == 0.0)
	     return vec3(0.0,0.0,0.0);
      vec3 vldir = normalize(vectexposition - lightsource.m_position);
	  spotstrength = dot(vldir,lightdir);
	  minspotstrength = cos(lightsource.m_spot_cutoff/360.0);
	  //表示在聚光灯外了
	  if(minspotstrength>spotstrength){
	     return vec3(0.0,0.0,0.0);
	  }
   }
   

   //反射光强度
   vec3 reflectv = reflect(lightdir,vectexnormal);
   vec3 eyedir = normalize(eyeposition-vectexposition);
   float reflectStrength = pow(max(dot(eyedir,reflectv),0.0),material.shininess)*material.specular.w;;
   //
   vec3 result;
   vec3 ambientlightcolor = lightsource.m_ambientlightcolor*material.ambient.xyz*material.ambient.w;
   vec3 diffuselightcolor = lightsource.m_diffuselightcolor*material.diffuse.xyz*diffuseStrength;
   vec3 reflectlightcolor = lightsource.m_specularlightcolor*material.specular.xyz*reflectStrength;
   vec3 lightresult; 
   if(lightsource.spotlight>0)
   {
	  if(minspotstrength>0.99){
	    lightresult =  Attenuationfactor*(ambientlightcolor + diffuselightcolor + reflectlightcolor);
	  }else{
	     float spotfactor =1.0-(1.0-spotstrength)*(1.0/(1.0-minspotstrength));
		 lightresult = spotfactor*Attenuationfactor*(ambientlightcolor + diffuselightcolor + reflectlightcolor);
	  }
   }
   else
   {
     lightresult = Attenuationfactor*(ambientlightcolor + diffuselightcolor + reflectlightcolor);
   }
   return lightresult;
}

vec3 computerLight(Material material,vec3 vectexposition,vec3 vectexnormal,vec3 eyeposition)
{
    vec3 result = vec3(0.0,0.0,0.0);
	int length = lightmodelbuffer.length();
	for(int i = 0;i<length;i++){
	   result = result + GouraudLightModel(lightmodelbuffer[i],material,vectexposition,vectexnormal,eyeposition);
	}
	return result;
}

