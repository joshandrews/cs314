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

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;      // we keep a MV matrix without the translation component to apply to vectors
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;               // ModelViewProjection Matrix

uniform vec3 gem_pos; // location of the gem
uniform float gem_rad; // radius of the gem
uniform float arm_blow; // radius of the gem

void main()
{
    // determine vertex color
    vec3 tnorm     = normalize( NormalMatrix * Normal );
    vec3 s         = normalize( vec3(Light.Position) ); // incident vector

    /********************************
     * Your code goes here
     ********************************/
    LightIntensity = dot(s, tnorm) * Material.Ka * 0.4 + Material.Ka * 0.6 * Position + vec3(0.8,0.8,0.8); // Getting coloured and lightened so that it is more easily seen
    
    // First I calculate the distance to the vertex
    vec4 pos = MVP * vec4(Position, 1.0);
    float dx = Position.x - gem_pos.x;
    float dy = Position.y - gem_pos.y;
    float dz = Position.z - gem_pos.z;
    float dist = sqrt((dx*dx)+(dy*dy)+(dz*dz));
    
    // Then if the distance is inside the gem sphere
    if (dist <= gem_rad) {
    
    	// We set the vertices that are inside the sphere to be on the sphere
    	if (dist != 0) {
			gl_Position = MVP * vec4((Position-gem_pos)*gem_rad/dist + gem_pos, 1.0);
		}
		else {
			gl_Position = MVP * vec4(Position, 1.0);
		}
	}
	else {
		vec4 temp3 = MVP * vec4(Position, 1.0);
		if (mod(arm_blow, 4) < 1) {
			if (Position.x < 0 && Position.y < 0) {
				temp3.y = temp3.y+mod(arm_blow, 4);
			}
		}
		if (mod(arm_blow, 4) > 1 && mod(arm_blow, 4) < 2) {
			if (Position.x < 0 && Position.y < 0) {
				temp3.y = temp3.y+2-mod(arm_blow, 4);
			}
		}
		if (mod(arm_blow, 4) > 2 && mod(arm_blow, 4) < 3) {
			if (Position.x > 0 && Position.y < 0) {
				temp3.y = temp3.y-2+mod(arm_blow, 4);
			}
		}
		if (mod(arm_blow, 4) > 3 && mod(arm_blow, 4) < 4) {
			if (Position.x > 0 && Position.y < 0) {
				temp3.y = temp3.y+4-mod(arm_blow, 4);
			}
		}
		gl_Position = temp3;
	}

}