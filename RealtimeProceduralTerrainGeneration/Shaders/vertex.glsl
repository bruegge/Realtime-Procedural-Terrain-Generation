#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoordinate;
layout (location = 2) in vec3 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform sampler2D texture1;

out vec4 vertexColor;
out vec2 vertexTextureCoordinates;

void main()
{
	vec4 texturePosition = texture(texture1, textureCoordinate.xy);
	vec4 positionHeight = vec4(position.xy, texturePosition.x,1);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * positionHeight;
	vertexColor = vec4(color, 1.0f);
	vertexTextureCoordinates = textureCoordinate;
}
