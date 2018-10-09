vec3 ComputerNormalMap(vec3 NormalMapcolor,mat3 TBN,vec2 TexCoord)
{
     vec3 NormalMap;	 
     NormalMap = 2.0 * NormalMapcolor - vec3(1.0, 1.0, 1.0);   
     NormalMap = TBN * NormalMap;                                                           
	 NormalMap = normalize(NormalMap); 
     return NormalMap; 	 
}


