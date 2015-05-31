#version 130

in vec4 s_vPosition;
in vec4 s_vColor;
out vec4 color;

void main () {
	// Look, Ma!  I avoided any matrix multiplication!
	// The value of s_vPosition should be between -1.0 and +1.0 (to be visible on the screen)
	color = s_vColor;
	gl_Position = s_vPosition;
}