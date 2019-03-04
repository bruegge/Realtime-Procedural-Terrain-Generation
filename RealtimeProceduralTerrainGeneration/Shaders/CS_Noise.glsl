#version 430 core

layout (local_size_x = 1, local_size_y = 1) in;
layout (binding = 0, r32f) uniform image2D terrainHeight;

uniform int perm[512];
uniform float fWidth;

float fade(float t) 
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float t, float a, float b) 
{
	return a + t * (b - a);
}

float grad(int hash, float x, float y, float z) 
{
	int h = hash & 15;
	// Convert lower 4 bits of hash into 12 gradient directions
	float u = h < 8 ? x : y,
		v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float noise(float x, float y, float z) 
{
	int X = int(floor(x)) & 255;
	int Y = int(floor(y)) & 255;
	int Z = int(floor(z)) & 255;

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	float u = fade(x);
	float v = fade(y);
	float w = fade(z);

	int A = perm[X] + Y;
	int AA = perm[A] + Z;
	int AB = perm[A + 1] + Z;
	int B = perm[X + 1] + Y;
	int BA = perm[B] + Z;
	int BB = perm[B + 1] + Z;

	float res = lerp(w, lerp(v, lerp(u, grad(perm[AA], x, y, z), grad(perm[BA], x - 1, y, z)), lerp(u, grad(perm[AB], x, y - 1, z), grad(perm[BB], x - 1, y - 1, z))), lerp(v, lerp(u, grad(perm[AA + 1], x, y, z - 1), grad(perm[BA + 1], x - 1, y, z - 1)), lerp(u, grad(perm[AB + 1], x, y - 1, z - 1), grad(perm[BB + 1], x - 1, y - 1, z - 1))));
	return (res + 1.0) / 2.0;
}

void main(void)
{
	ivec2 position = ivec2(gl_GlobalInvocationID.xy);
	
	float fHeight = 0;
	for (int i = 1; i < 20; ++i)
	{
		fHeight += 0.5f / i * noise(position.x / fWidth * i * 5, position.y / fWidth * i * 5, 0);
	}
	imageStore(terrainHeight,position,vec4(fHeight,0,0,0));
}
