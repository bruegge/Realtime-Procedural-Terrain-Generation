#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoordinate;
layout (location = 2) in vec3 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform sampler2D texture1;

out VS_OUT 
{
    vec3 vertexPositionWS;
	vec3 vertexPositionVS;
	vec2 vertexTextureCoordinates;
} vs_out;

void main()
{
	vec4 textureDepth = texture(texture1, textureCoordinate.xy);
	vec4 positionHeight = vec4(position.xy, textureDepth.x,1);
	vs_out.vertexPositionWS = positionHeight.xyz;
	vs_out.vertexTextureCoordinates = textureCoordinate;
	vs_out.vertexPositionVS = gl_Position.xyz;
}
