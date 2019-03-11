#version 430 core

layout (vertices = 4) out;

uniform vec3 cameraPosition = vec3(10,0,0);
uniform sampler2D textureTerrain2ndDerAcc;
uniform float fGridWidth;
uniform float fTextureWidth;
uniform float fEnableBezierSurface;
uniform float fEnableTessellation;

in VS_OUT
{
    vec3 vertexPositionWS;
	vec2 vertexTextureCoordinates;
	vec3 vertexNormal;
	flat int materialNumber;
} tcs_in[];


out TCS_OUT
{
	vec3 vertexPositionWS;
	vec2 vertexTextureCoordinates;
	vec3 vertexNormal;
	vec3 color;
} tcs_out[];

patch out vec4 bezierControlPoints[16];
patch out float isPatchTessellated;
patch out flat int[4] materialNumber;
float fControlPointDistance;

vec3 CalculateControlPointPositionXShift(vec3 vParentPosition, vec3 vParentNormal, float fPointDirection)
{
	vec3 vResult = vec3(0,0,0);
	vResult.xy = vParentPosition.xy + vec2(1,0) * fPointDirection * fControlPointDistance;  

	vec3 vCross = cross(vec3(0,1,0),vParentNormal);
	vec3 vTangent = vec3(1,0,0);
	if(length(vCross) != 0)
	{
		vTangent = normalize(vCross) * fPointDirection;
	}
	vResult.z = vParentPosition.z + (vTangent.z * fControlPointDistance) / abs(vTangent.x);

	return vResult;
}

