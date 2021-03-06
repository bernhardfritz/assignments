#version 330

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec4 LightPosition;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec3 Normal;

out vec4 vColor;
out vec3 vNormal;
out vec3 vLight;
out vec3 vCamera;

void main()
{
   gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(Position, 1.0);
   vColor = vec4(Color, 1.0);
   mat3 NormalMatrix = mat3(transpose(inverse(ModelMatrix)));
   vNormal = normalize(NormalMatrix * Normal);
   vLight = normalize(LightPosition.xyz);
   vCamera = vec3(ViewMatrix*ModelMatrix * vec4(Position, 1.0));
}
