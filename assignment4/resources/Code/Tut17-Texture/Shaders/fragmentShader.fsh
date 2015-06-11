#version 130

in vec3 fN;
in vec3 fL;
in vec3 fE;

// NEW! Texture stuff... the "sampler2D" is the image
in vec2 texCoord;				
uniform sampler2D texture;

out vec4 fColor;

void main () {
	vec3 N = normalize(fN);		// Noramlize N
	vec3 L = normalize(fL);		// Normalize L
	vec3 E = normalize(-fE);	// Reverse (E)ye vector (or view vector)
	vec3 H = normalize(L + E);	// Create the half vector	

	// NEW Diffuse component - main idea: use texture for color
	float diffuse_intensity = max(dot(N, L), 0.0);
	vec4 diffuse_final = diffuse_intensity*texture2D(texture, texCoord);

	// Specular component remains the same
	float spec_intensity = pow(max(dot(N, H), 0.0), 3);
	vec4 spec_final = spec_intensity*vec4(0.1, 0.1, 0.1, 1.0);
	
	fColor = diffuse_final + spec_final;
}