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

vec3 LinearInterpolation(vec3 color0, vec3 color1, float interpolatedValue, float minValue, float maxValue)
{
	float interpVal = (interpolatedValue-minValue)/(maxValue - minValue);
	vec3 result = color0 * (1.0f - interpVal) + color1 * interpVal;
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
		float mipmapLevel = textureQueryLod(textureGrass, fs_in.vertexTextureCoordinates*textureScale).x/40.0f;
    
		vec3 Grass = texture(textureGrass, fs_in.vertexTextureCoordinates*textureScale,mipmapLevel).xyz;
		vec3 Snow = texture(textureSnow, fs_in.vertexTextureCoordinates*textureScale,mipmapLevel).xyz;
		vec3 Cliff = texture(textureCliff, fs_in.vertexTextureCoordinates*textureScale,mipmapLevel).xyz;
		vec3 Mud = texture(textureMud, fs_in.vertexTextureCoordinates*textureScale,mipmapLevel).xyz;

		vec3 colorValue = vec3(0,0,0);
		
		float border1 = 0.25f;
		float border2 = 0.5f;
		float border3 = 0.75f;
		
		if(fs_in.vertexPosition.z < border1)
		{
			colorValue = LinearInterpolation(Grass,Mud,fs_in.vertexPosition.z, 0.0f, border1);
		}
		else if(fs_in.vertexPosition.z < border2)
		{
			colorValue = LinearInterpolation(Mud, Cliff,fs_in.vertexPosition.z, border1, border2);
		}
		else if(fs_in.vertexPosition.z < border3)
		{
			colorValue = LinearInterpolation(Cliff, Snow,fs_in.vertexPosition.z, border2, border3);
		}
		else
		{
			colorValue = Snow;
		}
		
		//vec3 colorValue = BiLinearInterpolation(Mud, Grass, Cliff, Snow, vec2(fs_in.vertexNormal.z, fs_in.vertexPosition.z));//deltaX = Normal, deltaY = height
		color = vec4(colorValue, 1);
	}
}