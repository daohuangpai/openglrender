
vec2 parallaxmap(mat3 TBN,sampler2D parallaxmapimage,vec2 texCoords,vec3 lookatdir,float heightscale)
{
    //以高度为层数标准
    const float minLayers = 10;
    const float maxLayers = 20;
	lookatdir = TBN*lookatdir;
	float level = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), normalize(lookatdir))));  
	//height是最高点相对的凹陷下去的高度，这里求出相对坐标
    float height =  1.0 - texture(parallaxmapimage, texCoords).r;    
    vec2 p =  lookatdir.xy / lookatdir.z * (height * heightscale);//lookatdir.xy / lookatdir.z相当于tan，p是视点方向，而求最远坐标是从看点方向，所以取负数
	//最远的坐标
    vec2 fartexcoords =  texCoords - p; 
	vec2 deltatex = p/level;
    //找到第一个比视线方向的基准高高的点，不管后面的点有没有比这个高的，这个点就是被看到的	
    float delta = heightscale*height/level;
	float lookatdirdepth = heightscale;
	vec2 aftertex = texCoords;	
	vec2 beforetex = texCoords;
	for(int i = 0;i<level;i++)
	{
	   //排除最后一个和最上面一个
	   if(i == level-1){
	      return texCoords;
	   }
	   float texdepth =  heightscale*texture(parallaxmapimage, fartexcoords + i*deltatex).r;	   
	   if(texdepth>=lookatdirdepth){
	    if(i == 0){
		   return fartexcoords;
		 }
		 else{
	       beforetex = fartexcoords + i*deltatex; 
		 }
		 break;
	   } 
	   lookatdirdepth = lookatdirdepth-delta;
	}
	 aftertex = beforetex - deltatex;
	 float heightwidget = texture(parallaxmapimage, beforetex).r/(texture(parallaxmapimage, beforetex).r+texture(parallaxmapimage, aftertex).r);
	 vec2 texret = beforetex*heightwidget + aftertex*(1-heightwidget);
	 //vec2 ret = clamp(aftertex,0.0f,1.0f); 
	 if(texret.x<0)
	 {
	 texret.x = 0;
	 }
	 if(texret.y<0)
	 {
	 texret.y = 0;
	 }
	 return texret;	
}


