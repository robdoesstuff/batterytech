// Basic quad fragment shader

precision mediump float;
        
uniform sampler2D tex;
uniform vec4 colorFilter;

varying vec2 uvCoord;

void main() {
	gl_FragColor = texture2D(tex, uvCoord) * colorFilter;
}