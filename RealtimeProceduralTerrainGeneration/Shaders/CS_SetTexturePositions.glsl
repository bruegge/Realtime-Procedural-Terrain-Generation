#version 430 core

layout (local_size_x = 1, local_size_y = 1) in;
layout (binding = 0, r32f) uniform image2D terrainHeight;
layout (binding = 1, rgba32f) uniform image2D terrainNormal;
layout (binding = 2, rgba32f) uniform image2D terrain2ndDerivative;
layout (binding = 3, r32f) uniform image2D terrainTexture;

uniform float randomCount;
uniform float random[100];
uniform float fTextureCount;

int PickTexture(float valueLow, float valueHigh, float value, int materialNumberLow, int materialNumberHigh)
{
	valueHigh -= valueLow;
	value -= valueLow;
	int numberMaterials = materialNumberHigh - materialNumberLow + 1;
	float materialValueStepSize = valueHigh / numberMaterials;

	return int(value / materialValueStepSize)+ materialNumberLow;
}

void main(void)
{
	ivec2 position = ivec2(gl_GlobalInvocationID.xy);
	float fHeight = imageLoad(terrainHeight, position).x;
	float fSteer = 1.0f - imageLoad(terrainNormal, position).z;
	vec2 vErosion = imageLoad(terrain2ndDerivative, position).ba;

	fHeight = mod(random[int(mod(position.x * position.y, randomCount))], 100) / 1000.0f + fHeight;

	//find out which textureGroup should be used

	int materialCount[8];
	materialCount[0] = 4; //gras
	materialCount[1] = 4; //leaves
	materialCount[2] = 3; //litter
	materialCount[3] = 1; //moss
	materialCount[4] = 4; //mud
	materialCount[5] = 3; //road
	materialCount[6] = 8; //rock
	materialCount[7] = 1; //snow

	int randomMaterial = 0;
	if(vErosion.x > 0.03f || vErosion.y > 0.03f) // erosion in the valley
	{
		float value = max(vErosion.x,vErosion.y);
		randomMaterial = PickTexture(0.00f, 0.8f, fHeight, 12, 15);
	}
	else //usual erosion
	{
		if(fHeight <0.4f)
		{
			randomMaterial = PickTexture(0.0f, 0.4f, fHeight, 0, 4);
		}
		else if(fHeight < 0.9f) //rocks
		{
			randomMaterial = PickTexture(0.4f, 0.9f, fHeight, 18, 23);
		}
		else //snow
		{
			randomMaterial = 26;
		}
	}
	
	imageStore(terrainTexture,position,ivec4(randomMaterial,0,0,0));
}
