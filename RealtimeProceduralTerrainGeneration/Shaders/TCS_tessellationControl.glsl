#version 430 core

layout (vertices = 4) out;

uniform vec3 cameraPosition = vec3(10,0,0);
uniform sampler2D textureTerrain2ndDerAcc;
uniform float fGridWidth;
uniform float fTextureWidth;

in VS_OUT
{
    vec3 vertexPositionWS;
	vec2 vertexTextureCoordinates;
	vec3 vertexNormal;
} tcs_in[];


out TCS_OUT
{
	vec3 vertexPositionWS;
	vec2 vertexTextureCoordinates;
	vec3 vertexNormal;
	vec3 bezierControlPoints[16];
	vec3 color;
} tcs_out[];

vec3 CalculateControlPointPositionXShift(vec3 vParentPosition, vec3 vParentNormal, float fPointDirection)
{
	vec3 vResult;
	float fControlPointDistance = 10.0f / fGridWidth * 0.33f;
	vResult.xy = vParentPosition.xy + vec2(1,0) * fPointDirection * fControlPointDistance;  

	vec3 vTangent = normalize(cross(vec3(0,1,0),vParentNormal)) * fPointDirection;
	vResult.z = vParentPosition.z + (vTangent.z * fControlPointDistance) / abs(vTangent.x);

	return vResult;
}

vec3 CalculateControlPointPositionYShift(vec3 vParentPosition, vec3 vParentNormal, float fPointDirection)
{
	vec3 vResult;
	float fControlPointDistance = 10.0f / fGridWidth * 0.33f;
	vResult.xy = vParentPosition.xy + vec2(0,1) * fPointDirection * fControlPointDistance;  

	vec3 vTangent = normalize(cross(vParentNormal, vec3(1,0,0))) * fPointDirection;
	vResult.z = vParentPosition.z + (vTangent.z * fControlPointDistance) / abs(vTangent.y);

	return vResult;
}

float GetTessLevelCamera(float distance0, float distance1)
{
    float avgDistance = (distance0 + distance1) / 2.0;
	
	float fMaxTessellationValue = 3.0f;
	float fMaxDistance = 13.6f;
	
	return max(1,(-fMaxTessellationValue / fMaxDistance)*avgDistance + fMaxTessellationValue);
}

