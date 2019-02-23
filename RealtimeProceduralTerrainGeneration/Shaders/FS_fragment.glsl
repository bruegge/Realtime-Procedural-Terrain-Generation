#version 430 core
uniform sampler2D textureTerrain;
uniform sampler2D textureTerrainNormal;
uniform sampler2D textureTerrain2ndDerAcc;
uniform sampler2D textureGrass;
uniform sampler2D textureCliff;
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

vec3 CalculateColor(vec2 textureCoordinate, float height, float normal)
{
	float textureScale = 30.0f;
	float mipmapLevel = textureQueryLod(textureGrass, textureCoordinate*textureScale).x/30.0f;
   	
	vec2 texCoord1 = textureCoordinate*textureScale;
	vec2 texCoord2 = textureCoordinate*textureScale*3.88f;
	
	vec3 Grass = texture(textureGrass, texCoord1, mipmapLevel).xyz;
	vec3 Cliff = texture(textureCliff, texCoord1, mipmapLevel).xyz;
	vec3 Snow = texture(textureSnow, texCoord1, mipmapLevel).xyz;
	
	vec3 colorValue = vec3(0,0,0);
		
	float border1 = 0.33f;
	float border2 = 0.66f;
		
	colorValue += LinearInterpolationWithBoundaryCheck(Grass, Cliff, height, 0.0f, border1);
	colorValue += LinearInterpolationWithBoundaryCheck(Cliff, Snow, height, border1, border2);
	colorValue += LinearInterpolationWithBoundaryCheck(Snow, Snow, height, border2, 2.0f);
		
	return colorValue;
}

void main()
{	
	vec3 normal = texture(textureTerrainNormal, fs_in.vertexTextureCoordinates).xyz;
	color = vec4(CalculateColor(fs_in.vertexTextureCoordinates,fs_in.vertexPositionWS.z, normal.z), 1);
	//color = vec4(normal,1);
	//color = vec4(0,1,1,1);
	//color = vec4(fs_in.vertexPositionWS.z, fs_in.vertexPositionWS.z, fs_in.vertexPositionWS.z, 1);
}