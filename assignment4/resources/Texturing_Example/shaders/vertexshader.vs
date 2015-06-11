#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 UV;

out vec2 UVcoords;

void main()
{
    gl_Position = vec4(Position.x, Position.y, Position.z, 1.0);
    UVcoords = UV;
}
