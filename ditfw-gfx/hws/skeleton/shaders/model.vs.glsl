#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec3 VertexColor;

out vec3 LitColor;

uniform vec4 lightViewPos; // Light position in eye coords.
uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ld; // Light source intensity

uniform mat4 wv;
uniform mat3 wit;
uniform mat4 wvp; // Projection * ModelView

void main()
{
  // Convert normal and position to eye coords
  vec3 tnorm = normalize(wit * VertexNormal);
  vec4 eyeCoords = wv * vec4(VertexPosition, 1.0);
  vec3 s = normalize(vec3(lightViewPos - eyeCoords));
  
  // The diffuse shading equation
  LitColor = Ld * Kd * max( dot( s, tnorm ), 0.0 ) * VertexColor;
  
  // Convert position to clip coordinates and pass along
  gl_Position = wvp * vec4(VertexPosition, 1.0);
}