vec3 interpolation(vec3 v1, vec3 v2, float fProportion)
{
	return v1 + fProportion * (v2 - v1);
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
		
		float tessFactor = fTextureWidth;
		float tess0 = max(1, abs(p0.y - p2.y) * tessFactor);
		float tess2 = max(1, abs(p3.y - p1.y) * tessFactor);
		float tess1 = max(1, abs(p0.x - p1.x) * tessFactor);
		float tess3 = max(1, abs(p2.x - p3.x) * tessFactor);
		float tessDiagonal = 1;
		gl_TessLevelOuter[0] = 3;//tess0;
		gl_TessLevelOuter[1] = 3;//tess1;
		gl_TessLevelOuter[2] = 3;//tess2;
		gl_TessLevelOuter[3] = 3;//tess3;
		gl_TessLevelInner[0] = 3;//max(tess1, tess3);
		gl_TessLevelInner[1] = 3;//max(tess2, tess0);
	}

	for(int i = 0; i< 16; ++i)
	{
		tcs_out[gl_InvocationID].bezierControlPoints[i] = vec3(0,0,0);
	}
	
	//calculate BezierControlPoints corners
	tcs_out[gl_InvocationID].bezierControlPoints[0] = tcs_in[3].vertexPositionWS;
	tcs_out[gl_InvocationID].bezierControlPoints[3] = tcs_in[1].vertexPositionWS;
	tcs_out[gl_InvocationID].bezierControlPoints[12] = tcs_in[2].vertexPositionWS;
	tcs_out[gl_InvocationID].bezierControlPoints[15] = tcs_in[0].vertexPositionWS;
	

	//calculate BezierControlPoints edges
	tcs_out[gl_InvocationID].bezierControlPoints[1] = CalculateControlPointPositionXShift(tcs_in[3].vertexPositionWS, tcs_in[3].vertexNormal, 1);
	tcs_out[gl_InvocationID].bezierControlPoints[2] = CalculateControlPointPositionXShift(tcs_in[1].vertexPositionWS, tcs_in[1].vertexNormal, -1);
	tcs_out[gl_InvocationID].bezierControlPoints[4] = CalculateControlPointPositionYShift(tcs_in[3].vertexPositionWS, tcs_in[3].vertexNormal, 1);
	tcs_out[gl_InvocationID].bezierControlPoints[8] = CalculateControlPointPositionYShift(tcs_in[2].vertexPositionWS, tcs_in[2].vertexNormal, -1);
	tcs_out[gl_InvocationID].bezierControlPoints[7] = CalculateControlPointPositionYShift(tcs_in[1].vertexPositionWS, tcs_in[1].vertexNormal, 1);
	tcs_out[gl_InvocationID].bezierControlPoints[11] = CalculateControlPointPositionYShift(tcs_in[0].vertexPositionWS, tcs_in[0].vertexNormal, -1);
	tcs_out[gl_InvocationID].bezierControlPoints[13] = CalculateControlPointPositionXShift(tcs_in[2].vertexPositionWS, tcs_in[2].vertexNormal, 1);
	tcs_out[gl_InvocationID].bezierControlPoints[14] = CalculateControlPointPositionXShift(tcs_in[0].vertexPositionWS, tcs_in[0].vertexNormal, -1);
	
	//calculate BezierControlPoints innerPoints
	
	//5
	vec3 interpolatedNormalX = interpolation(tcs_in[3].vertexNormal, tcs_in[1].vertexNormal, 0.33f);
	vec3 interpolatedNormalY = interpolation(tcs_in[3].vertexNormal, tcs_in[2].vertexNormal, 0.33f);
	vec3 position1 = CalculateControlPointPositionYShift(tcs_out[gl_InvocationID].bezierControlPoints[1], interpolatedNormalX, 1);
	vec3 position2 = CalculateControlPointPositionXShift(tcs_out[gl_InvocationID].bezierControlPoints[4], interpolatedNormalY, 1);
	tcs_out[gl_InvocationID].bezierControlPoints[5] = (position1 + position2) / 2.0f;

	//6
	interpolatedNormalX = interpolation(tcs_in[1].vertexNormal, tcs_in[3].vertexNormal, 0.33f);
	interpolatedNormalY = interpolation(tcs_in[1].vertexNormal, tcs_in[0].vertexNormal, 0.33f);
	position1 = CalculateControlPointPositionYShift(tcs_out[gl_InvocationID].bezierControlPoints[2], interpolatedNormalX, 1);
	position2 = CalculateControlPointPositionXShift(tcs_out[gl_InvocationID].bezierControlPoints[7], interpolatedNormalY, -1);
	tcs_out[gl_InvocationID].bezierControlPoints[6] = (position1 + position2) / 2.0f;
	
	//9
	interpolatedNormalX = interpolation(tcs_in[2].vertexNormal, tcs_in[0].vertexNormal, 0.33f);
	interpolatedNormalY = interpolation(tcs_in[2].vertexNormal, tcs_in[3].vertexNormal, 0.33f);
	position1 = CalculateControlPointPositionYShift(tcs_out[gl_InvocationID].bezierControlPoints[13], interpolatedNormalX, -1);
	position2 = CalculateControlPointPositionXShift(tcs_out[gl_InvocationID].bezierControlPoints[8], interpolatedNormalY, 1);
	tcs_out[gl_InvocationID].bezierControlPoints[9] = (position1 + position2) / 2.0f;

	//10
	interpolatedNormalX = interpolation(tcs_in[0].vertexNormal, tcs_in[2].vertexNormal, 0.33f);
	interpolatedNormalY = interpolation(tcs_in[0].vertexNormal, tcs_in[1].vertexNormal, 0.33f);
	position1 = CalculateControlPointPositionYShift(tcs_out[gl_InvocationID].bezierControlPoints[14], interpolatedNormalX, -1);
	position2 = CalculateControlPointPositionXShift(tcs_out[gl_InvocationID].bezierControlPoints[11], interpolatedNormalY, -1);
	tcs_out[gl_InvocationID].bezierControlPoints[10] = (position1 + position2) / 2.0f;
	

	vec3 colors[4];
	colors[0] = vec3(1,0,0);
	colors[1] = vec3(0,1,0);
	colors[2] = vec3(0,0,1);
	colors[3] = vec3(1,1,1);
	
	tcs_out[gl_InvocationID].color = colors[gl_InvocationID];
	tcs_out[gl_InvocationID].vertexTextureCoordinates = tcs_in[gl_InvocationID].vertexTextureCoordinates;
	tcs_out[gl_InvocationID].vertexPositionWS = tcs_in[gl_InvocationID].vertexPositionWS;
	tcs_out[gl_InvocationID].vertexNormal = tcs_in[gl_InvocationID].vertexNormal;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
