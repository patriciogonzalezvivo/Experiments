#version 120

uniform float normalScale;
uniform float useRim;

varying vec3 vPos;
varying vec3 vePos;
varying vec2 vTexCoord;
varying vec2 vUv;
varying vec3 vNormal;
varying vec3 vEye;
varying vec3 vU;
varying vec2 vN;

void main() {
    vU = normalize( vec3( gl_ModelViewMatrix * vec4( gl_Vertex.xyz, 1.0 ) ) );

    if( normalScale > 0. ) {
        vec3 n = normalize( gl_NormalMatrix * gl_Normal );
        vec3 r = reflect( vU, n );
        float m = 2.0 * sqrt( r.x * r.x + r.y * r.y + ( r.z + 1.0 ) * ( r.z+1.0 ) );
        vN = vec2(r.x / m + 0.5, r.y / m + 0.5);
    } else {
        vN = vec2(0.);
    }

    vTexCoord = gl_MultiTexCoord0.xy;
    vUv = vTexCoord;

    vPos = gl_Vertex.xyz;
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4( gl_Vertex.xyz, 1.0 );

    vNormal = normalize(gl_NormalMatrix * gl_Normal);

    if( useRim > 0. ) {
        vEye = ( gl_ModelViewMatrix * vec4( gl_Vertex.xyz, 1.0 ) ).xyz;
    } else {
        vEye = vec3( 0. );
    }
}