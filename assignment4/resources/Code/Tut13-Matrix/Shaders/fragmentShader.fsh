#version 130

// This is the same old fragment shader we've been using

in vec4 color;
out vec4 fColor;

void main () {
	fColor.r =abs(color.r);
	fColor.g = abs(color.g);
	fColor.b = abs(color.b);
	fColor.a = 1.0f;
}