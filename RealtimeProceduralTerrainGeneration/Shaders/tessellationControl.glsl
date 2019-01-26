#version 430 core

layout (vertices = 4) out;

uniform vec3 cameraPosition = vec3(10,0,0);
uniform sampler2D textureTerrain2ndDerAcc;

in VS_OUT
{
    vec3 vertexPositionWS;
	vec3 vertexPositionVS;
	vec2 vertexTextureCoordinates;
} tcs_in[];


out TCS_OUT
{
	vec3 vertexPositionWS;
	vec3 vertexPositionVS;
	vec2 vertexTextureCoordinates;
} tcs_out[];


float GetTessLevelCamera(float distance0, float distance1)
{
    float avgDistance = (distance0 + distance1) / 2.0;
	
	float fMaxTessellationValue = 3.0f;
	float fMaxDistance = 13.6f;
	
	return max(1,(-fMaxTessellationValue / fMaxDistance)*avgDistance + fMaxTessellationValue);
}

float GetTessLevelCurvature(vec2 p1, vec2 p2)
{
	float diff = abs(p2.x - p1.x);
	return max(1,diff / 30.0f);
}

void main(void)
{
	if(gl_InvocationID == 0)
	{
		/*
		float distanceCameraToV0 = distance(cameraPosition, tcs_in[0].vertexPositionWS);
		float distanceCameraToV1 = distance(cameraPosition, tcs_in[1].vertexPositionWS);
		float distanceCameraToV2 = distance(cameraPosition, tcs_in[2].vertexPositionWS);
		
		gl_TessLevelOuter[0] = GetTessLevelCamera(distanceCameraToV1, distanceCameraToV2);
		gl_TessLevelOuter[1] = GetTessLevelCamera(distanceCameraToV2, distanceCameraToV0);
		gl_TessLevelOuter[2] = GetTessLevelCamera(distanceCameraToV0, distanceCameraToV1);
		gl_TessLevelInner[0] = gl_TessLevelOuter[0];
		*/
		vec3 p0 = texture(textureTerrain2ndDerAcc, tcs_in[0].vertexTextureCoordinates).xyz;
		vec3 p1 = texture(textureTerrain2ndDerAcc, tcs_in[1].vertexTextureCoordinates).xyz;
		vec3 p2 = texture(textureTerrain2ndDerAcc, tcs_in[2].vertexTextureCoordinates).xyz;
		vec3 p3 = texture(textureTerrain2ndDerAcc, tcs_in[3].vertexTextureCoordinates).xyz;
		
		float tessFactor = 5;
		float tess0 = max(1, abs(p0.y - p2.y) / tessFactor);
		float tess2 = max(1, abs(p3.y - p1.y) / tessFactor);
		float tess1 = max(1, abs(p0.x - p1.x) / tessFactor);
		float tess3 = max(1, abs(p2.x - p3.x) / tessFactor);
		float tessDiagonal = 1;
		gl_TessLevelOuter[0] = tess0;
		gl_TessLevelOuter[1] = tess1;
		gl_TessLevelOuter[2] = tess2;
		gl_TessLevelOuter[3] = tess3;
		gl_TessLevelInner[0] = max(tess1, tess3);
		gl_TessLevelInner[1] = max(tess2, tess0);
	}
	tcs_out[gl_InvocationID].vertexTextureCoordinates = tcs_in[gl_InvocationID].vertexTextureCoordinates;
	tcs_out[gl_InvocationID].vertexPositionVS = tcs_in[gl_InvocationID].vertexPositionVS;
	tcs_out[gl_InvocationID].vertexPositionWS = tcs_in[gl_InvocationID].vertexPositionWS;
	
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
