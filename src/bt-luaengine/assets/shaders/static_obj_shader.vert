// Static object vertex shader

struct directional_light {
	vec3 direction;
	vec3 halfplane;
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
};

struct material_properties {
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
	float specular_exponent;
};

const float c_zero = 0.0;
const float c_one = 1.0;

uniform material_properties material;
uniform directional_light light;
uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat3 inv_matrix;

attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec2 vUV;

varying vec2 uvCoord;
varying vec4 vColor;

vec4 compute_directional_light(vec3 normal) {
	vec4 computed_color = vec4(c_zero, c_zero, c_zero, c_zero);
	float ndotl;
	float ndoth;
	ndotl = max(c_zero, dot(normal, light.direction));
	ndoth = max(c_zero, dot(normal, light.halfplane));
	computed_color += (light.ambient_color * material.ambient_color);
	computed_color += (ndotl * light.diffuse_color * material.diffuse_color);
	if (ndoth > c_zero) {
		computed_color += (pow(ndoth, material.specular_exponent) * material.specular_color * light.specular_color);
	}
	return computed_color;
}

void main() {
	uvCoord = vUV; 
	vColor = compute_directional_light(normalize(inv_matrix * vNormal));
	//vColor = vec4(1.0, 1.0, 1.0, 1.0);
	gl_Position = projection_matrix * modelview_matrix * vec4(vPosition, 1.0);
}              