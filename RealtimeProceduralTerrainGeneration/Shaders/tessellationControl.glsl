#version 430 core

layout (vertices = 3) out;

uniform vec3 cameraPosition = vec3(10,0,0);

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


float GetTessLevel(float distance0, float distance1)
{
    float avgDistance = (distance0 + distance1) / 2.0;
	
	float fMaxTessellationValue = 15.0f;
	float fMaxDistance = 7.0f;
	float fTessettation =  max(1,(-fMaxTessellationValue / fMaxDistance)*avgDistance + fMaxTessellationValue);

	return fTessettation;
}

void main(void)
{
	if(gl_InvocationID == 0)
	{
		float distanceCameraToV0 = distance(cameraPosition, tcs_in[0].vertexPositionWS);
		float distanceCameraToV1 = distance(cameraPosition, tcs_in[1].vertexPositionWS);
		float distanceCameraToV2 = distance(cameraPosition, tcs_in[2].vertexPositionWS);
		
		gl_TessLevelOuter[0] = GetTessLevel(distanceCameraToV1, distanceCameraToV2);
		gl_TessLevelOuter[1] = GetTessLevel(distanceCameraToV2, distanceCameraToV0);
		gl_TessLevelOuter[2] = GetTessLevel(distanceCameraToV0, distanceCameraToV1);
		gl_TessLevelInner[0] = gl_TessLevelOuter[0];
	}
	tcs_out[gl_InvocationID].vertexTextureCoordinates = tcs_in[gl_InvocationID].vertexTextureCoordinates;
	tcs_out[gl_InvocationID].vertexPositionVS = tcs_in[gl_InvocationID].vertexPositionVS;
	tcs_out[gl_InvocationID].vertexPositionWS = tcs_in[gl_InvocationID].vertexPositionWS;
	
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
