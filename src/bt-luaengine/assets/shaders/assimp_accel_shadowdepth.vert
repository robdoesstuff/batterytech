// Vertex shader to generate the Depth Map
// Used for shadow mapping - generates depth map from the light's viewpoint

// model-view projection matrix
uniform mat4 mvp_matrix;
uniform mat4 bone_matrices[31];

// position of the vertices
attribute vec3 vPosition;
attribute vec4 vBones;
attribute vec4 vWeights;

#ifdef SHADOWMAP_WITH_FRAGMENT
varying vec4 position;
#endif

void main() {
	vec4 pos = vec4(vPosition.xyz, 1.0);
	vec4 skinnedPos = (bone_matrices[int(vBones.x)] * pos * vWeights.x) +
					  (bone_matrices[int(vBones.y)] * pos * vWeights.y) +
					  (bone_matrices[int(vBones.z)] * pos * vWeights.z) +
					  (bone_matrices[int(vBones.w)] * pos * vWeights.w);
	#ifdef SHADOWMAP_WITH_FRAGMENT
	position = mvp_matrix * skinnedPos;
	gl_Position = position;
	#else
	gl_Position = mvp_matrix * skinnedPos;
	#endif
}