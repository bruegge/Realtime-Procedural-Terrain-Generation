#version 430 core

layout(triangles/*, equal_spacing*/) in;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform sampler2D texture1;

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

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
   	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
   	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
	tes_out.vertexTextureCoordinates = interpolate2D(tes_in[0].vertexTextureCoordinates, tes_in[1].vertexTextureCoordinates,tes_in[2].vertexTextureCoordinates);
	tes_out.vertexPositionWS = interpolate3D(tes_in[0].vertexPositionWS, tes_in[1].vertexPositionWS,tes_in[2].vertexPositionWS);
	tes_out.vertexPositionVS = interpolate3D(tes_in[0].vertexPositionVS, tes_in[1].vertexPositionVS,tes_in[2].vertexPositionVS);
	
	vec4 textureDepth = texture(texture1, tes_out.vertexTextureCoordinates);
	tes_out.vertexPositionWS.z = textureDepth.x;
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(tes_out.vertexPositionWS,1);
}	
