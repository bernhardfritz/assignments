#version 330

in vec2 UVcoords;

uniform sampler2D myTextureSampler;

out vec4 FragColor;

void main()
{
    FragColor = texture(myTextureSampler, UVcoords);
}
