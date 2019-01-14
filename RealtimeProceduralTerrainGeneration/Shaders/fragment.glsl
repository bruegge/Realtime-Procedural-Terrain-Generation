#version 430 core
in vec2 vertexTextureCoordinates;
uniform sampler2D texture1;
uniform sampler2D textureCliff;
uniform sampler2D textureGrass;
uniform sampler2D textureMud;
uniform sampler2D textureSnow;
uniform int displayTexture = 0;
out vec4 color;

in GS_OUT 
{
    vec2 vertexTextureCoordinates;
	vec3 vertexNormal;
	vec3 vertexPosition;
} fs_in;  

vec3 BiLinearInterpolation(vec3 color00, vec3 color01, vec3 color10, vec3 color11, vec2 interpolatedValue)
{
	vec3 colorX0 = color00 * (1.0f - interpolatedValue.x) + color01 * interpolatedValue.x;
	vec3 colorX1 = color10 * (1.0f - interpolatedValue.x) + color11 * interpolatedValue.x;
	vec3 result = colorX0 * (1.0f - interpolatedValue.y) + colorX1 * interpolatedValue.y;
	return result;
}

void main()
{
	if(displayTexture == 1)
	{
		color = texture(texture1, fs_in.vertexTextureCoordinates);
		//color = vec4(fs_in.vertexNormal.x,fs_in.vertexNormal.y,fs_in.vertexNormal.z,1);
	}
	else
	{
		float textureScale = 100.0f;
		float mipmapLevel = textureQueryLod(textureGrass, fs_in.vertexTextureCoordinates*textureScale).x/20.0f;
    
		vec3 Grass = texture(textureGrass, fs_in.vertexTextureCoordinates*textureScale,mipmapLevel).xyz;
		vec3 Snow = texture(textureSnow, fs_in.vertexTextureCoordinates*textureScale,mipmapLevel).xyz;
		vec3 Cliff = texture(textureCliff, fs_in.vertexTextureCoordinates*textureScale,mipmapLevel).xyz;
		vec3 Mud = texture(textureMud, fs_in.vertexTextureCoordinates*textureScale,mipmapLevel).xyz;

		vec3 colorValue = vec3(0,0,0);
		
		if(fs_in.vertexPosition.z < 0.25)
		{
			colorValue = Grass;
		}
		else if(fs_in.vertexPosition.z < 0.5)
		{
			colorValue = Mud;
		}
		else if(fs_in.vertexPosition.z < 0.75)
		{
			colorValue = Cliff;
		}
		else
		{
			colorValue = Snow;
		}
		
		//vec3 colorValue = BiLinearInterpolation(Mud, Grass, Cliff, Snow, vec2(fs_in.vertexNormal.z, fs_in.vertexPosition.z));//deltaX = Normal, deltaY = height
		color = vec4(colorValue, 1);
	}
}