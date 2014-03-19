#version 330

uniform sampler2D TexSampler0;

in vec2  vTexCoord;
layout (location = 0) out vec4 FragColor;

void main()
{
	// Texture2D fetches a value from the texture unit (sampler2D)
	vec4 texColor0 = texture(TexSampler0, vTexCoord);

	FragColor = texColor0;
}