// Assimp Shader with lots of configuration options

#ifndef POINT_LIGHT_COUNT
#define POINT_LIGHT_COUNT 0
#endif

const float c_zero = 0.0;
const float c_one  = 1.0;

uniform vec4 colorFilter;
uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform vec4 fog_and_uv_offset;

attribute vec3 vPosition;
attribute vec2 vUV;

#define HAS_LIGHTS (defined(DIR_LIGHT) || (POINT_LIGHT_COUNT) > 0)
#define FRAGMENT_LIGHTING !defined(VERTEX_LIGHTING)

#if HAS_LIGHTS
	attribute vec3 vNormal;
	uniform mat3 inv_matrix;
	#if FRAGMENT_LIGHTING
		// set up varyings for fragment lighting
		varying vec3 ecNormal;
		#if POINT_LIGHT_COUNT > 0
			varying vec3 ecPos3;
		#endif
	#else
		// vertex lighting
		struct material_properties {
			float specular_exponent;
		};
		uniform material_properties material;
	#endif
#endif

#if POINT_LIGHT_COUNT > 0 && defined(VERTEX_LIGHTING)
// eye-coordinate camera position is modelview * worldcamerapos
uniform vec3 ecCamera;
#endif

#ifdef SHADOWMAP
uniform mat4 shadow_matrix;
#endif

#ifdef HW_SKIN
uniform mat4 bone_matrices[31];
attribute vec4 vBones;
attribute vec4 vWeights;
#endif

varying vec2 uvCoord;
varying vec4 vColor;
#ifdef SHADOWMAP
varying vec4 shadowCoord;
#endif

#if defined(DIR_LIGHT) && defined(VERTEX_LIGHTING)
// The colors in directional_light are premultiplied with the material
struct directional_light {
	vec3 direction;
	vec3 halfplane;
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
};

uniform directional_light dirLight;

vec4 compute_directional_light(vec3 normal) {
	vec4 computed_color = dirLight.ambient_color;
	float ndotl;
	float ndoth;
	ndotl = max(c_zero, dot(normal, dirLight.direction));
	ndoth = max(c_zero, dot(normal, dirLight.halfplane));
	computed_color += (ndotl * dirLight.diffuse_color);
	// no need to check ndoth - it should never be 0 if ndotl isn't 0
	if (ndotl != c_zero) {
		computed_color += (pow(ndoth, material.specular_exponent) * dirLight.specular_color);
	}
	return computed_color;
}

#endif

#if POINT_LIGHT_COUNT > 0 && defined(VERTEX_LIGHTING)
// The colors in point_light are premultiplied with the material
struct point_light {
 	vec3 position;
 	vec3 attenuations;
 	vec4 ambient_color;
 	vec4 diffuse_color;
 	vec4 specular_color;
};

uniform point_light pointLight[POINT_LIGHT_COUNT];

vec4 compute_point_light(int lightIdx, vec3 eye, vec3 ecPosition3, vec3 normal) {
	float nDotVP; 	// normal . light direction
	float nDotHV; 	// normal . light half vec
	float pf;		// power factor
	float attenuation;	// computed attenuation factor
	float d;		// distance from surface to light source
	vec3 VP;		// direction from surface to light position
	vec3 halfVector;	// direction of maximum highlights
	
	// compute vector from surface to light position (both in eye coordinates)
	VP = vec3(pointLight[lightIdx].position) - ecPosition3;
	// compute distance between surface and light position
	d = length(VP);
	// normalize the vector from surface to light position
	VP = normalize(VP);
	// compute attenuation (x = constant, y = linear, z = quadratic)
	attenuation = 1.0 / (pointLight[lightIdx].attenuations.x +
						pointLight[lightIdx].attenuations.y * d +
						pointLight[lightIdx].attenuations.z * d * d);
	nDotVP = max(0.0, dot(normal, VP));
	vec4 computed_color = pointLight[lightIdx].ambient_color * attenuation;
	if (nDotVP != 0.0) {
		computed_color += pointLight[lightIdx].diffuse_color * nDotVP * attenuation;
		halfVector = normalize(VP + eye);
		nDotHV = max(0.0, dot(normal, halfVector));
		if (nDotHV != 0.0) {
			pf = pow(nDotHV, material.specular_exponent);
			computed_color += pointLight[lightIdx].specular_color * pf * attenuation;
		}
	}
	return computed_color;
}
#endif

