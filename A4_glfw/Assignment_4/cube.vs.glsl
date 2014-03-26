#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;

out vec3 vNormal;
out vec4 vPosition;

uniform mat3 NormalMatrix;      // we keep a MV matrix without the translation component to apply to vectors
uniform mat4 ProjectionMatrix;  // Projection Matrix
uniform mat4 ModelViewMatrix;   // ModelView Matrix
uniform vec3 cameraPosition;

void main()
{
    vNormal     = normalize( Normal );
    vPosition   = vec4(Position-cameraPosition,1.0); 

    gl_Position = ProjectionMatrix  * ModelViewMatrix * vec4(Position, 1.0);
}
