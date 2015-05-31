#version 130

in vec4 s_vPosition;
in vec4 s_vNormal;	// NEW! the normal is now coming in too

uniform mat4 mM;	// The matrix to convert into the world coordinate system
uniform mat4 mV;	// The matrix to convert into the camera coordinate system
uniform mat4 mP;	// The perspective matrix for depth

// NEW!  This matrix R is used for all rotations on the model.  We use this to
// calculate rotated normals, but we do NOT include translations of normals (that'd mess it up).
uniform mat4 mRotations;

// NEW!  This is the light - either a directional or point light
uniform vec4 vLight;

// NEW!  Color is gone! The normal and light vectors go to the shader
out vec3 fN;
out vec3 fL;

void main () {
	
	fN = (mRotations*s_vNormal).xyz;	// Rotate the normal! only take the first 3 parts, since fN is a vec3
	fL = vLight.xyz;					// Same here.  Note: if we're rotating the camera, we'd need to rotate the light direction as well.
	
	// New way using matrix multiplication.  From local, to world, to camera, to NDCs
	gl_Position = mP*mV*mM*s_vPosition;

}