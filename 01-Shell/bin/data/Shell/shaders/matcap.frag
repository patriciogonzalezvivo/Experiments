#version 120
uniform sampler2D tMatCap;
uniform sampler2D tNormal;

uniform float _useUV;

uniform float normalScale;
uniform float normalRepeat;

uniform float useRim;
uniform float transparency;

varying vec2 Uv;
varying vec3 Tangent;
varying vec3 Binormal;
varying vec3 Normal;
varying vec3 Eye;
varying vec3 U;
varying vec2 N;

varying vec3 Pos;

vec3 getTriPlanarBlend(vec3 _wNorm){
    vec3 blending = abs( _wNorm );
    blending = normalize(max(blending, 0.00001));
    float b = (blending.x + blending.y + blending.z);
    blending /= vec3(b, b, b);
    return blending;
}

vec2 barrelDistortion(vec2 coord, float amt) {
    vec2 cc = coord - 0.5;
    float dist = dot(cc, cc);
    return coord + cc * dist * amt;
}

float sat( float t ){
    return clamp( t, 0.0, 1.0 );
}

float linterp( float t ) {
    return sat( 1.0 - abs( 2.0*t - 1.0 ) );
}

float remap( float t, float a, float b ) {
    return sat( (t - a) / (b - a) );
}

vec3 spectrum_offset( float t ) {
    vec3 ret;
    float lo = step(t,0.5);
    float hi = 1.0-lo;
    float w = linterp( remap( t, 1.0/6.0, 5.0/6.0 ) );
    ret = vec3(lo,1.0,hi) * vec3(1.0-w, w, 1.0-w);
    
    return pow( ret, vec3(1.0/2.2) );
}

uniform float chroma;
const int num_iter = 12;
const float reci_num_iter_f = 1.0 / float(num_iter);

vec3 chromaAb( vec2 _pos, float _amount ){
    vec3 sumcol = vec3(0.0);
    vec3 sumw = vec3(0.0);
    for ( int i=0; i<num_iter;++i ){
        float t = float(i) * reci_num_iter_f;
        vec3 w = spectrum_offset( t );
        sumw += w;
        sumcol += w * texture2D(tMatCap, barrelDistortion(_pos, _amount*t*(chroma*10.0) ) ).rgb;
    }
    return sumcol.rgb / sumw;
}

void main() {
    vec3 finalNormal = Normal;
    vec2 calculatedNormal = N;

    if( normalScale > 0. ) {
        if(_useUV == 1.){
            vec3 normalTex = texture2D( tNormal, Uv * normalRepeat*100.0).xyz;
            normalTex = normalTex * 2.0 - 1.0;
            normalTex.xy *= normalScale;
            normalTex.y *= -1.;
            normalTex = normalize( normalTex );

            mat3 tsb = mat3( normalize( Tangent ), normalize( Binormal ), normalize( Normal ) );
            finalNormal = tsb * normalTex;

        } else {
            vec3 blending = getTriPlanarBlend(Normal);
            vec3 xaxis = texture2D( tNormal, Pos.yz * normalRepeat).rgb;
            vec3 yaxis = texture2D( tNormal, Pos.xz * normalRepeat).rgb;
            vec3 zaxis = texture2D( tNormal, Pos.xy * normalRepeat).rgb;
            vec3 normalTex = xaxis * blending.x + xaxis * blending.y + zaxis * blending.z;
            normalTex = normalTex * 2.0 - 1.0;
            normalTex.xy *= normalScale;
            normalTex.y *= -1.;
            normalTex = normalize( normalTex );

            vec3 T = vec3(1.0,0.0,0.0);
            vec3 BT = normalize( cross( Normal, T ) * 1.0 );

            mat3 tsb = mat3( normalize( T ), normalize( BT ), normalize( Normal ) );
            finalNormal = tsb * normalTex;
        }
    }

    vec3 r = reflect( U, normalize( finalNormal ) );
    float m = 2.0 * sqrt( r.x * r.x + r.y * r.y + ( r.z + 1.0 ) * ( r.z+1.0 ) );
    calculatedNormal = vec2( r.x / m + 0.5,  r.y / m + 0.5 );
    calculatedNormal.y = 1.0-calculatedNormal.y;

    float cosTheta = abs(dot( U, finalNormal));
    float exp = pow(1.0-cosTheta, 4.0);
    float fresnel = exp;

    vec3 base = vec3(0.0);

    if(chroma>0.0){
        float f = abs(dot( U, finalNormal));
        base = chromaAb(calculatedNormal,f);
    } else {
        base = texture2D( tMatCap, calculatedNormal ).rgb;
    }

    if( useRim > 0. ) {
        float f = useRim * ( 1. - smoothstep( 0.0, 1., cosTheta ) );
        base += vec3( f );
    }

    if(transparency>0.0){
        float thickness = abs(pow(2.0,(-10.0-Pos.z)*0.11));
            float sigma = 5.0;
            float intensity = fresnel * exp(sigma * thickness);
            vec4 glass = vec4(base,mix(gl_Color.a,intensity,transparency) );
            gl_FragColor = vec4( base, mix(gl_Color.a,intensity,transparency) );
        } else {
            gl_FragColor = vec4( base, gl_Color.a );
        }
    }