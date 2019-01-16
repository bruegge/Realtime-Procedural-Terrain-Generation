#version 430 core
uniform sampler2D texture1;
uniform sampler2D textureCliff;
uniform sampler2D textureGrass;
uniform sampler2D textureMud;
uniform sampler2D textureSnow;
out vec4 color;

in TES_OUT 
{
    vec3 vertexPositionWS;
	vec3 vertexPositionVS;
    vec2 vertexTextureCoordinates;
} fs_in;  

vec3 LinearInterpolationWithBoundaryCheck(vec3 color0, vec3 color1, float interpolatedValue, float minValue, float maxValue)
{
	float interpVal = (interpolatedValue-minValue)/(maxValue - minValue);
	vec3 result = color0 * (1.0f - interpVal) + color1 * interpVal;
	 
	result = interpVal >= 0 ? result : vec3(0,0,0);
	result = interpVal <= 1 ? result : vec3(0,0,0);
	return result;
}

vec3 CalculateColor(vec2 textureCoordinate, float height)
{
	float textureScale = 10.0f;
	float mipmapLevel = textureQueryLod(textureGrass, textureCoordinate*textureScale).x/40.0f;
    float tileMultiplier = 0.413f;
		
	vec2 texCoord1 = textureCoordinate*textureScale;
	vec2 texCoord2 = textureCoordinate*textureScale * tileMultiplier;

	vec3 Grass = texture(textureGrass, texCoord1,mipmapLevel).xyz * texture(textureGrass, texCoord2, mipmapLevel).xyz;
	vec3 Snow = texture(textureSnow, texCoord1,mipmapLevel).xyz * texture(textureSnow, texCoord2, mipmapLevel).xyz;
	vec3 Cliff = texture(textureCliff, texCoord1,mipmapLevel).xyz * texture(textureCliff, texCoord2, mipmapLevel).xyz;
	vec3 Mud = texture(textureMud, texCoord1,mipmapLevel).xyz * texture(textureMud, texCoord2, mipmapLevel).xyz;

	vec3 colorValue = vec3(0,0,0);
		
	float border1 = 0.25f;
	float border2 = 0.5f;
	float border3 = 0.75f;
		
	colorValue += LinearInterpolationWithBoundaryCheck(Grass, Mud, height, 0.0f, border1);
	colorValue += LinearInterpolationWithBoundaryCheck(Mud, Cliff, height, border1, border2);
	colorValue += LinearInterpolationWithBoundaryCheck(Cliff, Snow, height, border2, border3);
	colorValue += LinearInterpolationWithBoundaryCheck(Snow, Snow, height, border3, 1.0f);
		
	return colorValue;
}

void main()
{
		
	color = vec4(CalculateColor(fs_in.vertexTextureCoordinates,fs_in.vertexPositionWS.z), 1);
	
	//color = vec4(1,1,1,1);
}