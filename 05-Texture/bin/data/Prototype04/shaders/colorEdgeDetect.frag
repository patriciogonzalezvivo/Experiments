uniform sampler2DRect tex0;
uniform float radio;

vec2 offset[9];

void main(void){
    vec2 st = gl_TexCoord[0].st;
    float radius = 0.5+radio * 2.0;
    float yFactor = 1.0;
    float uvFactor = 1.0;

    mat3 yuv2rgb = mat3(    1.0, 1.0, 1.0,
                            0.0,-0.39465,2.03211,
                            1.13983,-0.58060,0.0);

    vec3 color = texture2DRect(tex0, st).rgb;
    vec3 colorYUV = yuv2rgb * color;

    colorYUV.x *= yFactor;
    colorYUV.yz *= uvFactor;

	offset[0] = vec2(-radius, -radius);
	offset[1] = vec2(0.0, -radius);
	offset[2] = vec2(radius, -radius);
	offset[3] = vec2(-radius, 0.0);
	offset[4] = vec2(0.0, 0.0);
	offset[5] = vec2(radius, 0.0);
	offset[6] = vec2(-radius, radius);
	offset[7] = vec2(0.0, radius);
	offset[8] = vec2(radius, radius);

    float maxDist = 0.0;
    for (int i = 0; i < 9; i++){
    	if (i != 4){
    		vec3 sample = texture2DRect(tex0, st + offset[i]).rgb;
            vec3 sampleYUV = yuv2rgb * sample;
            sampleYUV.x *= yFactor;
            sampleYUV.yz *= uvFactor;

            float d = distance(colorYUV,sampleYUV);
            if (d > maxDist){
                maxDist = d;
            }
    	}
    }
	
    gl_FragColor = vec4(mix(vec3(0.0),color,maxDist),1.0);
}
