#extension GL_EXT_texture_array : enable
#version 430 core
uniform sampler2D textureTerrain;
uniform sampler2D textureTerrainNormal;
uniform sampler2D textureTerrain2ndDerAcc;
uniform sampler2DArray textureMaterialColorSpecular;
uniform sampler2DArray textureMaterialNormalHeight;

out vec4 color;

in TES_OUT 
{
    vec3 vertexPositionWS;
    vec2 vertexTextureCoordinates;
	vec3 vertexNormal;
	vec3 color;
	vec2 patchTextureCoordinates;
	flat int[4] materialNumber;
} fs_in;  

vec4 CalculateColor()
{
	float fFactor = 0.50f;
	float fRadius0 = length(vec2(1,1)-fs_in.patchTextureCoordinates)*fFactor;
	float fRadius1 = length(vec2(1,0)-fs_in.patchTextureCoordinates)*fFactor;
	float fRadius2 = length(vec2(0,1)-fs_in.patchTextureCoordinates)*fFactor;
	float fRadius3 = length(fs_in.patchTextureCoordinates)*fFactor;

	vec4 normalHeight0 = texture(textureMaterialNormalHeight, vec3(fs_in.patchTextureCoordinates,fs_in.materialNumber[0]),0);
	vec4 normalHeight1 = texture(textureMaterialNormalHeight, vec3(fs_in.patchTextureCoordinates,fs_in.materialNumber[1]),0);
	vec4 normalHeight2 = texture(textureMaterialNormalHeight, vec3(fs_in.patchTextureCoordinates,fs_in.materialNumber[2]),0);
	vec4 normalHeight3 = texture(textureMaterialNormalHeight, vec3(fs_in.patchTextureCoordinates,fs_in.materialNumber[3]),0);
	
	float fHeight0 = normalHeight0.a - fRadius0;
	float fHeight1 = normalHeight1.a - fRadius1;
	float fHeight2 = normalHeight2.a - fRadius2;
	float fHeight3 = normalHeight3.a - fRadius3;
	
	int textureNumber1 = fHeight0> fHeight1 ? 0 : 1;
	int textureNumber2 = fHeight2> fHeight3 ? 2 : 3;
	float fHeight01 = max(fHeight0, fHeight1);
	float fHeight23 = max(fHeight2, fHeight3);
	int textureNumber = fHeight01 > fHeight23 ? textureNumber1 : textureNumber2;
	
	vec4 normalHeight = texture(textureMaterialNormalHeight, vec3(fs_in.patchTextureCoordinates,fs_in.materialNumber[textureNumber]),0);
	vec4 colorSpecular = texture(textureMaterialColorSpecular, vec3(fs_in.patchTextureCoordinates,fs_in.materialNumber[textureNumber]),0);

	normalHeight.xyz = normalize(normalHeight.xyz * 2.0f - 1.0f);
	return colorSpecular; 
}

void main()
{	
	//vec3 normal = texture(textureTerrainNormal, fs_in.vertexTextureCoordinates).xyz;
	//vec2 secondDerivative = texture(textureTerrain2ndDerAcc, fs_in.vertexTextureCoordinates).zw;
	//vec3 normal = fs_in.vertexNormal;
	//normal = normalize(normal * 2.0 - 1.0);
	color = CalculateColor();
	//color = vec4(normal,1);
	//color = vec4(abs(secondDerivative*10.0f),0,1);
	//color = vec4(fs_in.color,1);
	//color = vec4(0,1,1,1);
	//color = vec4(fs_in.vertexPositionWS.z, fs_in.vertexPositionWS.z, fs_in.vertexPositionWS.z, 1);
}