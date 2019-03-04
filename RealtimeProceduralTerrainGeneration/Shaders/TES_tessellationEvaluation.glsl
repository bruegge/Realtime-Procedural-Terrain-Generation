#version 430 core

layout(quads) in;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform sampler2D textureTerrain;

uniform float fEnableBezierSurface;
uniform vec3 lightPosition = vec3(50,50,100);

in TCS_OUT
{
	vec3 vertexPositionWS;
	vec2 vertexTextureCoordinates;
	vec3 vertexNormal;
	vec3 color;
} tes_in[];

out TES_OUT 
{
    vec3 vertexPositionWS;
	vec4 vertexPositionVS;
	vec2 vertexTextureCoordinates;
	vec3 vertexNormal;
	vec3 color;
	vec2 patchTextureCoordinates;
	flat int[4] materialNumber;
	vec3 lightPosition;
} tes_out;

patch in vec4 bezierControlPoints[16];
patch in float isPatchTessellated;
patch in flat int[4] materialNumber;

vec4 Interpolate4D(vec4 p1, vec4 p2, vec4 p3, vec4 p4)
{
	vec4 pm1 = mix(p1,p2, gl_TessCoord.x);
	vec4 pm2 = mix(p3,p4, gl_TessCoord.x);
	return mix(pm1, pm2, gl_TessCoord.y);
}

vec3 Interpolate3D(vec3 p1, vec3 p2, vec3 p3, vec3 p4)
{
	vec3 pm1 = mix(p1,p2, gl_TessCoord.x);
	vec3 pm2 = mix(p3,p4, gl_TessCoord.x);
	return mix(pm1, pm2, gl_TessCoord.y);
}

vec2 Interpolate2D(vec2 p1, vec2 p2, vec2 p3, vec2 p4)
{
	vec2 pm1 = mix(p1,p2, gl_TessCoord.x);
	vec2 pm2 = mix(p3,p4, gl_TessCoord.x);
	return mix(pm1, pm2, gl_TessCoord.y);
}

float CalculateBernsteinpolynomDegree3(int i, float fT)
{
	float fBinomialkoeffizienten[4];
	fBinomialkoeffizienten[0] = 1.0f;
	fBinomialkoeffizienten[1] = 3.0f;
	fBinomialkoeffizienten[2] = 3.0f;
	fBinomialkoeffizienten[3] = 1.0f;

	return fBinomialkoeffizienten[i] * pow(fT,i) * pow(1-fT,3 - i);
}

float CalculateBezierSurfaceHeight()
{
	vec2 positionRelative = vec2(1,1)-gl_TessCoord.yx;
	float fHeight = 0.0f;
	float xDirection[4];
	float yDirection[4];
	for(int i = 0; i<4; ++i)
	{
		xDirection[i] = CalculateBernsteinpolynomDegree3(i, positionRelative.x);
		yDirection[i] = CalculateBernsteinpolynomDegree3(i, positionRelative.y);
	}	

	for(int i = 0; i< 4; ++i)
	{
		for(int j = 0; j< 4; j++)
		{
			fHeight += xDirection[i] * yDirection[j] * bezierControlPoints[4*j+i].z;
		}
	}
	
	return fHeight;
	
}

void main()
{
	tes_out.vertexTextureCoordinates = Interpolate2D(tes_in[0].vertexTextureCoordinates, tes_in[1].vertexTextureCoordinates,tes_in[2].vertexTextureCoordinates,tes_in[3].vertexTextureCoordinates);
	tes_out.vertexPositionWS = Interpolate3D(tes_in[0].vertexPositionWS, tes_in[1].vertexPositionWS,tes_in[2].vertexPositionWS,tes_in[3].vertexPositionWS);
	tes_out.vertexNormal = Interpolate3D(tes_in[0].vertexNormal, tes_in[1].vertexNormal,tes_in[2].vertexNormal,tes_in[3].vertexNormal);
	tes_out.color = Interpolate3D(tes_in[0].color, tes_in[1].color, tes_in[2].color,tes_in[3].color);
	
	if(fEnableBezierSurface!=0)
	{
		if(isPatchTessellated != 0)
		{
			tes_out.vertexPositionWS.z = CalculateBezierSurfaceHeight();
		}
	}
	for(int i = 0; i < 4; ++i)
	{
		tes_out.materialNumber[i] = materialNumber[i];
	}
	tes_out.patchTextureCoordinates = vec2(1,1)-gl_TessCoord.yx;
	tes_out.vertexPositionVS = projectionMatrix * viewMatrix * modelMatrix * vec4(tes_out.vertexPositionWS,1);
	tes_out.lightPosition = (viewMatrix * modelMatrix * vec4(lightPosition,1)).xyz;
	gl_Position = tes_out.vertexPositionVS;
}	
