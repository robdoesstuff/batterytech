// Particle vertex shader

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

attribute vec3 vPosition;
attribute vec2 uvMap;
attribute float alpha;

varying vec3 uvCoordAlpha;

void main() {
	uvCoordAlpha = vec3(uvMap.x, (1.0-uvMap.y), alpha);
	gl_Position = projection_matrix * modelview_matrix * vec4(vPosition.xyz, 1.0);
}              