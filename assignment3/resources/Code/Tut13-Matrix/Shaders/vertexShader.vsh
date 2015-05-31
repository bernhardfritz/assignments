#version 130

// Old - but no longer has to be between -1 and +1 because mM (below) can scale/rotate/translate it!
in vec4 s_vPosition;

// New matrix stuff! Note that "uniform" means "for all vertices" - they share this
uniform mat4 mM;	// The matrix to convert into the world coordinate system
uniform mat4 mV;	// The matrix to convert into the camera coordinate system
uniform mat4 mP;	// The perspective matrix for depth

in vec4 s_vColor;
out vec4 color;

void main () {
	color = s_vColor;

	// Old way we did it when it was between -1 and +1 (Normalized Device Coordinates, NDCs)
	// gl_Position = s_vPosition;

	// New way using matrix multiplication.  From local, to world, to camera, to NDCs
	gl_Position = mP*mV*mM*s_vPosition;

}