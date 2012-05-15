// GPU accelerated skinned animation object vertex shader

#define MAX_POINT_LIGHTS 0

struct directional_light {
	vec3 direction;
	vec3 halfplane;
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
};

// struct point_light {
// 	vec3 position;
// 	vec3 attenuations;
// 	vec4 ambient_color;
// 	vec4 diffuse_color;
// 	vec4 specular_color;
// };

struct material_properties {
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
	float specular_exponent;
};

const float c_zero = 0.0;
const float c_one  = 1.0;

uniform material_properties material;
uniform directional_light dirLight;
// uniform point_light pointLight[MAX_POINT_LIGHTS];
// uniform int pointLightCount;
uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
#ifdef SHADOWMAP
uniform mat4 shadow_matrix;
#endif
uniform mat3 inv_matrix;
uniform mat4 bone_matrices[31];
uniform vec4 fog_and_uv_offset;
uniform vec3 cameraPos;

attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec3 vUV;
attribute vec4 vBones;
attribute vec4 vWeights;

varying vec2 uvCoord;
varying vec4 vColor;
// varying float fogAmount;
#ifdef SHADOWMAP
varying vec4 shadowCoord;
#endif

vec4 compute_directional_light(vec3 normal) {
	vec4 computed_color = vec4(c_zero, c_zero, c_zero, c_zero);
	float ndotl;
	float ndoth;
	ndotl = max(c_zero, dot(normal, dirLight.direction));
	ndoth = max(c_zero, dot(normal, dirLight.halfplane));
	computed_color += (dirLight.ambient_color * material.ambient_color);
	computed_color += (ndotl * dirLight.diffuse_color * material.diffuse_color);
	if (ndoth > c_zero) {
		computed_color += (pow(ndoth, material.specular_exponent) * material.specular_color * dirLight.specular_color);
	}
	return computed_color;
}

void main() {
	// flip V
	vec2 uv_offset = fog_and_uv_offset.zw;
	uvCoord = vec2(vUV.x + uv_offset.x, (1.0-(vUV.y+uv_offset.y)));

	vec4 vpos = vec4(vPosition.xyz, 1.0);
	vec4 skinnedPos = (bone_matrices[int(vBones.x)] * vpos * vWeights.x) +
					  (bone_matrices[int(vBones.y)] * vpos * vWeights.y) +
					  (bone_matrices[int(vBones.z)] * vpos * vWeights.z) +
					  (bone_matrices[int(vBones.w)] * vpos * vWeights.w) ;
	
	vec4 ecPosition = modelview_matrix * skinnedPos;
	vec3 ecPos3 = (vec3(ecPosition)) / ecPosition.w;
#ifdef SHADOWMAP
	shadowCoord = shadow_matrix * ecPosition;
	// perspective division of homogenized shadow coord should bring it into 0-1 range
	shadowCoord = shadowCoord / shadowCoord.w;
#endif
	gl_Position = projection_matrix * ecPosition;
	
	vec4 vnorm = vec4(vNormal.xyz, 0.0);
	vec4 skinnedNorm = (bone_matrices[int(vBones.x)] * vnorm * vWeights.x) +
					   (bone_matrices[int(vBones.y)] * vnorm * vWeights.y) +
					   (bone_matrices[int(vBones.z)] * vnorm * vWeights.z) +
					   (bone_matrices[int(vBones.w)] * vnorm * vWeights.w);
	vec3 ecNormal = normalize(inv_matrix * skinnedNorm.xyz);
	// vec3 ecCamera = (modelview_matrix * vec4(cameraPos, 1.0)).xyz;
	
	vColor = compute_directional_light(ecNormal);
}              