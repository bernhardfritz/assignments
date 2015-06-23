#version 330

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec2 UV;

out vec4 vColor;
out vec3 vNormal;
out vec3 vCamera;
out vec2 UVcoords;
out vec3 world_coord;
out vec3 eye_coord;

void main()
{
   vec4 world_pos = ModelMatrix * vec4(Position, 1.0);
   vec4 eye_pos = ViewMatrix * world_pos;
   vec4 clip_pos = ProjectionMatrix * eye_pos;

   world_coord = world_pos.xyz;
   eye_coord = eye_pos.xyz;

   vColor = vec4(Color, 1.0);
   mat3 NormalMatrix = mat3(transpose(inverse(ModelMatrix)));
   vNormal = normalize(NormalMatrix * Normal);
   vCamera = vec3(ViewMatrix*ModelMatrix * vec4(Position, 1.0));
   UVcoords = UV;

   gl_Position = clip_pos;
}
