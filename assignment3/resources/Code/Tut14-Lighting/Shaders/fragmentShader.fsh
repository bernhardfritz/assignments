#version 130

in vec3 fN;
in vec3 fL;

out vec4 fColor;

void main () {
	vec3 N = normalize(fN);
	vec3 L = normalize(fL);
	float diffuse_intensity = max(dot(N, L), 0.0);
	//fColor = vec4(diffuse_intensity, diffuse_intensity, diffuse_intensity, 0.5);
	
	if (diffuse_intensity == 0) {
		fColor = vec4(0.05, 0.05, 0.05, 1.0);
	}
	else {
		fColor = vec4(diffuse_intensity, diffuse_intensity, diffuse_intensity, 0.5);
	}
}