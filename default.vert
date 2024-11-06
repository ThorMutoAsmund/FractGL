#version 450 core

// Positions/Coordinates
layout (location = 0) in vec3 pos;
// Texture Coordinates
layout (location = 1) in vec2 uvs;

// Outputs the texture coordinates to the fragment shader
out vec2 UVs;

// Controls the scale of the vertices
//uniform float scalex;

void main()
{
	// Outputs the positions/coordinates of all vertices
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	UVs = uvs;
}