#version 450 core

// Outputs colors in RGBA
out vec4 FragColor;

// Gets the Texture Unit from the main function
uniform sampler2D screen;

// Inputs the texture coordinates from the Vertex Shader
in vec2 UVs;

void main()
{
	FragColor = texture(screen, UVs);
}