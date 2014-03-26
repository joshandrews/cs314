#version 330

uniform samplerCube TexSampler1;

in vec3 vNormal;
in vec4 vPosition;

layout (location = 0) out vec4 FragColor;

void main(void) {
	vec3 normal = normalize(vNormal);
	vec3 reflected = reflect(normalize(vec3(vPosition)), normal); 
	vec4 texColor0 = texture(TexSampler1,reflected);
	FragColor = vec4(texColor0.r, texColor0.g, texColor0.b, 1.0); 
}