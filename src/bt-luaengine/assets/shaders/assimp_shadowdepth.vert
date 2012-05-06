// Vertex shader to generate the Depth Map
// Used for shadow mapping - generates depth map from the light's viewpoint

// model-view projection matrix
uniform mat4 mvp_matrix;

// position of the vertices
attribute vec4 vPosition; 

varying vec4 position;

void main() {
	vec4 pos = mvp_matrix * vPosition;
	position = pos;
	gl_Position = pos; 
}