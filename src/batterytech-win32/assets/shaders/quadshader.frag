precision mediump float;                  
uniform sampler2D tex;				      
varying vec2 uvCoord;

void main() {                                        
	gl_FragColor = texture2D(tex, uvCoord);
}                                        