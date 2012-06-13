// Assimp Fragment Shader with lots of configuration options

#if __VERSION__ >= 130
precision mediump float;
#endif
#ifdef GL_ES
precision mediump float;
#endif

const float c_one = 1.0;

uniform sampler2D tex;
uniform vec4 colorFilter;
varying vec2 uvCoord;
varying vec4 vColor;

#ifdef DIR_LIGHT
struct directional_light {
	vec3 direction;
	vec3 halfplane;
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
};

uniform directional_light dirLight;
#endif

#ifdef SHADOWMAP
uniform sampler2D shadowTexture; // depth texture generated from light source perspective
uniform vec4 shadowColorEpsilon; // xyz are color, w is epsilon
varying vec4 shadowCoord; // projected coordinates to shadow map
float getShadowFactor(vec4 lightZ) {
	vec4 packedZValue = texture2D(shadowTexture, lightZ.xy);
	// unpack the value stored to get the depth. 
	const vec4 bitShifts =	 vec4(1.0,
						1.0 / 32.0,
						1.0 / (64.0 * 32.0),
						0.0);
	float shadow = dot(packedZValue, bitShifts);
	return float(shadow + shadowColorEpsilon.w > lightZ.z);
}
#endif

#ifdef FOG
uniform vec4 fogColor;
varying float fogAmount;
#endif

void main() {
	vec4 fragColor = texture2D(tex, uvCoord) * colorFilter * vColor;
#ifdef FOG
	vec4 fogMult = vec4(fogAmount, fogAmount, fogAmount, 1.0);
	fragColor = fragColor * fogMult + ((1.0-fogAmount) * fogColor);
#endif
#ifdef SHADOWMAP
	vec4 shadowColor = vec4(shadowColorEpsilon.rgb, c_one);
	float sc = getShadowFactor(shadowCoord);
	// if the projected shadow is further away than the surface the shadow was created from, it must be in the shadow
	shadowColor = (vec4(c_one, c_one, c_one, c_one) - shadowColor) * sc + shadowColor;
	fragColor = fragColor * shadowColor;
#endif
	gl_FragColor = fragColor;
}