#version 430 core

layout (local_size_x = 1, local_size_y = 1) in;
layout (binding = 0, r32f) uniform image2D terrainHeight;
layout (binding = 1, rgba32f) uniform image2D terrainNormal;
layout (binding = 2, rgba32f) uniform image2D terrain2ndDerivative;

uniform float fWidth;

void main(void)
{
	float fPixelDistance = 10.0f / fWidth * 2;
	
	ivec2 pixelPosition = ivec2(gl_GlobalInvocationID.xy);
	vec4 vPos = imageLoad(terrainHeight,pixelPosition);
	ivec2 pixelPositionX_1 = ivec2(int(mod(pixelPosition.x -1 + fWidth, fWidth)),pixelPosition.y);
	ivec2 pixelPositionX1 = ivec2(int(mod(pixelPosition.x +1, fWidth)),pixelPosition.y);
	ivec2 pixelPositionY_1 = ivec2(pixelPosition.x, int(mod(pixelPosition.y - 1 + fWidth, fWidth)));
	ivec2 pixelPositionY1 = ivec2(pixelPosition.x, int(mod(pixelPosition.y + 1 , fWidth)));
	
	vec4 vHeightX_1 = imageLoad(terrainHeight,pixelPositionX_1);
	vec4 vHeightX1 = imageLoad(terrainHeight,pixelPositionX1);
	vec4 vHeightY_1 = imageLoad(terrainHeight,pixelPositionY_1);
	vec4 vHeightY1 = imageLoad(terrainHeight,pixelPositionY1);
	
	//calculate normals
	float fZInXDir = (vHeightX1.x - vHeightX_1.x);
	float fZInYDir = (vHeightY1.x - vHeightY_1.x);
	vec3 vXDir = vec3(fPixelDistance, 0, fZInXDir);
	vec3 vYDir = vec3(0, fPixelDistance, fZInYDir);
	vec3 vNormal = normalize(cross(vXDir, vYDir));


	//2nd derivative
	imageStore(terrainNormal,pixelPosition,vec4(vNormal.x,vNormal.y,vNormal.z,0));

	//calculate 2nd derivative
	float xDir = (vHeightX_1.x - 2 * vPos.x + vHeightX1.x);
	float yDir = (vHeightY_1.x - 2 * vPos.x + vHeightY1.x);

	imageStore(terrain2ndDerivative,pixelPosition,vec4(yDir,xDir,yDir,xDir));

}
