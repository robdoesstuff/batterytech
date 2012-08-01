// Line vertex shader

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

attribute vec3 vPosition;
attribute vec4 vColor;

varying vec4 uvColor;

void main() {
	gl_Position = projection_matrix * modelview_matrix * vec4(vPosition, 1.0);
	uvColor = vColor;
	gl_PointSize = 5.0;
}              