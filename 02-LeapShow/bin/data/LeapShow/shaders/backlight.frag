#version 120

varying vec3 eyeVec;
varying float angle;
varying vec3 vNorm;

void main() {
	
	gl_FragColor = gl_Color;

	if(angle > 0.0){
		gl_FragColor.a = pow( angle , 2.0);// / 1.57079632679;
	} else {
		gl_FragColor.a = 0.0;
	}
}