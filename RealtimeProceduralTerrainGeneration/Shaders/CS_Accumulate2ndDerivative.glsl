#version 430 core

layout (local_size_x = 1, local_size_y = 1) in;
layout (binding = 0, r32f) uniform image2D terrainHeight;
layout (binding = 1, rgba32f) uniform image2D terrainNormal;
layout (binding = 2, rg32f) uniform image2D terrain2ndDerivative;

uniform float direction;
uniform float fWidth;

void iterationX()
{
	ivec2 pixelPosition = ivec2(gl_GlobalInvocationID.xy);
	pixelPosition = ivec2(pixelPosition.y, pixelPosition.x);
	
	float fSum = 0;
	for(int i = 0; i< fWidth - 1; ++i)
	{
		vec4 vValue = imageLoad(terrain2ndDerivative,pixelPosition);
		fSum += vValue.y;
		vValue = vec4(vValue.x, fSum, 0 ,0);
		imageStore(terrain2ndDerivative,pixelPosition,vValue);
		pixelPosition += ivec2(1,0);
	}
}

void iterationY()
{
	ivec2 pixelPosition = ivec2(gl_GlobalInvocationID.xy);
	float fSum = 0;
	for(int i = 0; i< fWidth - 1; ++i)
	{
		vec4 vValue = imageLoad(terrain2ndDerivative,pixelPosition);
		fSum += vValue.x;
		vValue = vec4(fSum, vValue.y, 0, 0);
		imageStore(terrain2ndDerivative,pixelPosition,vValue);
		pixelPosition += ivec2(0,1);
	}
}

void main(void)
{	
	if(direction == 0.0f)
	{
		iterationX();
	}
	else
	{
		iterationY();
	}
}
