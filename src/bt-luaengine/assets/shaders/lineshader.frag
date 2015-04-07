// Line fragment shader

#if __VERSION__ >= 130
precision mediump float;
#endif
#ifdef GL_ES
precision mediump float;
#endif
        
varying vec4 uvColor;

void main() {
	gl_FragColor = uvColor;
}