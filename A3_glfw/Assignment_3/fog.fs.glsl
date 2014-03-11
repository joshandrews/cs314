#version 330

in vec3 vNormal;
in vec4 vPosition;
in vec4 cPosition;

struct LightInfo
{
    vec4 Position; // we are going to treat this as a direction to achieve directional lighting
    vec3 La;       // ambient light
    vec3 Ld;       // diffuse light
    vec3 Ls;       // specular light
    vec3 Intensity;// light intensity
};
uniform LightInfo Light0;
uniform LightInfo Light1;
uniform LightInfo Light2;

struct MaterialInfo
{
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float Shininess;    // Specular shininess factor
};

uniform MaterialInfo Material;
uniform float current_mode;
uniform float fog_weight;
layout (location = 0) out vec4 FragColor;

void main()
{
	// Difuse Lighting
	vec3 toLight0 = normalize(vec3(Light0.Position) - vec3(vPosition));
	vec3 normal0 = normalize(vNormal);
	float diffuse0 = max(0.0, dot(normal0, toLight0));

    // Specular Lighting
    vec3 toV0= -normalize(vec3(vPosition));
    vec3 h0 = normalize(toV0 + toLight0);
    float specular0 = pow(max(0.0, dot(h0, normal0)), Material.Shininess);
    float lngth0 = length(vec3(Light0.Position) - vec3(vPosition));
    float flipper0 = 1.0+pow(lngth0, 2);
    vec3 ambientvec0 = Light0.La*Material.Ka;
    vec3 diffusevec0 = Material.Kd*Light0.Ld*diffuse0/flipper0;
    vec3 specularvec0 = Material.Ks*Light0.Ld*specular0/flipper0;
    vec3 intensity0 = Light0.Intensity*(ambientvec0 + diffusevec0 + specularvec0);
    float fFogCoord = abs(vPosition.z/vPosition.w);
    float fResult = min(0.98, 1-exp(-1*fog_weight*fFogCoord));
	FragColor = mix(vec4(intensity0.x, intensity0.y, intensity0.z, 1.0)*2, vec4(0.2, 0.5, 0.2, 1.0), fResult);


}