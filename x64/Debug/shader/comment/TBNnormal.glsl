
subroutine mat3 ComputerTBN(vec3,vec3,vec3);
subroutine uniform ComputerTBN ComputerTBNModel;

subroutine(ComputerTBN)
mat3 TBNOriginal(vec3 Tangent,vec3 Bitangent,vec3 Normal)
{
     return mat3(Tangent, Bitangent, Normal);
}

subroutine(ComputerTBN)
mat3 TBNnormalization(vec3 Tangent,vec3 Bitangent,vec3 Normal)
{
     Tangent = normalize(Tangent);
	 Normal = normalize(Normal);
	 Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
	 Bitangent = cross(Tangent, Normal); 
     return mat3(Tangent, Bitangent, Normal);
}