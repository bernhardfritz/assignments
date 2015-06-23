#version 330

uniform int LightControl;
uniform int TextureIndex;
uniform vec4 LightColor;
uniform sampler2D myTextureSampler;
uniform vec4 LightPosition;

in vec4 vColor;
in vec3 vNormal;
in vec3 vLight;
in vec3 vCamera;
in vec2 UVcoords;
in vec3 world_coord;
in vec3 eye_coord;

out vec4 FragColor;
void main()
{
    vec4 tempColor = texture(myTextureSampler, UVcoords);

    vec3 N = vNormal;
    vec3 L = normalize(LightPosition.xyz - world_coord);
    float LdotN  = dot(N,L);
    vec3 R = reflect(-L,N);
    float I_D = max(LdotN, 0.0);
    vec4 diffuse = I_D*vColor;
    float I_S = max(pow(dot(normalize(-eye_coord),R),30),0.0);
    vec4 specular = I_S*LightColor;

    if(TextureIndex > -1) {
      diffuse = I_D*tempColor;
    }

    vec4 ambient = vec4(0.1,0.1,0.1,1.0);

    FragColor = (LightControl&1)*ambient + (LightControl&2)*diffuse + (LightControl&4)*specular;
}
