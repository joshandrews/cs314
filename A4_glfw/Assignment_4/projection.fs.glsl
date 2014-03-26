#version 330

uniform sampler2D TexSampler2;

layout (location = 0) out vec4 FragColor;

in vec4 vTexCoord;

void main(void){
	vec2 tex2;
	tex2.x = vTexCoord.x/vTexCoord.w;
	tex2.y = vTexCoord.y/vTexCoord.w;
	vec4 texColor0 = texture(TexSampler2, tex2); 
	FragColor= texColor0;
}