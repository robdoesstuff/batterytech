// Vertex shader to generate the Depth Map
// Used for shadow mapping - generates depth map from the light's viewpoint

// model-view projection matrix
uniform mat4 mvp_matrix;

// position of the vertices
attribute vec4 vPosition; 

#ifdef SHADOWMAP_WITH_FRAGMENT
varying vec4 position;
#endif

void main() {
	vec4 pos = mvp_matrix * vPosition;
#ifdef SHADOWMAP_WITH_FRAGMENT
	position = pos;
#endif
	gl_Position = pos; 
}