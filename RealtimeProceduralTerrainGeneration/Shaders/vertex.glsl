#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoordinate;
layout (location = 2) in vec3 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 vertexColor;
out vec2 vertexTextureCoordinates;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
	vertexColor = vec4(color, 1.0f);
	vertexTextureCoordinates = textureCoordinate;
}
