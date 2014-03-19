#version 330

layout (location = 0) in vec3 Position;
layout (location = 2) in vec2 TexCoord;

out vec2  vTexCoord;

uniform mat3 NormalMatrix;      // we keep a MV matrix without the translation component to apply to vectors
uniform mat4 ProjectionMatrix;  // Projection Matrix
uniform mat4 ModelViewMatrix;   // ModelView Matrix

void main()
{
    vTexCoord = TexCoord;

    gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(Position, 1.0);
}
