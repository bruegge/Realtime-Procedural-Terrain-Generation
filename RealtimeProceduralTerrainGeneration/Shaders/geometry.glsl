#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in TES_OUT 
{
    vec3 vertexPositionWS;
	vec3 vertexPositionVS;
	vec2 vertexTextureCoordinates;
} gs_in[];  

out GS_OUT 
{
    vec3 vertexPositionWS;
	vec3 vertexPositionVS;
    vec2 vertexTextureCoordinates;
	vec3 vertexNormal;
} gs_out;  

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

const float heightOffset = -1.0f;

void main()
{    
	vec3 v1 = gs_in[0].vertexPositionWS - gs_in[1].vertexPositionWS;
	vec3 v2 = gs_in[0].vertexPositionWS - gs_in[2].vertexPositionWS;
	vec3 faceNormal = normalize(cross(v2,v1));

	gs_out.vertexTextureCoordinates = gs_in[0].vertexTextureCoordinates;
    gs_out.vertexPositionWS = gs_in[0].vertexPositionWS;
    gs_out.vertexPositionVS = gs_in[0].vertexPositionVS;
    gs_out.vertexNormal = faceNormal;
	EmitVertex();

    gs_out.vertexTextureCoordinates = gs_in[1].vertexTextureCoordinates;
    gs_out.vertexPositionWS = gs_in[1].vertexPositionWS;
    gs_out.vertexPositionVS = gs_in[1].vertexPositionVS;
    gs_out.vertexNormal = faceNormal;
	EmitVertex();
   
	gs_out.vertexTextureCoordinates = gs_in[2].vertexTextureCoordinates;
    gs_out.vertexPositionWS = gs_in[2].vertexPositionWS;
    gs_out.vertexPositionVS = gs_in[2].vertexPositionVS;
    gs_out.vertexNormal = faceNormal;
	EmitVertex();
    
    EndPrimitive();
}  