#ifdef FOG
uniform vec4 fogColor;

float compute_fog_amount(vec4 ecVert) {
	// fog near is fog_and_uv_offset.x, fog far is fog_and_uv_offset.y
	// if fog_far is 0, fog is disabled, but we want to return 1,1,1,1 in that case 
	// so we keep a value of 1 for fogDisable that we'll add to the fogAmt of 0.  Otherwise it will be 0 and won't affect the fog amount.
	float fogDisable = 1.0-clamp(fog_and_uv_offset.y, 0.0, 1.0);
	float fogAmt = clamp(1.0-(-ecVert.z / fog_and_uv_offset.y+0.001), 0.0, 1.0);
	return fogAmt+fogDisable;
}
#endif


// ---------------------------------- main -------------------------------- 
void main() {
	vec2 uv_offset = fog_and_uv_offset.zw;
	// flip V, as is custom
	uvCoord = vec2(vUV.x + uv_offset.x, (1.0-(vUV.y+uv_offset.y)));
	vec4 vpos = vec4(vPosition.xyz, 1.0);
	#if defined(VERTEX_LIGHTING)
	vec3 ecNormal;
	#endif
#ifdef HW_SKIN
	vec4 skinnedPos = (bone_matrices[int(vBones.x)] * vpos * vWeights.x) +
					  (bone_matrices[int(vBones.y)] * vpos * vWeights.y) +
					  (bone_matrices[int(vBones.z)] * vpos * vWeights.z) +
					  (bone_matrices[int(vBones.w)] * vpos * vWeights.w) ;
	vec4 ecPosition = modelview_matrix * skinnedPos;
#if (POINT_LIGHT_COUNT || defined(DIR_LIGHT))
	vec4 vnorm = vec4(vNormal.xyz, 0.0);
	vec4 skinnedNorm = (bone_matrices[int(vBones.x)] * vnorm * vWeights.x) +
					   (bone_matrices[int(vBones.y)] * vnorm * vWeights.y) +
					   (bone_matrices[int(vBones.z)] * vnorm * vWeights.z) +
					   (bone_matrices[int(vBones.w)] * vnorm * vWeights.w);
	ecNormal = normalize(inv_matrix * skinnedNorm.xyz);
#endif
#else
#if (POINT_LIGHT_COUNT || defined(DIR_LIGHT))
	ecNormal = normalize(inv_matrix * vNormal.xyz);
#endif
	vec4 ecPosition = modelview_matrix * vpos;
#endif

	gl_Position = projection_matrix * ecPosition;
	
#ifdef SHADOWMAP
	shadowCoord = shadow_matrix * ecPosition;
	// perspective division of homogenized shadow coord should bring it into 0-1 range
	shadowCoord = shadowCoord / shadowCoord.w;
#endif
	
#if defined(DIR_LIGHT) && defined(VERTEX_LIGHTING)
	vColor = compute_directional_light(ecNormal) * colorFilter;
#else
	vColor = colorFilter;
#endif

#if defined(VERTEX_LIGHTING)
	#if POINT_LIGHT_COUNT > 0
		vec3 ecPos3 = (vec3(ecPosition)) / ecPosition.w;
		vColor += compute_point_light(0, ecCamera, ecPos3, ecNormal);
	#endif
	#if POINT_LIGHT_COUNT > 1
		vColor += compute_point_light(1, ecCamera, ecPos3, ecNormal);
	#endif
	#if POINT_LIGHT_COUNT > 2
		vColor += compute_point_light(2, ecCamera, ecPos3, ecNormal);
	#endif
	#if POINT_LIGHT_COUNT > 3
		vColor += compute_point_light(3, ecCamera, ecPos3, ecNormal);
	#endif
	#if POINT_LIGHT_COUNT > 4
		vColor += compute_point_light(4, ecCamera, ecPos3, ecNormal);
	#endif
#else
	#if POINT_LIGHT_COUNT > 0
	// set up the varying ecPos3 for the frag shader
	ecPos3 = (vec3(ecPosition)) / ecPosition.w;
	#endif
#endif
#ifdef FOG
	float fog_coord = abs(gl_Position.z);
	float fog_far = fog_and_uv_offset.y;
    fog_coord = clamp(fog_coord, 0.0, fog_far);
    float fog = (fog_far - fog_coord)/fog_far;
    fog = clamp(fog, 0.0, 1.0);
    vColor = mix(fogColor, vColor, fog);
#endif
}              