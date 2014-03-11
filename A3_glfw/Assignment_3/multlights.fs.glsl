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
    vec3 intensity0 = (Light0.La+Material.Ka)/2+Light0.Intensity*(Material.Kd+Light0.Ld)/2*diffuse0/flipper0+ ((Material.Ks+Light0.Ld)/2 * specular0/flipper0);
    
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
        vec3 intensity1 = (Light1.La+Material.Ka)/2+Light1.Intensity*(Material.Kd+Light1.Ld)/2*diffuse1/flipper1+ ((Material.Ks+Light1.Ld)/2 * specular1/flipper1);
        
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
        vec3 intensity2 = (Light2.La+Material.Ka)/2+Light2.Intensity*(Material.Kd+Light2.Ld)/2*diffuse2/flipper2+ ((Material.Ks+Light2.Ld)/2 * specular2/flipper2);
        
        // Combine all intensities
        vec3 combinedIntesity = (intensity1 + intensity2 + intensity0)/3;
        FragColor = vec4(combinedIntesity.x, combinedIntesity.y, combinedIntesity.z, 1.0); 
    }


}