#version 430 core

layout(quads) in;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform sampler2D textureTerrain;

in TCS_OUT
{
	vec3 vertexPositionWS;
	vec3 vertexPositionVS;
	vec2 vertexTextureCoordinates;
} tes_in[];

out TES_OUT 
{
    vec3 vertexPositionWS;
	vec3 vertexPositionVS;
	vec2 vertexTextureCoordinates;
} tes_out;

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

void main()
{
	tes_out.vertexTextureCoordinates = Interpolate2D(tes_in[0].vertexTextureCoordinates, tes_in[1].vertexTextureCoordinates,tes_in[2].vertexTextureCoordinates,tes_in[3].vertexTextureCoordinates);
	tes_out.vertexPositionWS = Interpolate3D(tes_in[0].vertexPositionWS, tes_in[1].vertexPositionWS,tes_in[2].vertexPositionWS,tes_in[3].vertexPositionWS);
	tes_out.vertexPositionVS = Interpolate3D(tes_in[0].vertexPositionVS, tes_in[1].vertexPositionVS,tes_in[2].vertexPositionVS,tes_in[3].vertexPositionVS);
	
	vec4 textureDepth = texture(textureTerrain, tes_out.vertexTextureCoordinates);
	tes_out.vertexPositionWS.z = textureDepth.x;
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(tes_out.vertexPositionWS,1);
}	
