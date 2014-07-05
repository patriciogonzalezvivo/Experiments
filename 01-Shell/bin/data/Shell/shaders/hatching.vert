#version 120

uniform float repeat;

varying vec3 vNormal;
varying vec2 vUv;
varying vec3 vPosition;

varying vec4 vPos;
varying vec3 pos;

void main() {
	vPos =  vec4( gl_Vertex.xyz, 1.0 );

    vec4 mvPosition = gl_ModelViewMatrix * vec4( gl_Vertex.xyz, 1.0 );
    vPosition = (vec3(mvPosition)) / mvPosition.w;
    
    vNormal = gl_NormalMatrix * gl_Normal;
    
    vUv = gl_MultiTexCoord0.xy * repeat * 100.0;

    gl_Position = gl_ProjectionMatrix * mvPosition;
    gl_FrontColor = gl_Color;

    pos = gl_Position.xyz;
}