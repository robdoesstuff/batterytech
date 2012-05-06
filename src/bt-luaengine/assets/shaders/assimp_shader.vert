// Static object vertex shader

#define MAX_POINT_LIGHTS 0

struct directional_light {
	vec3 direction;
	vec3 halfplane;
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
};

//struct point_light {
//	vec3 position;
//	vec3 attenuations;
//	vec4 ambient_color;
//	vec4 diffuse_color;
//	vec4 specular_color;
//};

struct material_properties {
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
	float specular_exponent;
};

const float c_zero = 0.0;
const float c_one = 1.0;

uniform material_properties material;
uniform directional_light dirLight;
//uniform point_light pointLight[5];
//uniform int pointLightCount;
uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat4 shadow_matrix;
uniform mat3 inv_matrix;
uniform vec4 fog_and_uv_offset;
uniform vec3 cameraPos;

attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec3 vUV;

varying vec2 uvCoord;
varying vec4 vColor;
//varying float fogAmount;
varying vec4 shadowCoord;

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

//vec4 compute_point_light(int lightIdx, vec3 eye, vec3 ecPosition3, vec3 normal) {
//	vec4 computed_color = vec4(c_zero, c_zero, c_zero, c_zero);
//	float nDotVP; 	// normal . light direction
//	float nDotHV; 	// normal . light half vec
//	float pf;		// power factor
//	float attenuation;	// computed attenuation factor
//	float d;		// distance from surface to light source
//	vec3 VP;		// direction from surface to light position
//	vec3 halfVector;	// direction of maximum highlights
//	
//	// compute vector from surface to light position (both in eye coordinates)
//	VP = vec3(pointLight[lightIdx].position) - ecPosition3;
//	// compute distance between surface and light position
//	d = length(VP);
//	// normalize the vector from surface to light position
//	VP = normalize(VP);
//	// compute attenuation (x = constant, y = linear, z = quadratic)
//	attenuation = 1.0 / (pointLight[lightIdx].attenuations.x +
//						pointLight[lightIdx].attenuations.y * d +
//						pointLight[lightIdx].attenuations.z * d * d);
//	halfVector = normalize(VP + eye);
//	nDotVP = max(0.0, dot(normal, VP));
//	nDotHV = max(0.0, dot(normal, halfVector));
//	if (nDotVP == 0.0)
//		pf = 0.0;
//	else
//		pf = pow(nDotHV, material.specular_exponent);
//	computed_color += pointLight[lightIdx].ambient_color * material.ambient_color * attenuation;
//	computed_color += pointLight[lightIdx].diffuse_color * material.diffuse_color * nDotVP * attenuation;
//	computed_color += pointLight[lightIdx].specular_color * material.specular_color * pf * attenuation;
//	return computed_color;
//}

//float compute_fog_amount(vec4 ecVert) {
//	// if fog_far is 0, fog is disabled, but we want to return 1,1,1,1 in that case 
//	// so we keep a value of 1 for fogDisable that we'll add to the fogAmt of 0.  Otherwise it will be 0 and won't affect the fog amount.
//	float fog_far = fog_and_uv_offset.y;
//	float fogDisable = 1.0-clamp(fog_far, 0.0, 1.0);
//	float fogAmt = clamp(1.0-(-ecVert.z / fog_far+0.001), 0.0, 1.0);
//	return fogAmt+fogDisable;
//}

void main() {
	// flip V
	vec2 uv_offset = fog_and_uv_offset.zw;
	uvCoord = vec2(vUV.x + uv_offset.x, (1.0-(vUV.y+uv_offset.y)));
	vec4 ecPosition = modelview_matrix * vec4(vPosition.xyz, 1.0);
	vec3 ecNormal = normalize(inv_matrix * vNormal);
	vec3 ecPos3 = (vec3(ecPosition)) / ecPosition.w;
	shadowCoord = shadow_matrix * ecPosition;
	// perspective division of homogenized shadow coord should bring it into 0-1 range
	shadowCoord = shadowCoord / shadowCoord.w;
	gl_Position = projection_matrix * ecPosition;
	vColor = compute_directional_light(ecNormal);
	// vColor = vec4(0,0,0,0);
	//for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
	//	if (i < pointLightCount) {
	//		vColor += compute_point_light(i, ecCamera, ecPos3, ecNormal);
	//	}
	//}
	//fogAmount = compute_fog_amount(ecPosition);
	//fogAmount *= fogAmount;
}              