#version 330

in vec4 vColor;
in vec3 vNormal;
in vec3 vLight;
in vec3 vCamera;

out vec4 FragColor;

void main()
{
    float I_D = max(0,dot(vNormal,vLight));
    vec4 diffuse = I_D*vColor;

    vec3 E = normalize(-vCamera);
    vec3 H = normalize(vLight + E);
    float I_S = pow(max(dot(vNormal,H),0.0),25);
    vec4 specular = I_S*vec4(1.0,1.0,1.0,1.0); // white light

    FragColor = diffuse + specular;
    //FragColor = diffuse;
    //FragColor = vec4(vColor.r*I_D, vColor.g*I_D, vColor.b*I_D, 0.5);
    //FragColor = vec4(vNormal, 1.0);
    //FragColor = vColor;
}
