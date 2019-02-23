#version 430 core

layout (local_size_x = 1, local_size_y = 1) in;
layout (binding = 0, r32f) uniform image2D terrainHeight;
layout (binding = 1, rgba32f) uniform image2D terrainNormal;
layout (binding = 2, rg32f) uniform image2D terrain2ndDerivative;

uniform float randomCount;
uniform float random[100];
uniform float fWidth;

void main(void)
{
	ivec2 position = ivec2(gl_GlobalInvocationID.xy);
	
	float fDistance1stClosest = 100;
	float fDistance2ndClosest = 100;

	float fDiagonal = sqrt(fWidth * fWidth + fWidth * fWidth) / 5.0f;

	for (int j = 0; j< randomCount; ++j)
	{
		ivec2 var = ivec2(random[j*2], random[j*2+1]);
		float fDistance = length(var - position) / fDiagonal;
				
		for (int i = -1; i < 2; ++i)
		{
			for (int j = -1; j < 2; ++j)
			{
				vec2 difference = var + vec2(i * fWidth, j * fWidth) - position;
				float fDistanceCalculated = length(difference) / fDiagonal;
				fDistance = min(fDistance, fDistanceCalculated);
			}
		}

		if (fDistance < fDistance1stClosest)
		{
			fDistance2ndClosest = fDistance1stClosest;
			fDistance1stClosest = fDistance;
		}
		else if (fDistance < fDistance2ndClosest)
		{
			fDistance2ndClosest = fDistance;
		}
	}
	imageStore(terrainHeight,position,vec4((-fDistance1stClosest +fDistance2ndClosest),0,0,0));
}
