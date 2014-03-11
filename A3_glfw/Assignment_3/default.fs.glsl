#version 330

in vec3 Color;

struct LightInfo
{
    vec4 LPosition; // we are going to treat this as a direction to achieve directional lighting
    vec3 La;       // ambient light
    vec3 Ld;       // diffuse light
    vec3 Ls;       // specular light
};
uniform LightInfo Light;

struct MaterialInfo
{
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float Shininess;    // Specular shininess factor
};

MaterialInfo Material;

layout (location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(Color,1.0);
}