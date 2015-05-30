#version 330

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec4 LightPosition;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec3 Normal;

//out vec4 vColor;
//out float I_D;

out VS_OUT {
  vec3 normal;
} vs_out;

void main()
{
   gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(Position.x, Position.y, Position.z, 1.0);
   //mat3 NormalMatrix = mat3(transpose(inverse(ViewMatrix * ModelMatrix)));
   vs_out.normal = Normal;
   vec3 vLight = normalize(vec3(LightPosition.x, LightPosition.y, LightPosition.z));
   //I_D = max(0,dot(vs_out.normal,vLight));
   //vColor = vec4(Color, 1.0);
}
