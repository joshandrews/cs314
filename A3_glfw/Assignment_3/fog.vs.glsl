#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;

out vec3 vNormal;
out vec4 vPosition;
out vec4 cPosition;


struct MaterialInfo
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Shininess;
};
uniform MaterialInfo Material;

uniform mat3 NormalMatrix;      // we keep a MV matrix without the translation component to apply to vectors
uniform mat4 MVP;               // ModelViewProjection Matrix
uniform mat4 ModelViewMatrix;   // ModelView Matrix
void main()
{
    // get vnormal
    vNormal     = normalize( NormalMatrix * Normal );
    vPosition   = ModelViewMatrix * vec4(Position,1.0); // Get the position in eye coordinates
    cPosition   = MVP * vec4(Position,1.0); // Get the position 
    gl_Position = MVP * vec4(Position, 1.0);
}