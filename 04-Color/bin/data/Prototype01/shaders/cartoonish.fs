uniform sampler2DRect tex0;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec2 offset[9];

void main(void){
    vec2 st = gl_TexCoord[0].st;
    vec3 color = rgb2hsv( texture2DRect(tex0, st).rgb );

    float radio = 2.0;
	offset[0] = vec2(-radio, -radio);
	offset[1] = vec2(0.0, -radio);
	offset[2] = vec2(radio, -radio);
	offset[3] = vec2(-radio, 0.0);
	offset[4] = vec2(0.0, 0.0);
	offset[5] = vec2(radio, 0.0);
	offset[6] = vec2(-radio, radio);
	offset[7] = vec2(0.0, radio);
	offset[8] = vec2(radio, radio);

	vec3 hsvSample = vec3(0.0);

    for (int i = 0; i < 9; i++){
    	if (i != 4){
    		vec3 tmpHSV = rgb2hsv( texture2DRect(tex0, st + offset[i]).rgb );

    		vec3 diff = abs(color-tmpHSV);
    		if(diff.r > 0.0){
    			hsvSample.r += tmpHSV.r;
    		}

    		if(diff.g > 0.0){
    			hsvSample.g += tmpHSV.g;
    		}

    		if(diff.b > 0.0){
    			hsvSample.b += tmpHSV.b;
    		}
    	}
    }

    hsvSample /= 10.0;

	
    gl_FragColor = vec4(vec3(abs(color-hsvSample).b),1.0);
}
