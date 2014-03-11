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

MaterialInfo Material;

layout (location = 0) out vec4 FragColor;

void main()
{
	// Difuse Lighting
	vec3 toLight = normalize(vec3(Light0.Position) - vec3(vPosition));
	vec3 normal = normalize(vNormal);
	float diffuse = max(0.0, dot(normal, toLight));

    // Specular Lighting
    vec3 toV= -normalize(vec3(vPosition));
    vec3 h = normalize(toV + toLight);
    float specular = pow(max(0.0, dot(h, normal)), Material.Shininess);
    float lngth = length(vec3(Light0.Position) - vec3(vPosition));
    float flipper = 1.0+pow(lngth, 2);
	vec3 intensity = Light0.La+Light0.Intensity*diffuse/flipper+ (Material.Ks * specular/flipper);

	FragColor = vec4(intensity.x, intensity.y, intensity.z, 1.0);
}