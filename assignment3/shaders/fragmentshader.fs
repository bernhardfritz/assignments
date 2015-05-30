#version 330

in vec4 vColor;
in vec3 vNormal;
in vec3 vLight;

out vec4 FragColor;

void main()
{
    float I_D = max(0,dot(vNormal,vLight));
    FragColor = vec4(vColor.r*I_D, vColor.g*I_D, vColor.b*I_D, 0.5);
    //FragColor = vec4(vNormal, 1.0);
    //FragColor = vColor;
}
