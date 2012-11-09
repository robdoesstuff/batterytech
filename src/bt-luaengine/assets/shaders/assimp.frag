// Assimp Fragment Shader with lots of configuration options

#if __VERSION__ >= 130
precision mediump float;
#endif
#ifdef GL_ES
precision mediump float;
#endif

const float c_one = 1.0;
const vec4 vec4_one = vec4(c_one, c_one, c_one, c_one);

uniform sampler2D tex;
varying vec2 uvCoord;
varying vec4 vColor;

#ifdef SHADOWMAP
uniform sampler2D shadowTexture; // depth texture generated from light source perspective
uniform vec4 shadowColorEpsilon; // xyz are color, w is epsilon
varying vec4 shadowCoord; // projected coordinates to shadow map
float getShadowFactor(vec4 lightZ) {
	vec4 packedZValue = texture2D(shadowTexture, lightZ.xy);
	// unpack the value stored to get the depth. 
	const vec4 bitShifts = vec4(1.0,
						1.0 / 32.0,
						1.0 / (64.0 * 32.0),
						0.0);
	float shadow = dot(packedZValue, bitShifts);
	return float(shadow + shadowColorEpsilon.w > lightZ.z);
}
#endif

void main() {
	vec4 fragColor = texture2D(tex, uvCoord) * vColor;
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