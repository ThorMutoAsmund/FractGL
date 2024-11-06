#version 450 core

// Outputs colors in RGBA
out vec4 FragColor;

// Gets the Texture Unit from the main function
uniform sampler2D screen;

// Inputs the texture coordinates from the Vertex Shader
in vec2 UVs;

vec4 color1 = vec4(1.9,0.55,0,1.0);
vec4 color2 = vec4(0.226,0.000,0.615,1.0);

void main()
{
	vec4 data = texture(screen, UVs);
	float iter = float(int(data.r) % 128);
	float maxIter = int(data.g);

	float d = iter / 128;
	vec4 c = mix(color1,color2,d);
	if (iter == maxIter)
	{
		FragColor = vec4(0,0,0,1.0);
	}
	else
	{
		FragColor = c;
	}
}