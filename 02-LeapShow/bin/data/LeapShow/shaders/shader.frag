uniform sampler2DRect tex0;
uniform sampler2DRect tex1;

uniform vec2 resolution;
uniform vec2 textureResolution;
uniform float chroma;
uniform float grain;

uniform float time;
uniform float grainFreq;
uniform float grainPct;

uniform float bloomAmount;
uniform float bloomSize;

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

const int num_iter = 12;
const float reci_num_iter_f = 1.0 / float(num_iter);

vec3 pickColor( vec2 _pos ){
	vec3 color = texture2DRect( tex0, _pos*resolution.xy ).rgb;
	float mag = length(_pos - vec2(.5));

	if (grain>0.0){
		vec3 disp = texture2DRect(tex1, _pos*textureResolution.xy ).rgb*mag;

		_pos.x += disp.x*grain;
		_pos.x -= disp.y*grain;
		
		color *= 0.1;
		color += texture2DRect( tex0, _pos*resolution.xy ).rgb*0.9;
	}

	vec4 sum = vec4(0);
    if(bloomAmount > 0.0){
    	int offset = 1+int(bloomSize*3.0);
    	for(int i= -offset ;i < offset; i++){
        	for (int j = -offset; j < offset; j++){
            	float m = pow(max(0.,1.-length(vec2(float(i)/float(offset), float(j)/float(offset)) - vec2(0.))),2.);
            	sum += texture2DRect(tex0, gl_TexCoord[0].st + vec2(float(j), float(i)) )*m;
        	}
    	}
    	color += mix(vec3(0.),sum.rgb,bloomAmount);
    }
	
	return color;
}

float rand(vec2 co){
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float rand(float c){
	return rand(vec2(c,1.0));
}

void main(){
	vec2 st = gl_FragCoord.xy/resolution.xy;
	st.y = 1.0 - st.y;
	
	vec3 sumcol = vec3(0.0);
	vec3 sumw = vec3(0.0);	
	
	vec3 color = vec3(0.0);
	
	if ( chroma != 0.0 ){
		for ( int i=0; i<num_iter;++i ){
			float t = float(i) * reci_num_iter_f;
			vec3 w = spectrum_offset( t );
			sumw += w;
			sumcol += w * pickColor( barrelDistortion(st, chroma*t) );
		}
		// color = (sumcol.rgb+mix(vec3(0.),sum.rgb,bloomAmount))/sumw;
		color = sumcol.rgb / sumw;
	} else {
		color = pickColor( st ); 
	}
		
		if ( grainPct != 0.0){
			float t = float(time * grainFreq);
			float value;
			
			value = rand(st+t*.00001);
			color *= (1.0+(value-.2)*grainPct);
		}
		
		gl_FragColor = vec4(color,gl_Color.a);
	}
	