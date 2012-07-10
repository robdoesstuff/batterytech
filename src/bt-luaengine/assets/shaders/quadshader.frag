// Basic quad fragment shader

#if __VERSION__ >= 130
precision mediump float;
#endif
#ifdef GL_ES
precision mediump float;
#endif
        
uniform sampler2D tex;
uniform vec4 colorFilter;

varying vec2 uvCoord;
varying vec4 varColor;

void main() {
	gl_FragColor = texture2D(tex, uvCoord) * colorFilter * varColor;
}