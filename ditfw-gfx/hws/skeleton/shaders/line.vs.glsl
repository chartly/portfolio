#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexColor;

out vec4 LineColor;

uniform mat4 MVP; // Projection * ModelView

void main()
{
  LineColor = vec4(VertexColor, 1.0);
  gl_Position = MVP * vec4(VertexPosition, 1.0);
}