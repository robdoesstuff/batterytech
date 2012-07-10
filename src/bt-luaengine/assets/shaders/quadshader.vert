// Basic quad vertex shader

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

attribute vec3 vPosition;
attribute vec2 uvMap;
attribute vec4 vColor;

varying vec2 uvCoord;
varying vec4 varColor;

void main() {
	uvCoord = uvMap;
    varColor = vColor;
	gl_Position = projection_matrix * modelview_matrix * vec4(vPosition, 1.0);
}              