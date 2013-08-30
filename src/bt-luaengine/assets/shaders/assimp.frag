// Assimp Fragment Shader with lots of configuration options

#if __VERSION__ >= 130
precision mediump float;
#endif
#ifdef GL_ES
precision mediump float;
#endif

const float c_one = 1.0;
const float c_zero = 0.0;
const vec4 vec4_one = vec4(c_one, c_one, c_one, c_one);
const vec4 vec4_zero = vec4(c_zero, c_zero, c_zero, c_zero);

uniform sampler2D tex;
varying vec2 uvCoord;
varying vec4 vColor;

#ifndef POINT_LIGHT_COUNT
#define POINT_LIGHT_COUNT 0
#endif

#define HAS_LIGHTS (defined(DIR_LIGHT) || (POINT_LIGHT_COUNT) > 0)
#define FRAGMENT_LIGHTING !defined(VERTEX_LIGHTING)

#if HAS_LIGHTS && FRAGMENT_LIGHTING
	varying vec3 ecNormal;
	struct material_properties {
		float specular_exponent;
	};
	uniform material_properties material;
	
	#ifdef DIR_LIGHT
		// The colors in directional_light are premultiplied with the material
		struct directional_light {
			vec3 direction;
			vec3 halfplane;
			vec4 ambient_color;
			vec4 diffuse_color;
			vec4 specular_color;
		};
		
		uniform directional_light dirLight;
		
		vec4 compute_directional_light() {
			vec4 computed_color = dirLight.ambient_color;
			float ndotl;
			float ndoth;
			ndotl = max(c_zero, dot(ecNormal, dirLight.direction));
			ndoth = max(c_zero, dot(ecNormal, dirLight.halfplane));
			computed_color += (ndotl * dirLight.diffuse_color);
			// no need to check ndoth - it should never be 0 if ndotl isn't 0
			if (ndotl != c_zero) {
				computed_color += (pow(ndoth, material.specular_exponent) * dirLight.specular_color);
			}
			return computed_color;
		}
	#endif
	
	#if POINT_LIGHT_COUNT > 0
		// eye-coordinate camera position is modelview * worldcamerapos
		uniform vec3 ecCamera;
		varying vec3 ecPos3;
		// The colors in point_light are premultiplied with the material
		struct point_light {
		 	vec3 position;
		 	vec3 attenuations;
		 	vec4 ambient_color;
		 	vec4 diffuse_color;
		 	vec4 specular_color;
		};
		
		uniform point_light pointLight[POINT_LIGHT_COUNT];
		
		vec4 compute_point_light(int lightIdx) {
			float nDotVP; 	// normal . light direction
			float nDotHV; 	// normal . light half vec
			float pf;		// power factor
			float attenuation;	// computed attenuation factor
			float d;		// distance from surface to light source
			vec3 VP;		// direction from surface to light position
			vec3 halfVector;	// direction of maximum highlights
			
			// compute vector from surface to light position (both in eye coordinates)
			VP = vec3(pointLight[lightIdx].position) - ecPos3;
			// compute distance between surface and light position
			d = length(VP);
			// normalize the vector from surface to light position
			VP = normalize(VP);
			// compute attenuation (x = constant, y = linear, z = quadratic)
			attenuation = 1.0 / (pointLight[lightIdx].attenuations.x +
								pointLight[lightIdx].attenuations.y * d +
								pointLight[lightIdx].attenuations.z * d * d);
			nDotVP = max(0.0, dot(ecNormal, VP));
			vec4 computed_color = pointLight[lightIdx].ambient_color * attenuation;
			if (nDotVP != 0.0) {
				computed_color += pointLight[lightIdx].diffuse_color * nDotVP * attenuation;
				halfVector = normalize(VP + ecCamera);
				nDotHV = max(0.0, dot(ecNormal, halfVector));
				if (nDotHV != 0.0) {
					pf = pow(nDotHV, material.specular_exponent);
					computed_color += pointLight[lightIdx].specular_color * pf * attenuation;
				}
			}
			return computed_color;
		}
	#endif
#endif

#ifdef SHADOWMAP
	uniform sampler2D shadowTexture; // depth texture generated from light source perspective
	uniform vec4 shadowColorEpsilon; // xyz are color, w is epsilon
	varying vec4 shadowCoord; // projected coordinates to shadow map
	float getShadowFactor(vec4 lightZ) {
	#ifdef SHADOWMAP_PACK_RGB
		vec4 packedZValue = texture2D(shadowTexture, lightZ.xy);
		// unpack the value stored to get the depth. 
		const vec4 bitShifts = vec4(1.0,
							1.0 / 32.0,
							1.0 / (64.0 * 32.0),
							0.0);
		float shadow = dot(packedZValue, bitShifts);
	#else
		#ifdef SHADOWMAP_ALPHA_CHANNEL
			float shadow = texture2D(shadowTexture, lightZ.xy).a;
		#else
			float shadow = texture2D(shadowTexture, lightZ.xy).r;
		#endif
	#endif
		return float(shadow + shadowColorEpsilon.w > lightZ.z);
	}
#endif

void main() {
	vec4 fragColor = texture2D(tex, uvCoord) * vColor;
#if FRAGMENT_LIGHTING && HAS_LIGHTS
	// fragment lighting
	#ifdef DIR_LIGHT
		vec4 light = compute_directional_light();
	#else
		vec4 light = vec4_one;
	#endif
	#if POINT_LIGHT_COUNT > 0
		light += compute_point_light(0);
	#endif
	#if POINT_LIGHT_COUNT > 1
		light += compute_point_light(1);
	#endif
	#if POINT_LIGHT_COUNT > 2
		light += compute_point_light(2);
	#endif
	#if POINT_LIGHT_COUNT > 3
		light += compute_point_light(3);
	#endif
	#if POINT_LIGHT_COUNT > 4
		light += compute_point_light(4);
	#endif
	fragColor *= light;
#endif

#ifdef SHADOWMAP
	if (shadowCoord.x > 0.0 && shadowCoord.x < 1.0 && shadowCoord.y > 0.0 && shadowCoord.y < 1.0) {
		vec4 shadowColor = vec4(shadowColorEpsilon.rgb, c_one);
		float sc = getShadowFactor(shadowCoord);
		// if the projected shadow is further away than the surface the shadow was created from, it must be in the shadow
		shadowColor = (vec4_one - shadowColor) * sc + shadowColor;
		fragColor = fragColor * shadowColor;
	}
#endif
	gl_FragColor = fragColor;
}