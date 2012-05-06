// draw no fragments.

#if __VERSION__ >= 130
precision mediump float;
#endif
#ifdef GL_ES
precision mediump float;
#endif

varying vec4 position;

vec4 pack (float depth) {
	const vec4 bitSh = vec4(1.0,32.0,64.0*32.0,64.0 * 32.0 * 32.0);
	const vec4 bitMsk = vec4(1.0 / 32.0, 1.0 / 64.0, 1.0 / 32.0, 0.0);
    vec4 comp = fract(depth * bitSh);
    comp -= comp.yzww * bitMsk;
    return comp;
}

void main() {
	float normalizedDistance = position.z / position.w;
	// scale it from 0-1
	normalizedDistance = (normalizedDistance + 1.0) / 2.0;
	gl_FragColor = pack(normalizedDistance);
}