#version 330

uniform int LightControl;
uniform int TextureIndex;
uniform vec4 LightColor;
uniform sampler2D myTextureSampler;

in vec4 vColor;
in vec3 vNormal;
in vec3 vLight;
in vec3 vCamera;
in vec2 UVcoords;

out vec4 FragColor;
void main()
{
    vec4 tempColor = texture(myTextureSampler, UVcoords);
    float I_D = max(0,dot(vNormal,vLight));
    vec4 diffuse = I_D*vColor;
    if(TextureIndex > -1) {
      diffuse = I_D*tempColor;
    }

    vec3 E = normalize(-vCamera);
    vec3 H = normalize(vLight + E);
    float I_S = max(dot(vNormal,H),0.0);
    vec4 specular = pow(I_S,30)*LightColor;

    vec4 ambient = vec4(0.1,0.1,0.1,1.0);

    FragColor = (LightControl&1)*ambient + (LightControl&2)*diffuse + (LightControl&4)*specular;
}
