#version 430 core

layout (local_size_x = 1, local_size_y = 1) in;
layout (binding = 0, r32f) uniform image2D terrainHeight;
layout (binding = 1, rgba32f) uniform image2D terrainNormal;
layout (binding = 2, rg32f) uniform image2D terrain2ndDerivative;

uniform float fWidth;

void main(void)
{
	float fPixelDistance = 10.0f / fWidth * 2;
	
	ivec2 pixelPosition = ivec2(gl_GlobalInvocationID.xy);
	vec4 vPos = imageLoad(terrainHeight,pixelPosition);
	vec4 vHeightX_1 = imageLoad(terrainHeight,pixelPosition + ivec2(-1,0));
	vec4 vHeightX1 = imageLoad(terrainHeight,pixelPosition + ivec2(1,0));
	vec4 vHeightY_1 = imageLoad(terrainHeight,pixelPosition + ivec2(0,-1));
	vec4 vHeightY1 = imageLoad(terrainHeight,pixelPosition + ivec2(0,1));
	
	//calculate normals
	float fZInXDir = (vHeightX1.x - vHeightX_1.x);
	float fZInYDir = (vHeightY1.x - vHeightY_1.x);
	vec3 vXDir = vec3(fPixelDistance, 0, fZInXDir);
	vec3 vYDir = vec3(0, fPixelDistance, fZInYDir);
	vec3 vNormal = normalize(cross(vXDir, vYDir));

	imageStore(terrainNormal,pixelPosition,vec4(vNormal.x,vNormal.y,vNormal.z,0));

	//calculate 2nd derivative
	float xDir = (vHeightX_1.x - 2 * vPos.x + vHeightX1.x);
	float yDir = (vHeightY_1.x - 2 * vPos.x + vHeightY1.x);

	imageStore(terrain2ndDerivative,pixelPosition,vec4(yDir,xDir,0,0));

}
