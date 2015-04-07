// draw no fragments.

#if __VERSION__ >= 130
precision mediump float;
#endif
#ifdef GL_ES
precision mediump float;
#endif

#ifdef SHADOWMAP_WITH_FRAGMENT
varying vec4 position;

vec4 pack (float depth) {
	const vec4 bitSh = vec4(1.0,32.0,64.0*32.0,64.0 * 32.0 * 32.0);
	const vec4 bitMsk = vec4(1.0 / 32.0, 1.0 / 64.0, 1.0 / 32.0, 0.0);
    vec4 comp = fract(depth * bitSh);
    comp -= comp.yzww * bitMsk;
    return comp;
}
#endif

void main() {
#ifdef SHADOWMAP_WITH_FRAGMENT
	float normalizedDistance = position.z / position.w;
	// scale it from 0-1
	normalizedDistance = (normalizedDistance + 1.0) / 2.0;
	#ifdef SHADOWMAP_PACK_RGB
		gl_FragColor = pack(normalizedDistance);
	#else
		#ifdef SHADOWMAP_ALPHA_CHANNEL
			gl_FragColor = vec4(0.0,0.0,0.0,normalizedDistance);
		#else
			gl_FragColor = vec4(normalizedDistance,0.0,0.0,0.0);
		#endif
	#endif
#else
	gl_FragColor = vec4(0.0,0.0,0.0,0.0);
#endif
}
