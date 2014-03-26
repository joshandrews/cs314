#version 330

layout (location = 0) in vec3 Position;

uniform mat4 MVP;  // Projection Matrix


uniform mat4 uPV;

out  vec4  vTexCoord;

void main() {
	vTexCoord=  uPV * vec4(Position, 1.0); 
	gl_Position = MVP * vec4(Position, 1.0);
}
