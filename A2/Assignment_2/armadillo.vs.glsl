#version 330

layout (location = 0) in vec4 Position;
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
uniform mat4 rotInFrame;               // Rotation in frame for head

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// YOUR CODE HERE
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void main()
{
  // determine vertex color
  vec3 tnorm     = normalize( NormalMatrix * Normal );
  vec3 s         = normalize( vec3(Light.Position) ); // incident vector
  LightIntensity = dot(s, tnorm) * Material.Ka * 0.4 + Material.Ka * 0.6;

  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  // YOUR CODE HERE
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  // the bounds I'm using for head frame
  if (abs(Position.x) < .6 && Position.y > 0.3 && Position.z > -0.3) {
      gl_Position = MVP * rotInFrame * Position;
  }
  else {
      gl_Position = MVP * Position;
  }
}
