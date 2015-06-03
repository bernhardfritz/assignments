#version 330

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec4 LightPosition;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec3 Normal;

out VS_OUT {
  vec3 normal;
} vs_out;

void main()
{
   gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(Position.x, Position.y, Position.z, 1.0);
   mat3 NormalMatrix = mat3(transpose(inverse(ViewMatrix*ModelMatrix)));
   vs_out.normal = normalize(vec3(ProjectionMatrix * vec4(NormalMatrix * Normal, 1.0)));
   vec3 vLight = normalize(LightPosition.xyz);
}
