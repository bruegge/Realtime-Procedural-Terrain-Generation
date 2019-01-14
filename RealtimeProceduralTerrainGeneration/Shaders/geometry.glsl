#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT 
{
    vec2 vertexTextureCoordinates;
} gs_in[];  

out GS_OUT 
{
    vec2 vertexTextureCoordinates;
	vec3 vertexNormal;
	vec3 vertexPosition;
} gs_out;  

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

const float heightOffset = -1.0f;

void main()
{    
	vec3 v1 = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
	vec3 v2 = gl_in[0].gl_Position.xyz - gl_in[2].gl_Position.xyz;
	vec3 faceNormal = normalize(cross(v2,v1));

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(gl_in[0].gl_Position.xy,gl_in[0].gl_Position.z + heightOffset,1); 
	gs_out.vertexTextureCoordinates = gs_in[0].vertexTextureCoordinates;
    gs_out.vertexNormal = faceNormal;
	gs_out.vertexPosition = gl_in[0].gl_Position.xyz;
	EmitVertex();

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(gl_in[1].gl_Position.xy,gl_in[1].gl_Position.z + heightOffset,1); 
	gs_out.vertexTextureCoordinates = gs_in[1].vertexTextureCoordinates;
    gs_out.vertexNormal = faceNormal;
    gs_out.vertexPosition = gl_in[1].gl_Position.xyz;
	EmitVertex();
   
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(gl_in[2].gl_Position.xy,gl_in[2].gl_Position.z + heightOffset,1); 
	gs_out.vertexTextureCoordinates = gs_in[2].vertexTextureCoordinates;
    gs_out.vertexNormal = faceNormal;
	gs_out.vertexPosition = gl_in[2].gl_Position.xyz;
    EmitVertex();
    
    EndPrimitive();
}  