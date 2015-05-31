#version 130

in vec3 fN;
in vec3 fL;

in vec3 fE;				// NEW!  Coming in from the vertex shader

out vec4 fColor;

void main () {
	vec3 N = normalize(fN);
	vec3 L = normalize(fL);

	vec3 E = normalize(-fE);	// NEW!	Reverse E
	vec3 H = normalize(L + E);	// NEW! Create the half vector	

	// Diffuse component
	float diffuse_intensity = max(dot(N, L), 0.0);
	//vec4 diffuse_final = diffuse_intensity*vec4(0.1, 0.8, 0.1, 1.0);		// NEW!! Multiply the diffuse intensity times a color
	vec4 diffuse_final = diffuse_intensity*vec4(0.1, 0.1, 0.1, 1.0);

	// NEW! Specular component
	float spec_intensity = pow(max(dot(N, H), 0.0), 30);
	//vec4 spec_final = spec_intensity*vec4(0.7, 0.9, 0.7, 1.0);
	vec4 spec_final = spec_intensity*vec4(0.9, 0.1, 0.1, 1.0);
	
	fColor = diffuse_final + spec_final;
}