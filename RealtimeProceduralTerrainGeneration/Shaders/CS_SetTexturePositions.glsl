#version 430 core

layout (local_size_x = 1, local_size_y = 1) in;
layout (binding = 0, r32f) uniform image2D terrainHeight;
layout (binding = 1, rgba32f) uniform image2D terrainNormal;
layout (binding = 2, rgba32f) uniform image2D terrain2ndDerivative;
layout (binding = 3, r32f) uniform image2D terrainTexture;

uniform float randomCount;
uniform float random[100];
uniform float fTextureCount;

void main(void)
{
	ivec2 position = ivec2(gl_GlobalInvocationID.xy);
	float fHeight = imageLoad(terrainHeight, position).x;
	float fSteer = 1.0f - imageLoad(terrainNormal, position).z;
	vec2 vErosion = imageLoad(terrain2ndDerivative, position).xy;

	/*fHeight += mod(random[int(mod(position.x*position.y,randomCount))] , 10)/100.0f;
	fSteer += mod(random[int(mod(position.x*position.y,randomCount))] , 10)/100.0f;
	fHeight += mod(random[int(mod(position.x*position.y,randomCount))] , 10)/100.0f;
	*/

	vec3 textureVector = vec3(fHeight, fSteer, length(vErosion));

	//find out which textureGroup should be used

	int materialCount[8];
	materialCount[0] = 2;//4; //gras
	materialCount[1] = 2;//4; //leaves
	materialCount[2] = 1;//3; //litter
	materialCount[3] = 1;//1; //moss
	materialCount[4] = 2;//4; //mud
	materialCount[5] = 3;//3; //road
	materialCount[6] = 3;//8; //rock
	materialCount[7] = 1;//1; //snow

	vec3 materialCondition[8];
	materialCondition[0] = vec3(0.3, 0.1, 0.1);	//gras
	materialCondition[1] = vec3(0.2, 0.05, 0.5);	//leaves
	materialCondition[2] = vec3(0, 0, 0);	//litter
	materialCondition[3] = vec3(0.4, 0.2, 0.05);	//moss
	materialCondition[4] = vec3(0.3, 0.15, 0.3);	//mud
	materialCondition[5] = vec3(0.25, 0.05, 0.15);	//road
	materialCondition[6] = vec3(0.7, 0.2, 0.5);	//rock
	materialCondition[7] = vec3(0.8, 0.1, 0.05);	//snow

	int materialIndex = 0;
	float smallestDistance = 10;
	for(int i = 0; i< 8; ++i)
	{
		float distance = length(materialCondition[i] - textureVector);
		if(distance < smallestDistance)
		{
			smallestDistance = distance;
			materialIndex = i;
		}
	}

	int countMaterials = materialCount[materialIndex];
	int startingIndex = 0;
	for(int i = 0; i< materialIndex; i++)
	{
		startingIndex += materialCount[i];
	}

	int randomMaterial = int(startingIndex + mod(random[int(mod(position.x * position.y, randomCount))], countMaterials));
	imageStore(terrainTexture,position,ivec4(randomMaterial,0,0,0));
}