vec3 CalculateControlPointPositionYShift(vec3 vParentPosition, vec3 vParentNormal, float fPointDirection)
{
	vec3 vResult = vec3(0,0,0);
	vResult.xy = vParentPosition.xy + vec2(0,1) * fPointDirection * fControlPointDistance;  

	vec3 vCross = cross(vParentNormal,vec3(1,0,0));
	vec3 vTangent = vec3(0,1,0);
	if(length(vCross) != 0)
	{
		vTangent = normalize(vCross) * fPointDirection;
	}
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

void CalculateCameraTessellation()
{
	float distanceCameraToV0 = distance(cameraPosition, tcs_in[0].vertexPositionWS);
	float distanceCameraToV1 = distance(cameraPosition, tcs_in[1].vertexPositionWS);
	float distanceCameraToV2 = distance(cameraPosition, tcs_in[2].vertexPositionWS);
	float distanceCameraToV3 = distance(cameraPosition, tcs_in[3].vertexPositionWS);
	
	vec4 result = vec4(0,0,0,0);	
	gl_TessLevelOuter[0] = GetTessLevelCamera(distanceCameraToV0, distanceCameraToV2);
	gl_TessLevelOuter[1] = GetTessLevelCamera(distanceCameraToV0, distanceCameraToV1);
	gl_TessLevelOuter[2] = GetTessLevelCamera(distanceCameraToV1, distanceCameraToV3);
	gl_TessLevelOuter[3] = GetTessLevelCamera(distanceCameraToV2, distanceCameraToV3);

	gl_TessLevelInner[0] = (gl_TessLevelOuter[0] + gl_TessLevelOuter[2]) *0.5f;
	gl_TessLevelInner[1] = (gl_TessLevelOuter[1] + gl_TessLevelOuter[3]) *0.5f;
}

float CalculateTessellation()
{
	vec3 p0 = texture(textureTerrain2ndDerAcc, tcs_in[0].vertexTextureCoordinates).xyz;
	vec3 p1 = texture(textureTerrain2ndDerAcc, tcs_in[1].vertexTextureCoordinates).xyz;
	vec3 p2 = texture(textureTerrain2ndDerAcc, tcs_in[2].vertexTextureCoordinates).xyz;
	vec3 p3 = texture(textureTerrain2ndDerAcc, tcs_in[3].vertexTextureCoordinates).xyz;
		
	float tessFactor = fTextureWidth;
	float tess0 = max(1, abs(p0.y - p2.y) * tessFactor);
	float tess2 = max(1, abs(p3.y - p1.y) * tessFactor);
	float tess1 = max(1, abs(p0.x - p1.x) * tessFactor);
	float tess3 = max(1, abs(p2.x - p3.x) * tessFactor);
	
	gl_TessLevelOuter[0] = tess0;
	gl_TessLevelOuter[1] = tess1;
	gl_TessLevelOuter[2] = tess2;
	gl_TessLevelOuter[3] = tess3;
	gl_TessLevelInner[0] = max(tess1, tess3);
	gl_TessLevelInner[1] = max(tess2, tess0);

	return tess0 + tess1+ tess2+ tess3;
}



void CalculateBezierSurfaceControlPoints()
{
	fControlPointDistance = 10.0f / fGridWidth * 0.33f;
	
	for(int i = 0; i< 16; ++i)
	{
		bezierControlPoints[i] = vec4(0.1f,0.1f,0.5f,1);
	}
	//return;

	//calculate BezierControlPoints corners
	bezierControlPoints[0] = vec4(tcs_in[3].vertexPositionWS,1);
	bezierControlPoints[3] = vec4(tcs_in[1].vertexPositionWS,1);
	bezierControlPoints[12] = vec4(tcs_in[2].vertexPositionWS,1);
	bezierControlPoints[15] = vec4(tcs_in[0].vertexPositionWS,1);
	

	//calculate BezierControlPoints edges
	bezierControlPoints[1] =  vec4(CalculateControlPointPositionXShift(bezierControlPoints[0].xyz, tcs_in[3].vertexNormal, 1),1); //yes
	bezierControlPoints[2] =  vec4(CalculateControlPointPositionXShift(bezierControlPoints[3].xyz, tcs_in[1].vertexNormal, -1),1); //no
	bezierControlPoints[4] =  vec4(CalculateControlPointPositionYShift(bezierControlPoints[0].xyz, tcs_in[3].vertexNormal, 1),1); //yes
	bezierControlPoints[8] =  vec4(CalculateControlPointPositionYShift(bezierControlPoints[12].xyz, tcs_in[2].vertexNormal, -1),1); //no
	bezierControlPoints[7] =  vec4(CalculateControlPointPositionYShift(bezierControlPoints[3].xyz, tcs_in[1].vertexNormal, 1),1); //no
	bezierControlPoints[11] = vec4(CalculateControlPointPositionYShift(bezierControlPoints[15].xyz, tcs_in[0].vertexNormal, -1),1); //no
	bezierControlPoints[13] = vec4(CalculateControlPointPositionXShift(bezierControlPoints[12].xyz, tcs_in[2].vertexNormal, 1),1); //no
	bezierControlPoints[14] = vec4(CalculateControlPointPositionXShift(bezierControlPoints[15].xyz, tcs_in[0].vertexNormal, -1),1); //no
	
	//calculate BezierControlPoints innerPoints
	
	//5
	vec3 interpolatedNormalX = interpolation(tcs_in[3].vertexNormal, tcs_in[1].vertexNormal, 0.33f);
	vec3 interpolatedNormalY = interpolation(tcs_in[3].vertexNormal, tcs_in[2].vertexNormal, 0.33f);
	vec3 position1 = CalculateControlPointPositionYShift(bezierControlPoints[1].xyz, interpolatedNormalX, 1);
	vec3 position2 = CalculateControlPointPositionXShift(bezierControlPoints[4].xyz, interpolatedNormalY, 1);
	bezierControlPoints[5] = vec4((position1 + position2) / 2.0f,1);

	//6
	interpolatedNormalX = interpolation(tcs_in[1].vertexNormal, tcs_in[3].vertexNormal, 0.33f);
	interpolatedNormalY = interpolation(tcs_in[1].vertexNormal, tcs_in[0].vertexNormal, 0.33f);
	position1 = CalculateControlPointPositionYShift(bezierControlPoints[2].xyz, interpolatedNormalX, 1);
	position2 = CalculateControlPointPositionXShift(bezierControlPoints[7].xyz, interpolatedNormalY, -1);
	bezierControlPoints[6] = vec4((position1 + position2) / 2.0f,1);
	
	//9
	interpolatedNormalX = interpolation(tcs_in[2].vertexNormal, tcs_in[0].vertexNormal, 0.33f);
	interpolatedNormalY = interpolation(tcs_in[2].vertexNormal, tcs_in[3].vertexNormal, 0.33f);
	position1 = CalculateControlPointPositionYShift(bezierControlPoints[13].xyz, interpolatedNormalX, -1);
	position2 = CalculateControlPointPositionXShift(bezierControlPoints[8].xyz, interpolatedNormalY, 1);
	bezierControlPoints[9] = vec4((position1 + position2) / 2.0f,1);

	//10
	interpolatedNormalX = interpolation(tcs_in[0].vertexNormal, tcs_in[2].vertexNormal, 0.33f);
	interpolatedNormalY = interpolation(tcs_in[0].vertexNormal, tcs_in[1].vertexNormal, 0.33f);
	position1 = CalculateControlPointPositionYShift(bezierControlPoints[14].xyz, interpolatedNormalX, -1);
	position2 = CalculateControlPointPositionXShift(bezierControlPoints[11].xyz, interpolatedNormalY, -1);
	bezierControlPoints[10] = vec4((position1 + position2) / 2.0f,1);
}

void main(void)
{
	if(gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = 1;
		gl_TessLevelOuter[1] = 1;
		gl_TessLevelOuter[2] = 1;
		gl_TessLevelOuter[3] = 1;
		gl_TessLevelInner[0] = 1;
		gl_TessLevelInner[1] = 1;
		
		isPatchTessellated = 0;
		if(fEnableTessellation != 0)
		{
			//CalculateCameraTessellation();
			isPatchTessellated = CalculateTessellation() - 4;
		}
		if(fEnableBezierSurface != 0)
		{
			if(isPatchTessellated != 0)
			{
				CalculateBezierSurfaceControlPoints();
			}
		}
	}
	
	materialNumber[gl_InvocationID] = tcs_in[gl_InvocationID].materialNumber;
	tcs_out[gl_InvocationID].color = vec3(0.5,0.5,0.5);
	tcs_out[gl_InvocationID].vertexTextureCoordinates = tcs_in[gl_InvocationID].vertexTextureCoordinates;
	tcs_out[gl_InvocationID].vertexPositionWS = tcs_in[gl_InvocationID].vertexPositionWS;
	tcs_out[gl_InvocationID].vertexNormal = tcs_in[gl_InvocationID].vertexNormal;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
