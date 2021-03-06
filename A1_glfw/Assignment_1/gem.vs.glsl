#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;

out vec3 LightIntensity;

struct LightInfo
{
    vec4 Position; // we are going to treat this as a direction to acheive directional lighting
    vec3 La;       // ambient light
    vec3 Ld;       // diffuse light
    vec3 Ls;       // specular light
};
uniform LightInfo Light;

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

uniform vec3 gem_pos; // location of the gem

void main()
{
    // determine vertex color
    vec3 tnorm     = normalize( NormalMatrix * Normal );
    vec3 s         = vec3(Light.Position); // incident vector
    LightIntensity = dot(s, tnorm) * Material.Ka * 0.4 + Material.Ka * 0.6;

    /**********************************
     * Your Code Goes Here
     **********************************/
    gl_Position = MVP * vec4(Position, 1.0); // REPLACE ME
}