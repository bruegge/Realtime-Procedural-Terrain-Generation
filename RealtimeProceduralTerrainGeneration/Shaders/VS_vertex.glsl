#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoordinate;
layout (location = 2) in vec3 color;


uniform sampler2D textureTerrainNormal;
uniform sampler2D textureTerrain;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out VS_OUT 
{
    vec3 vertexPositionWS;
	vec2 vertexTextureCoordinates;
	vec3 vertexNormal;
} vs_out;

void main()
{
	vec4 textureDepth = texture(textureTerrain, textureCoordinate.xy);
	vs_out.vertexNormal = texture(textureTerrainNormal, textureCoordinate.xy).xyz;
	vec4 positionHeight = vec4(position.xy, textureDepth.x,1);
	vs_out.vertexPositionWS = positionHeight.xyz;
	vs_out.vertexTextureCoordinates = textureCoordinate;
}
