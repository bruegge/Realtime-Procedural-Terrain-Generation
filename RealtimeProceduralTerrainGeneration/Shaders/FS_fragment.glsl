#extension GL_EXT_texture_array : enable
#version 430 core
uniform sampler2D textureTerrain;
uniform sampler2D textureTerrainNormal;
uniform sampler2D textureTerrain2ndDerAcc;
uniform sampler2DArray textureMaterialColorSpecular;
uniform sampler2DArray textureMaterialNormalHeight;


uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 diffuseAlbedo = vec3(1,1,1);
uniform vec3 specularAlbedo = vec3(0.7,0.7,0.7);
uniform float specularPower = 1.0f;

out vec4 color;

in TES_OUT 
{
    vec3 vertexPositionWS;
	vec4 vertexPositionVS;
    vec2 vertexTextureCoordinates;
	vec3 vertexNormal;
	vec3 color;
	vec2 patchTextureCoordinates;
	flat int[4] materialNumber;
	vec3 lightPosition;
} fs_in;  

void CalculateColor(out vec3 vNormalOut, out float fSpecularOut, out vec3 vColorOut)
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

	vNormalOut = normalize(normalHeight.xyz * 2.0f - 1.0f);
	vColorOut = colorSpecular.rgb;
	fSpecularOut = colorSpecular.a;
}

vec3 Phong(vec3 normal, float specularTexture, vec3 color)
{
	vec4 P = fs_in.vertexPositionVS;
	vec3 N = normalize(mat3(viewMatrix * modelMatrix) * normal);
	vec3 L = normalize(fs_in.lightPosition - P.xyz);
	vec3 V = normalize(-P.xyz);

	vec3 R = reflect(-L, N);
	vec3 diffuse = max(dot(N,L), 0.0) * color;
	vec3 specular = pow(max(dot(R,V), 0.0), specularPower) * color;

	return diffuse /*+ specular*specularTexture*/;
}

vec3 NormalMapping(vec3 normal, vec3 normalFromTexture)
{
	vec3 tangent = normalize(cross(vec3(0,1,0),normalFromTexture)); 
	vec3 bitangent = normalize(cross(normalFromTexture,tangent)); 
	mat3 TBNMatrix = mat3(tangent, bitangent,normalFromTexture);
	return TBNMatrix * normal;
}

void main()
{	
	vec3 normal = fs_in.vertexNormal;
	vec3 normalTexture;
	float specularTexture;
	vec3 colorTexture;
	CalculateColor(normalTexture, specularTexture, colorTexture);
	normal = NormalMapping(normal,normalTexture);

	//vec4 erosion = texture(textureTerrain2ndDerAcc, fs_in.vertexTextureCoordinates);
	//color = vec4(erosion.ba*10,0,1);
	color = vec4(Phong(normal,specularTexture,colorTexture),1);
}