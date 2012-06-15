// Particle fragment shader

#if __VERSION__ >= 130
precision mediump float;
#endif
#ifdef GL_ES
precision mediump float;
#endif
        
uniform sampler2D tex;

varying vec3 uvCoordAlpha;

void main() {
	gl_FragColor = texture2D(tex, uvCoordAlpha.xy)*vec4(1.0,1.0,1.0,uvCoordAlpha.z);
}