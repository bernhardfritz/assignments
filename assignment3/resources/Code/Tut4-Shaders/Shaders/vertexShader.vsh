in vec4 s_vPosition;

void main () {
	// Look, Ma!  I avoided any matrix multiplication!
	// The value of s_vPosition should be between -1.0 and +1.0 (to be visible on the screen)
	gl_Position = s_vPosition;
}