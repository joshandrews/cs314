#version 330

in vec3 vNormal;
in vec4 vPosition;

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
    vec3 intensity0 = ambientvec0 + diffusevec0 + specularvec0;

    if (current_mode != 2.0) {
	   FragColor = vec4(intensity0.x, intensity0.y, intensity0.z, 1.0);
    }

    if (current_mode == 2.0) {
        
        // LIGHT1
        // Difuse Lighting
        vec3 toLight1 = normalize(vec3(Light1.Position) - vec3(vPosition));
        vec3 normal1 = normalize(vNormal);
        float diffuse1 = max(0.0, dot(normal1, toLight1));

        // Specular Lighting
        vec3 toV1= -normalize(vec3(vPosition));
        vec3 h1 = normalize(toV1 + toLight1);
        float specular1 = pow(max(0.0, dot(h1, normal1)), Material.Shininess);
        float lngth1 = length(vec3(Light1.Position) - vec3(vPosition));
        float flipper1 = 1.0+pow(lngth1, 2);
        vec3 ambientvec1 = Light1.La*Material.Ka;
        vec3 diffusevec1 = Material.Kd*Light1.Ld*diffuse1/flipper1;
        vec3 specularvec1 = Material.Ks*Light1.Ld*specular1/flipper1;
        vec3 intensity1 = ambientvec1 + diffusevec1 + specularvec1;

        // LIGHT2
        // Difuse Lighting
        vec3 toLight2 = normalize(vec3(Light2.Position) - vec3(vPosition));
        vec3 normal2 = normalize(vNormal);
        float diffuse2 = max(0.0, dot(normal2, toLight2));

        // Specular Lighting
        vec3 toV2= -normalize(vec3(vPosition));
        vec3 h2 = normalize(toV2 + toLight2);
        float specular2 = pow(max(0.0, dot(h2, normal2)), Material.Shininess);
        float lngth2 = length(vec3(Light2.Position) - vec3(vPosition));
        float flipper2 = 1.0+pow(lngth2, 2);
        vec3 ambientvec2 = Light2.La*Material.Ka;
        vec3 diffusevec2 = Material.Kd*Light2.Ld*diffuse2/flipper2;
        vec3 specularvec2 = Material.Ks*Light2.Ld*specular2/flipper2;
        vec3 intensity2 = ambientvec2 + diffusevec2 + specularvec2;
        
        // Combine all intensities
        vec3 combinedIntesity = Light2.Intensity*(intensity1 + intensity2 + intensity0);
        FragColor = vec4(combinedIntesity.x, combinedIntesity.y, combinedIntesity.z, 1.0); 
    }


}