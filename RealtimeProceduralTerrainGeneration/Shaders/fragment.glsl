#version 330 core
in vec4 vertexColor;
in vec2 vertexTextureCoordinates;
uniform sampler2D texture1;
uniform int displayTexture = 0;
out vec4 color;

void main()
{
	if(displayTexture == 1)
	{
		color = texture(texture1, vertexTextureCoordinates);
	}
	else
	{
		color = vertexColor;
	}
}